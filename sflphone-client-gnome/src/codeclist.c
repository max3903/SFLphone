/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
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

codec_library_t* codec_library_get_system_codecs() {
  if (system_library == NULL) {
    codec_library_load_available_codecs(system_library);
  }

  return system_library;
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

codec_library_t*
codec_library_new ()
{
  codec_library_t* library = (codec_library_t*) malloc(sizeof(codec_library_t));

  library->codec_list = g_queue_new();

  DEBUG("New codec library created");

  return library;
}

void
codec_library_free (codec_library_t* library)
{
  // TODO Free the elements properly.
  g_queue_free (library->codec_list);
  g_free (library);
  library = NULL;
}

void
codec_library_clear (codec_library_t* library)
{
  g_queue_free (library->codec_list);
  library->codec_list = NULL;
}

void
codec_library_load_available_codecs (codec_library_t* library)
{
  GList* codecs = dbus_get_all_audio_codecs ();
  GList* it;
  for (it = codecs; it != NULL; it = g_list_next(it))
    {
      codec_t* codec = g_new(codec_t, 1);

      memcpy (codec, it->data, sizeof(audio_codec_t)); // Does not copy the strings themselves.

      codec_library_add (library, codec);

      // TODO g_free((it)->data)
    }
}

void
codec_library_load_codecs_by_account (account_t* account)
{
  GList* codecs = dbus_get_active_audio_codecs (account->accountID);
  GList* it;
  for (it = codecs; it != NULL; it = g_list_next(it))
    {
      codec_t* codec = g_new(codec_t, 1);

      memcpy (codec, it->data, sizeof(codec_t)); // Does not copy the strings themselves.

      DEBUG("Adding codec \"%s\"", codec->codec.mime_subtype);
      codec_library_add (account->codecs, codec);

      // TODO g_free((it)->data)
    }
}

void
codec_library_add (codec_library_t* library, codec_t* codec)
{
  DEBUG("Adding codec %s", codec->codec.mime_subtype);

  g_queue_push_tail (library->codec_list, (gpointer *) codec);
}

guint
codec_library_get_size (codec_library_t* library)
{
  return g_queue_get_length (library->codec_list);
}

codec_t*
codec_library_get_codec_by_name (codec_library_t* library, gconstpointer name)
{
  GList* codec = g_queue_find_custom (library->codec_list, name,
      match_mime_subtype_predicate);
  return codec->data;
}

codec_t*
codec_library_get_codec_by_payload_type (codec_library_t* library,
    gconstpointer payload)
{
  GList* codec = g_queue_find_custom (library->codec_list, payload,
      match_payload_predicate);
  return codec->data;
}

GQueue*
codec_library_get_all_codecs (codec_library_t* library)
{
  return library->codec_list;
}

codec_t*
codec_library_get_nth_codec (codec_library_t* library, guint n)
{
  return g_queue_peek_nth (library->codec_list, n);
}

void
codec_library_set_prefered_order (codec_library_t* library, guint index)
{
  codec_t * prefered = codec_library_get_nth_codec (library, index);
  g_queue_pop_nth (library->codec_list, index);
  g_queue_push_head (library->codec_list, prefered);
}

void
codec_list_move_codec_down (codec_library_t* library, guint index)
{
  DEBUG("Codec list Size: %i \n", codec_library_get_size(library));

  if (codec_library_get_size (library) != index)
    {
      gpointer codec = g_queue_pop_nth (library->codec_list, index);
      g_queue_push_nth (library->codec_list, codec, index + 1);
    }
}

void
codec_library_move_codec_up (codec_library_t* library, guint index)
{
  DEBUG("Codec list Size: %i \n", codec_library_get_size(library));

  if (codec_library_get_size (library) != index)
    {
      gpointer codec = g_queue_pop_nth (library->codec_list, index);
      g_queue_push_nth (library->codec_list, codec, index - 1);
    }
}

void
codec_set_active (codec_t **c)
{
  if (c)
    {
      DEBUG("%s set active", (*c)->codec.mime_subtype);
      (*c)->codec.is_active = TRUE;
    }
}

void
codec_set_inactive (codec_t **c)
{
  if (c)
    {
      DEBUG("%s set active", (*c)->codec.mime_subtype);
      (*c)->codec.is_active = FALSE;
    }
}

void codec_library_set (codec_library_t* library, const gchar* accountID)
{
  // Build an array of identifiers
  gchar** identifiers = g_new(gchar*, codec_library_get_size(library));

  int i;
  for (i = 0; i < codec_library_get_size(library); i++) {
    codec_t* codec = g_queue_peek_nth(library->codec_list, i);
    identifiers[i] = codec->codec.identifier;
  }

  dbus_set_active_audio_codecs(identifiers, accountID);

  // TODO g_free(identifiers)
}
