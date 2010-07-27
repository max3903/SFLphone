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

codec_library_t*
codec_library_new ()
{
  codec_library_t* library =
      (codec_library_t*) malloc (sizeof(codec_library_t));

  library->codec_list = g_queue_new ();

  library->codec_list_mutex = g_mutex_new();

  library->number_active = 0;

  DEBUG("New codec library created.");

  return library;
}

void
codec_library_free (codec_library_t* library)
{
  // TODO Free the elements properly.
  g_queue_free (library->codec_list);

  g_mutex_free(library->codec_list_mutex);

  g_free (library);
  library = NULL;
}

void
codec_library_clear (codec_library_t* library)
{
  g_mutex_lock(library->codec_list_mutex);
    {
      g_queue_free (library->codec_list);
    }
  g_mutex_unlock(library->codec_list_mutex);

  library->codec_list = NULL;
  library->number_active = 0;

  g_mutex_lock(library->codec_list_mutex);
    {
      library->codec_list = g_queue_new ();
    }
  g_mutex_unlock(library->codec_list_mutex);
}

void
codec_library_load_available_codecs (codec_library_t* library)
{
  DEBUG("Loading system codecs ...");

  GList* codecs = dbus_get_all_audio_codecs ();
  GList* it;

  for (it = codecs; it != NULL; it = g_list_next(it))
    {
      codec_t* codec = g_new(codec_t, 1);

      memcpy (codec, it->data, sizeof(codec_t)); // Does not copy the strings themselves.

      codec_library_add (library, codec);

      // TODO g_free((it)->data)
    }
}

void
codec_library_load_codecs_by_account (account_t* account)
{
  DEBUG("Loading codecs for account \"%s\" ...", account->accountID);
  DEBUG("Library of size %d", codec_library_get_size(account->codecs));

  // Clear all of the actual codecs.
  codec_library_clear (account->codecs);

  // Get the active audio codecs available for the given account.
  GList* codecs = dbus_get_active_audio_codecs (account->accountID);
  GList* it;

  for (it = codecs; it != NULL; it = g_list_next(it))
    {
      codec_t* codec = g_new(codec_t, 1);

      memcpy (codec, it->data, sizeof(codec_t)); // Does not copy the strings themselves.

      codec_library_add (account->codecs, codec);

      // TODO g_free((it)->data)
    }

  // Add the system codecs that should also be made available.
  codec_library_t* library = codec_library_merge (
      codec_library_get_system_codecs (), account->codecs);

  // Replace the codec library in the account with the new one.
  codec_library_free (account->codecs);

  account->codecs = library;
}

codec_library_t*
codec_library_merge (const codec_library_t* l1, const codec_library_t* l2)
{
  DEBUG("Merging librairies ...");
  codec_library_t* output = codec_library_new ();

  int i;
  for (i = 0; i < codec_library_get_size (l1); i++)
    {
      codec_t* codec_l1 = g_queue_peek_nth (l1->codec_list, i);

      codec_t* codec_l2;
      if ((codec_l2 = codec_library_get_codec_by_identifier (l2,
          codec_l1->codec.identifier)))
        {
          codec_library_add (output, codec_l2);
        }
      else
        {
          codec_library_add (output, codec_l1);
        }
    }

  return output;
}

void
codec_library_add (codec_library_t* library, codec_t* codec)
{
  DEBUG("       L Adding codec \"%s\" to codec library.", codec->codec.mime_subtype);

  g_mutex_lock(library->codec_list_mutex);
    {
      g_queue_push_tail (library->codec_list, (gpointer *) codec);

      if (codec->codec.is_active)
        {
          library->number_active += 1;
        }
      else
        {
          library->number_active -= 1;
        }
    }
  g_mutex_unlock(library->codec_list_mutex);
}

guint
codec_library_get_size (codec_library_t* library)
{
  return g_queue_get_length (library->codec_list);
}

guint
codec_library_get_number_active (codec_library_t* library)
{
  return library->number_active;
}

guint
codec_library_get_number_inactive (codec_library_t* library)
{
  return codec_library_get_size (library) - library->number_active;
}

codec_t*
codec_library_get_codec_by_mime_subtype (codec_library_t* library,
    gconstpointer name)
{
  GList* codec;
  g_mutex_lock(library->codec_list_mutex);
    {
      codec = g_queue_find_custom (library->codec_list, name,
          match_mime_subtype_predicate);
    }
  g_mutex_unlock(library->codec_list_mutex);

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
  g_mutex_lock(library->codec_list_mutex);
    {
      codec = g_queue_find_custom (library->codec_list, payload,
          match_payload_predicate);
    }
  g_mutex_unlock(library->codec_list_mutex);

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
  GList* codec;
  g_mutex_lock(library->codec_list_mutex);
    {
      codec = g_queue_find_custom (library->codec_list, identifier,
          match_identifier_predicate);
    }
  g_mutex_unlock(library->codec_list_mutex);

  if (codec)
    {
      return codec->data;
    }

  WARN("Codec with identifier \"%d\" could not be found in the library.", identifier);

  return NULL;
}

