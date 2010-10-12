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

#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include "codec.h"

/** @enum account_state_t
  * This enum have all the states an account can take.
  */
typedef enum {
    /** Invalid state */
    ACCOUNT_STATE_INVALID = 0,
    /** The account is registered  */
    ACCOUNT_STATE_REGISTERED,
    /** The account is not registered */
    ACCOUNT_STATE_UNREGISTERED,
    /** The account is trying to register */
    ACCOUNT_STATE_TRYING,
    /** Error state. The account is not registered */
    ACCOUNT_STATE_ERROR,
    /** An authentification error occured. Wrong password or wrong username. The account is not registered */
    ACCOUNT_STATE_ERROR_AUTH,
    /** The network is unreachable. The account is not registered */
    ACCOUNT_STATE_ERROR_NETWORK,
    /** Host is unreachable. The account is not registered */
    ACCOUNT_STATE_ERROR_HOST,
    /** Stun server configuration error. The account is not registered */
    ACCOUNT_STATE_ERROR_CONF_STUN,
    /** Stun server is not existing. The account is not registered */
    ACCOUNT_STATE_ERROR_EXIST_STUN,
    /** IP profile status **/
    IP2IP_PROFILE_STATUS
} account_state_t;

/**
 * Utility structure for holding a video frame resolution.
 */
typedef struct {
    guint width;
    guint height;
} resolution_t;

/**
 * Utility structure for holding a video frame rate.
 */
typedef struct {
    guint numerator;
    guint denominator;
} framerate_t;

/**
 * @return a new string representing this framerate.
 */
gchar* framerate_to_string (framerate_t* rate);

/**
 * @param rate The string representation of the frame rate ("num"/"denom")
 * @return a framerate structure for the given string. The structure must be freed by the user.
 */
framerate_t* framerate_new_from_string (const gchar* rate);

/**
 * Utility structure for holding the video settings per account.
 */
typedef struct {
    resolution_t resolution;

    framerate_t framerate;

    gchar* device;

    gboolean always_offer_video;

    GMutex* mutex;
} video_settings_t;

/**
 * @return a new instance of a video settings object.
 */
video_settings_t* video_settings_new();

/**
 * @param settings A video settings object to free.
 */
void video_settings_free (video_settings_t* settings);

/**
 * @param settings A video settings object to print.
 */
void video_settings_print (video_settings_t* settings);

/**
 * @param accountID The account id on which to save those settings for.
 */
void video_settings_saves (video_settings_t* settings, const gchar* accountID);

/**
 * @param device The device to set in the structure.
 */
void video_settings_set_device (video_settings_t* settings, gchar* device);

/**
 * @return The video device.
 */
gchar* video_settings_get_device (video_settings_t* settings);

/**
 * @param resolution The device to set in the structure.
 */
void video_settings_set_resolution (video_settings_t* settings, resolution_t* resolution);

/**
 * @param width The desired width.
 */
void video_settings_set_width (video_settings_t* settings, guint width);

/**
 * @return The preferred width.
 */
guint video_settings_get_width (video_settings_t* settings);

/**
 * @param height The desired height.
 */
void video_settings_set_height (video_settings_t* settings, guint height);

/**
 * @return The preferred height.
 */
guint video_settings_get_height (video_settings_t* settings);

/**
 * @param framerate The device to set in the structure.
 */
void video_settings_set_framerate (video_settings_t* settings, framerate_t* framerate);

/**
 * @param numerator The desired numerator, in the framerate structure.
 */
void video_settings_set_numerator (video_settings_t* settings, guint numerator);

/**
 * @return The preferred numerator.
 */
guint video_settings_get_numerator (video_settings_t* settings);

/**
 * @param denominator The desired denominator, in the framerate structure.
 */
void video_settings_set_denominator (video_settings_t* settings, guint denominator);

/**
 * @return The preferred denominator.
 */
guint video_settings_get_denominator (video_settings_t* settings);

/**
 * @param offer TRUE if video should be offered on every call.
 */
void video_settings_set_always_offer_video (video_settings_t* settings, gboolean offer);

/**
 * @return TRUE the "always offer video" property is enabled.
 */
gboolean video_settings_get_always_offer_video (video_settings_t* settings);

/** @struct account_t
  * @brief Account information.
  * This struct holds information about an account.  All values are stored in the
  * properties GHashTable except the accountID and state.  This match how the
  * server internally works and the dbus API to save and retrieve the accounts details.
  *
  * To retrieve the Alias for example, use g_hash_table_lookup(a->properties, ACCOUNT_ALIAS).
  */
typedef struct  {
    gchar * accountID;
    account_state_t state;
    gchar * protocol_state_description;
    guint protocol_state_code;
    GHashTable * properties;
    GPtrArray * credential_information;

    /* The codec list */
    codec_library_t* codecs;
    guint _messages_number;

    video_settings_t* video_settings;
} account_t;

/**
 * @param accountID The accountID.
 * @return a new and initialized account object.
 */
account_t* account_new (gchar* accountID);

/**
 * @param account An existing instance of an account object to initialize.
 */
gboolean account_init (account_t* account);

/**
 * @param account The account to free
 * @postcondition All of the elements within the structure as well as the structure itself will be freed.
 */
void account_free (account_t* account);


#endif
