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

#ifndef __CODEC_LIBRARY_H__
#define __CODEC_LIBRARY_H__

#include <gtk/gtk.h>

#include "codec.h"
#include "account.h"

/**
 * Create a codec library type.
 * @return a pointer to the new codec library object.
 */
codec_library_t*
codec_library_new ();

/**
 * Free a codec library object.
 */
void
codec_library_free (codec_library_t* library);

/**
 * Take two codec library and merge them into a single one.
 * @param l1 Some codec library.
 * @param l2 Some other codec library.
 * @return A new codec library.
 * @postcondition All of the codecs in l1 but NOT in l2 are now in the new library. All of the codecs of l1 having the
 * same identifier as some codec of l2 are also in the new library, but with the state from l2.
 */
codec_library_t*
codec_library_merge (const codec_library_t* l1, const codec_library_t* l2);

/**
 * @return a codec library that contains all of the supported codecs in the deamon.
 */
codec_library_t*
codec_library_get_system_codecs ();

/** 
 * Retrieve the codec list from the server via DBUS.
 * @param library The codec library object to load into.
 */
void
codec_library_load_available_codecs (codec_library_t* library);

/** 
 * Retrieve the codec list from the server via DBUS.
 * @param account The account in which to load the enabled codecs.
 */
void
codec_library_load_codecs_by_account (account_t* account);

/**
 * This function clears the internal list of codec that it keeps.
 * @param library The codec library object to clear.
 */
void
codec_library_clear (codec_library_t* library);

/** 
 * Add a codec to the codec library.
 * @param library The codec library object in which to add the codec.
 * @param codec The codec to add.
 */
void
codec_library_add (codec_library_t* library, codec_t* codec);

/**
 * @return The number of codecs in the list
 */
guint
codec_library_get_size (codec_library_t* library);

/**
 * @param library The library structure.
 * @return The number of active codecs in the library;
 */
guint
codec_library_get_number_active (codec_library_t* library);

/**
 * @param library The library structure.
 * @return The number of inactive codecs in the library;
 */
guint
codec_library_get_number_inactive (codec_library_t* library);

/** 
 * @param library The codec library object in which to search for.
 * @param name The codec's mime subtype to search for.
 */
codec_t*
codec_library_get_codec_by_mime_subtype (codec_library_t* library,
    gconstpointer name);

/**
 * @param library The codec library object in which to search for.
 * @param name The codec's payload type to search for.
 */
codec_t*
codec_library_get_codec_by_payload_type (codec_library_t* library,
    gconstpointer payload);

/**
 * @param library The codec library object in which to search for.
 * @param identifier The identifier for this codec.
 */
codec_t*
codec_library_get_codec_by_identifier (codec_library_t* library,
    gconstpointer identifier);

/**
 * @param library The codec library.
 * @return a GQueue object containing all of the codecs.
 */
GQueue*
codec_library_get_all_codecs (codec_library_t* library);

/** 
 * @param index The position of the codec in the list.
 * @return the codec at the nth position in the list
 */
codec_t*
codec_library_get_nth_codec (codec_library_t* library, guint n);

/**
 * Sync the codec library in this client with the server.
 * @param library The codec library that should be set on the server for the given account.
 * @param accountID The account identifiers for which codecs should be synced.
 * @postcondition The codec list for the account on the server will be the same as in this client.
 */
void
codec_library_set (codec_library_t* library, const gchar* accountID);

/**
 * Set the prefered codec first in the codec list
 * @param library The library in which to move the codec up.
 * @param index The position in the list of the prefered codec
 */
void
codec_library_set_prefered_order (codec_library_t* library, guint index);

/** 
 * Move the codec from an unit up in the codec_list
 * @param library The library in which to move the codec up.
 * @param index The current index in the list
 */
void
codec_library_move_codec_up (codec_library_t* library, guint index);

/** 
 * Move the codec from an unit down in the codec_list
 * @param library The library in which to move the codec down.
 * @param index The current index in the list
 */
void
codec_list_move_codec_down (codec_library_t* library, guint index);

/**
 * Toggle the activation state of a codec in some library.
 * @param library The library in which this codec is set to the new state.
 * @param codec The codec that get a state change.
 */
void
codec_library_toggle_active (codec_library_t* library, codec_t* codec);

/**
 * Set a new activation state for a codec in some library.
 * @param library The library in which this codec is set to the new state.
 * @param codec The codec that get a state change.
 * @param state The new state in which the codec should be in.
 */
void
codec_library_set_active (codec_library_t* library, codec_t* codec,
    gboolean state);

/**
 * Set a codec active. An active codec will be used for codec negotiation
 * @param name The string description of the codec
 */
void
codec_set_inactive (codec_t **c);

/**
 * Set a codec inactive. An active codec won't be used for codec negotiation
 * @param name The string description of the codec
 */
void
codec_set_active (codec_t **c);

#endif