GQueue*
codec_library_get_all_codecs (codec_library_t* library)
{
  return library->codec_list;
}

codec_t*
codec_library_get_nth_codec (codec_library_t* library, guint n)
{
  codec_t* codec;
  g_mutex_lock(library->codec_list_mutex);
    {
      codec = g_queue_peek_nth (library->codec_list, n);
    }
  g_mutex_unlock(library->codec_list_mutex);

  return codec;
}

void
codec_library_set_prefered_order (codec_library_t* library, guint index)
{
  codec_t * prefered = codec_library_get_nth_codec (library, index);
  g_mutex_lock(library->codec_list_mutex);
    {
      g_queue_pop_nth (library->codec_list, index);
      g_queue_push_head (library->codec_list, prefered);
    }
  g_mutex_unlock(library->codec_list_mutex);
}

void
codec_list_move_codec_down (codec_library_t* library, guint index)
{
  DEBUG("Codec list size: %i", codec_library_get_size(library));

  if (codec_library_get_size (library) != index)
    {
      gpointer codec;
      g_mutex_lock(library->codec_list_mutex);
        {
          codec = g_queue_pop_nth (library->codec_list, index);
          g_queue_push_nth (library->codec_list, codec, index + 1);
        }
      g_mutex_unlock(library->codec_list_mutex);
    }
}

void
codec_library_move_codec_up (codec_library_t* library, guint index)
{
  DEBUG("Codec list size: %i", codec_library_get_size(library));

  if (codec_library_get_size (library) != index)
    {
      gpointer codec;
      g_mutex_lock(library->codec_list_mutex);
        {
          codec = g_queue_pop_nth (library->codec_list, index);
          g_queue_push_nth (library->codec_list, codec, index - 1);
        }
      g_mutex_unlock(library->codec_list_mutex);
    }
}

void
codec_library_toggle_active (codec_library_t* library, codec_t* codec)
{
  if (codec->codec.is_active)
    {
      g_mutex_lock(library->codec_list_mutex);
        {
          codec->codec.is_active = FALSE;
          library->number_active -= 1;
        }
      g_mutex_unlock(library->codec_list_mutex);
    }
  else
    {
      g_mutex_lock(library->codec_list_mutex);
        {
          codec->codec.is_active = TRUE;
          library->number_active += 1;
        }
      g_mutex_unlock(library->codec_list_mutex);
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

void
codec_set_active (codec_t** c)
{
  if (c)
    {
      DEBUG("%s set active", (*c)->codec.mime_subtype);
      (*c)->codec.is_active = TRUE;
    }
}

void
codec_set_inactive (codec_t** c)
{
  if (c)
    {
      DEBUG("%s set active", (*c)->codec.mime_subtype);
      (*c)->codec.is_active = FALSE;
    }
}

codec_t* codec_copy(codec_t* src)
{
  codec_t* dst = g_new(codec_t, 1);

  *dst = *src;
  dst->codec.identifier = g_strdup(src->codec.identifier);
  dst->codec.mime_type = g_strdup(src->codec.mime_type);
  dst->codec.mime_subtype = g_strdup(src->codec.mime_subtype);
  dst->codec.description = g_strdup(src->codec.description);

  return dst;
}

static void copy_codec(gpointer el, gpointer user_data)
{
  codec_t* codec = (codec_t*) el;
  codec_library_t* library = (codec_library_t*) user_data;

  codec_library_add(library, codec_copy(codec));
}

codec_library_t*
codec_library_copy (codec_library_t* library)
{
  codec_library_t* lib = codec_library_new ();

  g_queue_foreach (library->codec_list, copy_codec, lib);

  lib->number_active = library->number_active;

  return lib;
}

void
codec_library_set (codec_library_t* library, const gchar* accountID)
{
  // Work on a private copy to avoid race condition problems
  codec_library_t* library_copy = codec_library_copy (library);

  // Scan the codec library for finding only those codecs that are active
  GQueue* active_queue = g_queue_new();
  int i;
  for (i = 0; i < codec_library_get_size (library_copy); i++)
    {
      codec_t* codec = g_queue_peek_nth (library_copy->codec_list, i);
      if (codec && codec->codec.is_active)
        {
          g_queue_push_tail(active_queue, codec);
        }
    }

  // Build a string array for sending over dbus
  gchar** identifiers = g_new(gchar*, g_queue_get_length(active_queue) + 1);

  DEBUG("Active codecs %d", g_queue_get_length(active_queue));

  int j = 0;
  for (i = 0; i < g_queue_get_length(active_queue); i++)
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
