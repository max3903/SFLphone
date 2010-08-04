/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
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

#include "account.h"

#include "CodecList.h"

#include <glib.h>
#include <glib/gprintf.h>

#include "sflphone_const.h"

gchar* framerate_to_string(framerate_t* rate)
{
  return g_strdup_printf("%d/%d", rate->numerator, rate->denominator);
}

framerate_t* framerate_new_from_string(const gchar* rate)
{
  framerate_t* framerate = g_new(framerate_t, 1);
  framerate->numerator = 0;
  framerate->denominator = 1;

  gchar** split = g_strsplit(rate, "/", 2);
  if (split != NULL && split[1] != NULL) {
    framerate->numerator = atoi(g_strstrip(split[0]));
    framerate->denominator = atoi(g_strstrip(split[1]));
  }

  g_strfreev(split);

  return framerate;
}

video_settings_t* video_settings_new()
{
  video_settings_t* settings = g_new(video_settings_t, 1);
  settings->always_offer_video = FALSE;
  settings->device = g_strdup("");
  settings->resolution.height = 1;
  settings->resolution.width = 1;
  settings->framerate.numerator = 0;
  settings->framerate.denominator = 1;
  settings->mutex = g_mutex_new();
}

void video_settings_free(video_settings_t* settings)
{
  g_mutex_lock(settings->mutex);
  {
    g_free(settings->device);
  }
  g_mutex_unlock(settings->mutex);
  g_mutex_free(settings->mutex);
  g_free(settings);
}

void video_settings_print(video_settings_t* settings)
{
  g_mutex_lock(settings->mutex);
  {
  DEBUG(
      "\nResolution : \n"
      "         width : %d\n"
      "         height: %d\n"
      "Framerate : \n"
      "         numerator : %d\n"
      "         denominator: %d\n"
      "Always offering video : %d\n"
      "Device : \"%s\""
      ,settings->resolution.width, settings->resolution.height,
      settings->framerate.numerator, settings->framerate.denominator,
      settings->always_offer_video,
      settings->device);
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_saves(video_settings_t* settings, const gchar* accountID)
{
  g_mutex_lock(settings->mutex);
  {
  dbus_set_video_settings(accountID, settings);
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_set_device(video_settings_t* settings, gchar* device)
{
  g_mutex_lock(settings->mutex);
  {
  g_free(settings->device);
  settings->device = g_strdup(device);
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_set_width(video_settings_t* settings, guint width)
{
  g_mutex_lock(settings->mutex);
  {
  settings->resolution.width = width;
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_set_height(video_settings_t* settings, guint height)
{
  g_mutex_lock(settings->mutex);
  {
  settings->resolution.height = height;
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_set_resolution(video_settings_t* settings, resolution_t* resolution)
{
  g_mutex_lock(settings->mutex);
  {
  settings->resolution.width = resolution->width;
  settings->resolution.height = resolution->height;
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_set_framerate(video_settings_t* settings, framerate_t* framerate)
{
  g_mutex_lock(settings->mutex);
  {
  settings->framerate.numerator = framerate->numerator;
  settings->framerate.denominator = framerate->denominator;
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_set_numerator(video_settings_t* settings, guint numerator)
{
  g_mutex_lock(settings->mutex);
  {
  settings->framerate.numerator = numerator;
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_set_denominator(video_settings_t* settings, guint denominator)
{
  g_mutex_lock(settings->mutex);
  {
  settings->framerate.denominator = denominator;
  }
  g_mutex_unlock(settings->mutex);
}

void video_settings_set_always_offer_video(video_settings_t* settings, gboolean offer)
{
  g_mutex_lock(settings->mutex);
  {
  settings->always_offer_video = offer;
  }
  g_mutex_unlock(settings->mutex);
}

account_t* account_new(gchar* accountID)
{
  account_t* ret =  g_new0(account_t,1);
  ret->accountID = g_strdup (accountID);
  ret->protocol_state_description = NULL;
  ret->properties = NULL;
  ret->credential_information = NULL;
  ret->codecs = codec_library_new();
  ret->video_settings = NULL;

  return ret;
}

void account_free(account_t* account)
{
  g_free(account->accountID);
  video_settings_free(account->video_settings);
  codec_library_free(account->codecs);
}
