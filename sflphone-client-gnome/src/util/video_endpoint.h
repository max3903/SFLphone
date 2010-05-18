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

#ifndef __VIDEO_ENDPOINT_H__
#define __VIDEO_ENDPOINT_H__

#include "shm.h"

#include <glib.h>
#include <pthread.h>
#include <stdint.h>

/**
 * Callback type for the arrival of new frames.
 */
typedef void(*frame_observer)(uint8_t*);

/**
 * Opaque structure. Do not manipulate directly.
 */
typedef struct {
  GSList* observers;
  gchar* device;
  uint8_t * frame;

  // This below should be opaque.
  sflphone_shm_t* shm_frame;
  sflphone_shm_t* shm_lock;

  pthread_t thread;
  pthread_rwlock_t* lock;
} sflphone_video_endpoint_t;

/**
 * @return A new sflphone_video_endpoint type of object.
 */
sflphone_video_endpoint_t* sflphone_video_init();

/**
 * @return A new sflphone_video_endpoint type of object.
 * @param device The name of the device.
 */
sflphone_video_endpoint_t* sflphone_video_init_with_device(gchar* device);

/**
 * Set the device name and only that.
 * @param endpt An existing sflphone_video_endpoint type of object.
 * @param device The name of the device.
 */
int sflphone_video_set_device(sflphone_video_endpoint_t* endpt, gchar* device);

/**
 * @param endpt An existing sflphone_video_endpoint type of object.
 */
int sflphone_video_free(sflphone_video_endpoint_t* endpt);

/**
 * Open the video device.
 * @param endpt An existing sflphone_video_endpoint type of object.
 */
int sflphone_video_open(sflphone_video_endpoint_t* endpt);

/**
 * Close a video device.
 * @param endpt An existing sflphone_video_endpoint type of object.
 */
int sflphone_video_close(sflphone_video_endpoint_t* endpt);

/**
 * Register a callback function for asynchronous notification upon
 * the arrival of new video frames.
 */
int sflphone_video_add_observer(sflphone_video_endpoint_t* endpt, frame_observer obs);

/**
 * Remove an observer from the observer list.
 * @postcondition The observer that was previously registered won't get notified of new frames.
 */
int sflphone_video_remove_observer(sflphone_video_endpoint_t* endpt, frame_observer obs);

/**
 * Start capturing frames in a non blocking way. Registered observers
 * will get notified when new frames arrive.
 * @param endpt An existing sflphone_video_endpoint type of object.
 */
int sflphone_video_start_async(sflphone_video_endpoint_t* endpt);

/**
 * Stop capturing frames.
 * @param endpt An existing sflphone_video_endpoint type of object.
 */
int sflphone_video_stop_async(sflphone_video_endpoint_t* endpt);

#endif
