/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.net>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.net>
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

#include <codeclist.h>

#include "dbus.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

static codec_library_t* system_library = NULL;

static gint
match_identifier_predicate (gconstpointer a, gconstpointer b)
{
  codec_t * c = (codec_t *) a;
  if (strcmp (c->codec.identifier, (const gchar *) b) == 0)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

static gint
match_mime_subtype_predicate (gconstpointer a, gconstpointer b)
{
  codec_t * c = (codec_t *) a;
  if (strcmp (c->codec.mime_subtype, (const gchar *) b) == 0)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

static gint
match_payload_predicate (gconstpointer a, gconstpointer b)
{
  codec_t * c = (codec_t *) a;
  if (c->codec.payload == GPOINTER_TO_INT (b))
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

/**
 * Free the internal queues used to hold the audio codecs
 */
static void
codec_library_free_audio_codecs (codec_library_t* library)
{
  // TODO Free the elements properly.
  g_queue_free (library->audio_codec_list);
  g_mutex_free (library->audio_codec_list_mutex);
}

/**
 * Free the internal queues used to hold the video codecs
 */
static void
codec_library_free_video_codecs (codec_library_t* library)
{
  g_queue_free (library->video_codec_list);
  g_mutex_free (library->video_codec_list_mutex);
}

/**
 * Find a Codec pointer in the queue.
 */
static GList*
codec_library_get_audio_codec (codec_library_t* library, codec_t* codec)
{
  return g_queue_find (library->audio_codec_list, codec);
}

static GList*
codec_library_get_video_codec (codec_library_t* library, codec_t* codec)
{
  return g_queue_find (library->video_codec_list, codec);
}

static void
codec_library_add_audio_codec (codec_library_t* library,
    audio_codec_t* audioCodec)
{
  // If there is already a codec with that identifier, the new one has precedence over the old.
  GList* codec;
  if ((codec = codec_library_get_audio_codec (library, audioCodec)) != NULL)
    {
      // TODO unref/free the old data.
      codec->data = audioCodec;
    }
  else
    {
      g_mutex_lock(library->audio_codec_list_mutex);
        {
          g_queue_push_tail (library->audio_codec_list, (gpointer *) codec);
        }
      g_mutex_unlock(library->audio_codec_list_mutex);
    }
}

static void
codec_library_add_video_codec (codec_library_t* library,
    video_codec_t* videoCodec)
{
  GList* codec;
  if ((codec = codec_library_get_video_codec (library, videoCodec)) != NULL)
    {
      // TODO unref/free the old data.
      codec->data = videoCodec;
    }
  else
    {
      g_mutex_lock(library->video_codec_list_mutex);
        {
          g_queue_push_tail (library->video_codec_list, (gpointer *) codec);
        }
      g_mutex_unlock(library->video_codec_list_mutex);
    }
}

static void
copy_codec_in_library (gpointer el, gpointer user_data)
{
  codec_t* codec = (codec_t*) el;
  codec_library_t* library = (codec_library_t*) user_data;

  codec_library_add (library, codec_copy (codec));
}

static void
codec_library_audio_clear (codec_library_t* library)
{
  g_mutex_lock(library->audio_codec_list_mutex);
    {
      g_queue_free (library->audio_codec_list);
    }
  g_mutex_unlock(library->audio_codec_list_mutex);

  library->audio_codec_list = NULL;

  g_mutex_lock(library->audio_codec_list_mutex);
    {
      library->audio_codec_list = g_queue_new ();
    }
  g_mutex_unlock(library->audio_codec_list_mutex);
}

static void
codec_library_add_list (codec_library_t* library, GList* codecs)
{
  if (!codecs)
    {
      WARN("Video codec list is NULL (%s:%d)", __FILE__, __LINE__);
      return;
    }

  GList* it;
  for (it = codecs; it != NULL; it = g_list_next(it))
    {
      codec_t* codec = g_new(codec_t, 1);
      memcpy (codec, it->data, sizeof(codec_t)); // Does not copy the strings themselves.
      codec_library_add (library, codec);
      // TODO g_free((it)->data)
    }
}

static void
codec_library_video_clear (codec_library_t* library)
{
  g_mutex_lock(library->video_codec_list_mutex);
    {
      g_queue_free (library->video_codec_list);
    }
  g_mutex_unlock(library->video_codec_list_mutex);

  library->video_codec_list = NULL;

  g_mutex_lock(library->video_codec_list_mutex);
    {
      library->video_codec_list = g_queue_new ();
    }
  g_mutex_unlock(library->video_codec_list_mutex);
}

static void
codec_library_audio_reset (account_t* account)
{
  codec_library_audio_clear (account->codecs);
  codec_library_add_list (account->codecs, dbus_get_all_audio_codecs ());
}

static void
codec_library_video_reset (account_t* account)
{
  codec_library_video_clear (account->codecs);
  codec_library_add_list (account->codecs, dbus_get_all_video_codecs ());
}

static codec_t*
codec_library_get_audio_codec_by_identifier (codec_library_t* library,
    gconstpointer identifier)
{
  GList* codec;
  g_mutex_lock(library->audio_codec_list_mutex);
    {
      codec = g_queue_find_custom (library->audio_codec_list, identifier,
          match_identifier_predicate);
    }
  g_mutex_unlock(library->audio_codec_list_mutex);

  if (codec)
    {
      return codec->data;
    }

  WARN("Codec with identifier \"%d\" could not be found in the audio library.", identifier);

  return NULL;
}

static codec_t*
codec_library_get_video_codec_by_identifier (codec_library_t* library,
    gconstpointer identifier)
{
  GList* codec;
  g_mutex_lock(library->video_codec_list_mutex);
    {
      codec = g_queue_find_custom (library->video_codec_list, identifier,
          match_identifier_predicate);
    }
  g_mutex_unlock(library->video_codec_list_mutex);

  if (codec)
    {
      return codec->data;
    }

  WARN("Codec with identifier \"%d\" could not be found in the video library.", identifier);

  return NULL;
}

static
void
swap_link_down (GList* codec)
{
  if (!codec)
    {
      return;
    }

  GList* link_down;
  if ((link_down = g_list_next(codec)) != NULL)
    {
      gpointer tmp = codec->data;
      codec->data = link_down->data;
      link_down->data = tmp;
    }
}

static
void
swap_link_up (GList* codec)
{
  if (!codec)
    {
      return;
    }

  GList* link_up;
  if ((link_up = g_list_previous(codec)) != NULL)
    {
      gpointer tmp = codec->data;
      codec->data = link_up->data;
      link_up->data = tmp;
    }
}

codec_library_t*
codec_library_get_system_codecs ()
{
  if (system_library == NULL)
    {
      system_library = codec_library_new ();
      codec_library_load_available_codecs (system_library);
    }

  return system_library;
}

codec_library_t*
codec_library_new ()
{
  codec_library_t* library =
      (codec_library_t*) malloc (sizeof(codec_library_t));

  library->audio_codec_list = g_queue_new ();
  library->audio_codec_list_mutex = g_mutex_new();

  library->video_codec_list = g_queue_new ();
  library->video_codec_list_mutex = g_mutex_new();

  DEBUG("New codec library created.");

  return library;
}

void
codec_library_free (codec_library_t* library)
{
  codec_library_free_audio_codecs (library);
  codec_library_free_video_codecs (library);
  g_free (library);
  library = NULL;
}

void
codec_library_load_available_codecs (codec_library_t* library)
{
  DEBUG("Loading system codecs ...");

  // Load audio codecs
  codec_library_add_list (library, dbus_get_all_audio_codecs ());

  // Load video codecs
  codec_library_add_list (library, dbus_get_all_video_codecs ());
}

void
codec_library_load_audio_codecs_by_account (account_t* account)
{
  DEBUG("Loading audio codecs for account \"%s\" ...", account->accountID);

  // Clear all of the actual codecs, and load built-in list
  codec_library_audio_reset (account);

  // Get the active audio codecs available for the given account.
  codec_library_add_list (account->codecs, dbus_get_active_audio_codecs (
      account->accountID));
}

void
codec_library_load_video_codecs_by_account (account_t* account)
{
  DEBUG("Loading video codecs for account \"%s\" ...", account->accountID);

  // Clear all of the actual codecs, and load built-in list
  codec_library_audio_reset (account);

  // Get the active audio codecs available for the given account.
  codec_library_add_list (account->codecs, dbus_get_active_video_codecs (
      account->accountID));
}

void
codec_library_add (codec_library_t* library, codec_t* codec)
{
  DEBUG("       L Adding codec \"%s\" to codec library.", codec->codec.mime_subtype);
  if (g_strcmp0 (codec->codec.mime_type, "audio") == 0)
    {
      codec_library_add_audio_codec (library, codec);
    }
  else if (g_strcmp0 (codec->codec.mime_type, "video") == 0)
    {
      codec_library_add_video_codec (library, codec);
    }
}

guint
codec_library_get_size (codec_library_t* library)
{
  return g_queue_get_length (library->audio_codec_list) + g_queue_get_length (
      library->video_codec_list);
}

codec_t*
codec_library_get_codec_by_mime_subtype (codec_library_t* library,
    gconstpointer name)
{
  GList* codec;
  g_mutex_lock(library->audio_codec_list_mutex);
    {
      codec = g_queue_find_custom (library->audio_codec_list, name,
          match_mime_subtype_predicate);
    }
  g_mutex_unlock(library->audio_codec_list_mutex);

  if (codec)
    {
      return codec->data;
    }

  WARN("Codec with mime subtype \"%s\" could not be found in the library.", name);

  return NULL;
}

codec_t*
codec_library_get_codec_by_payload_type (codec_library_t* library,
    gconstpointer payload)
{
  GList* codec;
  g_mutex_lock(library->audio_codec_list_mutex);
    {
      codec = g_queue_find_custom (library->audio_codec_list, payload,
          match_payload_predicate);
    }
  g_mutex_unlock(library->audio_codec_list_mutex);

  if (codec)
    {
      return codec->data;
    }

  WARN("Codec with payload type \"%d\" could not be found in the library.", payload);

  return NULL;
}

codec_t*
codec_library_get_codec_by_identifier (codec_library_t* library,
    gconstpointer identifier)
{
  codec_t* codec;
  if ((codec
      = codec_library_get_audio_codec_by_identifier (library, identifier))
      == NULL)
    {
      if ((codec = codec_library_get_video_codec_by_identifier (library,
          identifier)) == NULL)
        {
          return NULL;
        }
    }

  return codec;
}

GQueue*
codec_library_get_audio_codecs (codec_library_t* library)
{
  return library->audio_codec_list;
}

GQueue*
codec_library_get_video_codecs (codec_library_t* library)
{
  return library->video_codec_list;
}

void
codec_library_move_codec_down (codec_library_t* library, codec_t* codec)
{
  if (g_strcmp0 (codec->codec.identifier, "audio"))
    {
      g_mutex_lock(library->audio_codec_list_mutex);
        {
          swap_link_down (g_queue_find (library->audio_codec_list, codec));
        }
      g_mutex_unlock(library->audio_codec_list_mutex);

    }
  else if (g_strcmp0 (codec->codec.identifier, "video"))
    {
      g_mutex_lock(library->video_codec_list_mutex);
        {
          swap_link_down (g_queue_find (library->video_codec_list, codec));
        }
      g_mutex_unlock(library->video_codec_list_mutex);
    }
}

void
codec_library_move_codec_up (codec_library_t* library, codec_t* codec)
{
  if (g_strcmp0 (codec->codec.identifier, "audio"))
    {
      g_mutex_lock(library->audio_codec_list_mutex);
        {
          swap_link_up (g_queue_find (library->audio_codec_list, codec));
        }
      g_mutex_unlock(library->audio_codec_list_mutex);
    }
  else if (g_strcmp0 (codec->codec.identifier, "video"))
    {
      g_mutex_lock(library->video_codec_list_mutex);
        {
          swap_link_up (g_queue_find (library->video_codec_list, codec));
        }
      g_mutex_unlock(library->video_codec_list_mutex);
    }
}

void
codec_library_toggle_active (codec_library_t* library, codec_t* codec)
{
  if (codec->codec.is_active)
    {
      g_mutex_lock(library->audio_codec_list_mutex);
        {
          codec->codec.is_active = FALSE;
        }
      g_mutex_unlock(library->audio_codec_list_mutex);
    }
  else
    {
      g_mutex_lock(library->audio_codec_list_mutex);
        {
          codec->codec.is_active = TRUE;
        }
      g_mutex_unlock(library->audio_codec_list_mutex);
    }
}

void
codec_library_set_active (codec_library_t* library, codec_t* codec,
    gboolean state)
{
  if (codec->codec.is_active == state)
    {
      return;
    }

  codec_library_toggle_active (library, codec);
}

codec_t*
codec_copy (codec_t* src)
{
  codec_t* dst = g_new(codec_t, 1);

  *dst = *src;
  dst->codec.identifier = g_strdup (src->codec.identifier);
  dst->codec.mime_type = g_strdup (src->codec.mime_type);
  dst->codec.mime_subtype = g_strdup (src->codec.mime_subtype);
  dst->codec.description = g_strdup (src->codec.description);

  return dst;
}

codec_library_t*
codec_library_copy (codec_library_t* library)
{
  codec_library_t* lib = codec_library_new ();

  g_queue_foreach (library->audio_codec_list, copy_codec_in_library, lib);

  lib->number_active = library->number_active;

  return lib;
}

void
codec_library_set (codec_library_t* library, const gchar* accountID)
{
  // Work on a private copy to avoid race condition problems
  codec_library_t* library_copy = codec_library_copy (library);

  // Scan the codec library for finding only those codecs that are active
  GQueue* active_queue = g_queue_new ();
  int i;
  for (i = 0; i < codec_library_get_size (library_copy); i++)
    {
      codec_t* codec = g_queue_peek_nth (library_copy->audio_codec_list, i);
      if (codec && codec->codec.is_active)
        {
          g_queue_push_tail (active_queue, codec);
        }
    }

  // Build a string array for sending over dbus
  gchar** identifiers = g_new(gchar*, g_queue_get_length(active_queue) + 1);

  DEBUG("Active codecs %d", g_queue_get_length(active_queue));

  int j = 0;
  for (i = 0; i < g_queue_get_length (active_queue); i++)
    {
      codec_t* codec = g_queue_peek_nth (active_queue, i);

      if (codec)
        {
          DEBUG("Sending preferred codec %s (%s) for account id \"%s\"", codec->codec.identifier, codec->codec.mime_subtype, accountID);
          identifiers[j] = codec->codec.identifier;
          j += 1;
        }
    }
  identifiers[j] = NULL;

  dbus_set_active_audio_codecs (identifiers, accountID);

  // TODO g_free(identifiers)
}
