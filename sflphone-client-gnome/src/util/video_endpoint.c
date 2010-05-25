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
#include "video_event.h"
#include "sflphone_const.h"

#include <glib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>

#include "dbus/dbus.h"

typedef struct {
  frame_observer cb;
  void * data;
} observer_t;

sflphone_video_endpoint_t* sflphone_video_init()
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
  endpt->frame = NULL;
  endpt->shm_frame = sflphone_shm_new();
  endpt->event_listener = NULL;
  return endpt;
}

int sflphone_video_free(sflphone_video_endpoint_t* endpt)
{
  sflphone_shm_free(endpt->shm_frame);

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
  // Instruct the daemon to start video capture, if it's not already doing so.
  gchar* path = dbus_video_start_local_capture(endpt->device); // FIXME Check return value
  if (g_strcmp0(path, "") == 0) {
    return -1;
  }

  // Initialise event notification for frame capture.
  endpt->event_listener = sflphone_eventfd_init(endpt->device);
  if (endpt->event_listener == NULL) {
    return -1;
  }

  // Set path to the shm device. We can only know until that point.
  sflphone_shm_set_path(endpt->shm_frame, path);
  sflphone_shm_open(endpt->shm_frame);
  endpt->frame = (uint8_t*) malloc(sflphone_shm_get_size(endpt->shm_frame));
}

int sflphone_video_close(sflphone_video_endpoint_t* endpt)
{
  sflphone_eventfd_free(endpt->event_listener);
  sflphone_shm_close(endpt->shm_frame);
  free(endpt->frame);
}

int sflphone_video_add_observer(sflphone_video_endpoint_t* endpt, frame_observer obs, void* data)
{
  observer_t* observer_context = (observer_t*) malloc(sizeof(observer_t));
  if (observer_context == NULL) {
    return -1;
  }

  observer_context->cb = obs;
  observer_context->data = data;

  endpt->observers = g_slist_append(endpt->observers, (gpointer) observer_context);
}

int sflphone_video_remove_observer(sflphone_video_endpoint_t* endpt, frame_observer obs)
{
  // FIXME free the structure correctly.
  endpt->observers = g_slist_remove(endpt->observers, (gpointer) obs);
}

static void notify_observer(gpointer obs, gpointer frame)
{
  observer_t* observer_ctx = (observer_t*) obs;
  frame_observer frame_cb = observer_ctx->cb;
  void * data = observer_ctx->data;

  frame_cb((uint8_t*) frame, data);
}

static notify_all_observers(sflphone_video_endpoint_t* endpt, uint8_t* frame)
{
  DEBUG("Notifying all %d observers", g_slist_length(endpt->observers));

  // g_slist_foreach(endpt->observers, notify_observer, (gpointer) frame);
  GSList* obs;
  for(obs = endpt->observers; obs; obs = g_slist_next(obs)) {
    notify_observer(obs->data, (gpointer) frame);
  }

}

static void* capturing_thread(void* params)
{
  sflphone_video_endpoint_t* endpt = (sflphone_video_endpoint_t*) params;

  while (1) {
    // Blocking call
    // TODO We assume that the only event is NEW_FRAME. This might change if we ever have new events.
    sflphone_eventfd_catch(endpt->event_listener);

    // Go get the frame as fast as possible
    DEBUG("Size %d", sflphone_shm_get_size(endpt->shm_frame))
    memcpy (endpt->frame, sflphone_shm_get_addr(endpt->shm_frame), sflphone_shm_get_size(endpt->shm_frame));

    // Notify all observers
    if (endpt->frame == NULL) {
      ERROR("Frame is null in capturing_thread %s:%d", __FILE__, __LINE__);
    } else {
      DEBUG("Notifying observers.")
      notify_all_observers(endpt, endpt->frame);
    }

    pthread_testcancel();
  }

  DEBUG("Exiting capturing thread");
}

int sflphone_video_start_async(sflphone_video_endpoint_t* endpt)
{
  int rc = pthread_create(&endpt->thread, NULL, &capturing_thread, (void*) endpt);
}

int sflphone_video_stop_async(sflphone_video_endpoint_t* endpt)
{
  pthread_cancel(endpt->thread);
}
