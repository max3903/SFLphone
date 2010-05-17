/*
 *  Copyright (C) 2010 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "video_endpoint.h"
#include "sflphone_const.h"

#include <glib.h>
#include <pthread.h>
#include <string.h>

#include "dbus/dbus.h"

sflphone_video_endpoint_t* sflphone_video_init_()
{
  return sflphone_video_init_with_device("");
}

sflphone_video_endpoint_t* sflphone_video_init_with_device(gchar * device)
{
  sflphone_video_endpoint_t* endpt = (sflphone_video_endpoint_t*) malloc(sizeof(sflphone_video_endpoint_t));
  if (endpt == NULL) {
    ERROR("Failed to created video endpoint");
  }

  endpt->device = g_strdup(device);
  endpt->observers = NULL;

  endpt->shm_frame = sflphone_shm_new(device);
  endpt->shm_lock = sflphone_shm_new(device);

  endpt->frame = NULL;

  return endpt;
}

int sflphone_video_free(sflphone_video_endpoint_t* endpt)
{
  sflphone_shm_free(endpt->shm_frame);
  sflphone_shm_free(endpt->shm_lock);

  g_slist_free(endpt->observers);
  g_free(endpt->device);
}

int sflphone_video_set_device(sflphone_video_endpoint_t * endpt, gchar * device)
{
  g_free(endpt->device);
  endpt->device = g_strdup(device);
}

int sflphone_video_open(sflphone_video_endpoint_t* endpt)
{
  // Instruct the deamon to start video capture
  gchar* path = dbus_video_start_local_capture(endpt->device); // FIXME Check return value
  if (g_strcmp0(path, "") == 0) {
    return -1;
  }

  gchar* lock_path = g_strconcat(path, "-rwlock", NULL); // FIXME This is implicit. Should not rely on it like that.

  sflphone_shm_set_path(endpt->shm_frame, path);
  sflphone_shm_set_path(endpt->shm_lock, lock_path);

  g_free(lock_path);

  sflphone_shm_open(endpt->shm_frame);
  sflphone_shm_open(endpt->shm_lock);

  endpt->lock = (pthread_rwlock_t*) sflphone_shm_get_addr(endpt->shm_lock);
  endpt->frame = (uint8_t*) malloc(sflphone_shm_get_size(endpt->shm_lock));
}

int sflphone_video_close(sflphone_video_endpoint_t* endpt)
{
  sflphone_shm_close(endpt->shm_frame);
  sflphone_shm_close(endpt->shm_frame);

  free(endpt->frame);
}

int sflphone_video_add_observer(sflphone_video_endpoint_t* endpt, frame_observer obs)
{
  endpt->observers = g_slist_append(endpt->observers, obs);
}

int sflphone_video_remove_observer(sflphone_video_endpoint_t* endpt, frame_observer obs)
{
  endpt->observers = g_slist_remove(endpt->observers, obs);
}

static void notify_observer(gpointer data, gpointer user_data)
{
  uint8_t* frame = (uint8_t*) user_data;
  frame_observer obs = (frame_observer) data;
  obs(frame);
}

static notify_all_observers(sflphone_video_endpoint_t* endpt, uint8_t* frame)
{
  g_slist_foreach(endpt->observers, (GFunc)notify_observer, (gpointer) frame);
}

static void* capturing_thread(void* params)
{
  sflphone_video_endpoint_t* endpt = (sflphone_video_endpoint_t*) params;
  while (1) {

    // Quickly release the lock
    pthread_rwlock_rdlock(endpt->lock);
      memcpy (endpt->frame, sflphone_shm_get_addr(endpt->shm_frame), sflphone_shm_get_size(endpt->shm_frame));
    pthread_rwlock_unlock(endpt->lock);

    // Notify all observers
    notify_all_observers(endpt, endpt->frame);

    pthread_testcancel(); // Might not work
  }
}

int sflphone_video_start_async(sflphone_video_endpoint_t* endpt)
{
  int rc = pthread_create(&endpt->thread, NULL, &capturing_thread, (void*) endpt);
}

int sflphone_video_stop_async(sflphone_video_endpoint_t* endpt)
{
  pthread_cancel(endpt->thread);
}
