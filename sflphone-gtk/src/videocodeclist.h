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

void video_codec_list_init();
void video_codec_list_clear();
void video_codec_list_add(videoCodec_t * c);
void video_codec_set_active(gchar* name);
void video_codec_set_inactive(gchar* name);
guint video_codec_list_get_size();
videoCodec_t * video_codec_list_get(const gchar * name);
videoCodec_t* video_codec_list_get_nth(guint index);

/**
 * Set the prefered video codec first in the video codec list
 * @param index The position in the list of the prefered video codec
 */ 
void video_codec_set_prefered_order(guint index);
void video_codec_list_move_codec_up(guint index);
void video_codec_list_move_codec_down(guint index);
void video_codec_list_update_to_daemon();

#endif /*VIDEOCODECLIST_H_*/
