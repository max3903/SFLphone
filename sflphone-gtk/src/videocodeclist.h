/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: marilyne Mercier <marilyne.mercier@polymtl.ca>
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


#ifndef VIDEOCODECLIST_H_
#define VIDEOCODECLIST_H_

#include <gtk/gtk.h>
/** @file videocodeclist.h
  * @brief A list to hold video codecs.
  */

typedef struct {
  int _payload;
  gboolean is_active;
  gchar * name;
}videoCodec_t;

//! Initialize the video codec list
void video_codec_list_init();

//! Clear the video codec list
void video_codec_list_clear();

//! Add a new video codec to the list
/*!
 * \param the new video codec to add
 */
void video_codec_list_add(videoCodec_t * c);

//! Set a video codec as active
/*!
 * \param the name of the video codec
 */
void video_codec_set_active(gchar* name);

//! Set a video codec as inactive
/*!
 * \param the name of the video codec
 */
void video_codec_set_inactive(gchar* name);

//! Get the size of the video codec list
/*!
 * \return the size of the codec list
 */
guint video_codec_list_get_size();

//! Get the size of the video codec list
/*!
 * \return the size of the codec list
 */
videoCodec_t * video_codec_list_get(const gchar * name);

//! Get the size of the video codec list
/*!
 * \return the size of the codec list
 */
videoCodec_t* video_codec_list_get_nth(guint index);

/**
 * Set the prefered video codec first in the video codec list
 * @param index The position in the list of the prefered video codec
 */ 
void video_codec_set_prefered_order(guint index);

//! Move a video codec up in the list
/*!
 * \param the index of the codec to move up
 */
void video_codec_list_move_codec_up(guint index);

//! Move a video codec down in the list
/*!
 * \param the index of the codec to move down
 */
void video_codec_list_move_codec_down(guint index);

//! Send the updated list to the daemon
void video_codec_list_update_to_daemon();

#endif /*VIDEOCODECLIST_H_*/
