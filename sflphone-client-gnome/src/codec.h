/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#ifndef __CODEC_H__
#define __CODEC_H__

#include <glib.h>

#include <stdint.h>

/**
 * This structure holds information about some audio codec.
 */
typedef struct {
  char* identifier;
  uint32_t clock_rate;
  uint8_t payload;
  char* mime_type;
  char* mime_subtype;
  double bitrate;
  double bandwidth;
  gchar* description;
  gboolean is_active;
} audio_codec_t;

/**
 * This structure holds information about some video codec.
 * (Same as audio codec for now).
 */
typedef struct {
  char* identifier;
  uint32_t clock_rate;
  uint8_t payload;
  char* mime_type;
  char* mime_subtype;
  double bitrate;
  double bandwidth;
  gchar* description;
  gboolean is_active;
} video_codec_t;

/**
 * To obtain a "polymorphic" kind of behavior, one can choose to read the "codec"
 * field of this union structure to access fields that are common to video and audio codecs.
 */
typedef union {
  struct {
    char* identifier;
    uint32_t clock_rate;
    uint8_t payload;
    char* mime_type;
    char* mime_subtype;
    double bitrate;
    double bandwidth;
    gchar* description;
    gboolean is_active;
  } codec;

  audio_codec_t audio;

  video_codec_t video;

} codec_t;


/**
 * This structure is used to hold various information needed by the functions
 * below and avoid global variables.
 *
 * Opaque structure.
 */
typedef struct {
  GQueue* audio_codec_list;
  GMutex* audio_codec_list_mutex;

  guint number_active; // FIXME Should get rid of that.

  GQueue* video_codec_list;
  GMutex* video_codec_list_mutex;
} codec_library_t;

#endif
