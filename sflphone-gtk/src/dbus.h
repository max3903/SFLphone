/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc@squidy.info>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Guillaume Carmel-Archambault <guillaume.carmel-archambault@savoirfairelinux.com>
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


#ifndef __DBUS_H__
#define __DBUS_H__

#include <accountlist.h>
#include <configwindow.h>

/** @file dbus.h
  * @brief General DBus functions wrappers.
  */
/** @return TRUE if connection succeeded, FALSE otherwise */
gboolean dbus_connect ();
void dbus_clean ();

/* CallManager */
void dbus_hold (const call_t * c );
void dbus_unhold (const call_t * c );
void dbus_hang_up (const call_t * c );
void dbus_transfert (const call_t * c);
void dbus_accept (const call_t * c);
void dbus_refuse (const call_t * c);
void dbus_place_call (const call_t * c);
/* Shared Memory */
gchar* dbus_get_local_shared_memory_key();
gchar* dbus_get_remote_shared_memory_key();
//Invite 3rd person in a conference
gboolean dbus_invite_conference( const call_t * c );
gboolean dbus_join_conference( const call_t * onHoldCall,  const call_t * newCall);
//Remote video status change - enable or disable remote video display
gboolean dbus_change_video_avaibility();
//Webcam Status change - enable or disable video capture
void dbus_change_webcam_status(gboolean status, const call_t * c);


/* ConfigurationManager */
/** Returns a NULL terminated array of gchar pointers */
gchar ** dbus_account_list();
GHashTable * dbus_account_details(gchar * accountID);
void dbus_set_account_details(account_t *a);
void dbus_add_account(account_t *a);
void dbus_remove_account(gchar * accountID);
void dbus_set_volume(const gchar * device, gdouble value);
gdouble dbus_get_volume(const gchar * device);
void dbus_play_dtmf(const gchar * key);
gchar* dbus_get_default_account();
void dbus_set_default_account(gchar * accountID);
gchar** dbus_codec_list();
gchar** dbus_codec_details(int payload);
gchar** dbus_default_codec_list();
gchar** dbus_get_active_codec_list( void );
void dbus_set_active_codec_list( const gchar** list );
//Video codec list in Video Settings
gchar** dbus_video_codec_list();
gchar** dbus_get_active_video_codec_list( void );
void dbus_set_active_video_codec_list( const gchar** list );
//Brightness of the video capture
slider_t dbus_get_brightness();
void dbus_set_brightness(int value);
//Contrast of the video capture
slider_t dbus_get_contrast();
void dbus_set_contrast(int value);
//Colour of the video capture
slider_t dbus_get_colour();
void dbus_set_colour(int value);
//Webcam list
gchar** dbus_get_webcam_device_list();
void dbus_set_webcam_device(gchar* name);
//Resolution list
gchar** dbus_get_resolution_list();
void dbus_set_resolution(gchar* name);
gchar* dbus_get_current_resolution();
//Bitrate list
gchar** dbus_get_bitrate_list();
void dbus_set_bitrate(gchar* name);
gchar* dbus_get_current_bitrate();

//Enables/Disables local video capture in the preferences webcam settings tab
gboolean dbus_enable_local_video_pref();
gboolean dbus_disable_local_video_pref();

// Audio devices related methods
gchar** dbus_get_input_audio_plugin_list();
gchar** dbus_get_output_audio_plugin_list();
void dbus_set_input_audio_plugin(gchar* audioPlugin);
void dbus_set_output_audio_plugin(gchar* audioPlugin);
gchar** dbus_get_audio_output_device_list();
void dbus_set_audio_output_device(const int index);
gchar** dbus_get_audio_input_device_list();
void dbus_set_audio_input_device(const int index);
// Output and input current devices
gchar** dbus_get_current_audio_devices_index();
int dbus_get_audio_device_index(const gchar* name);
gchar* dbus_get_current_audio_output_plugin();
int dbus_is_iax2_enabled( void );
int dbus_is_ringtone_enabled( void );
void dbus_ringtone_enabled( void );
gchar* dbus_get_ringtone_choice( void );
void dbus_set_ringtone_choice( const gchar* tone );



/** ContactManager */
gchar** dbus_get_contacts(gchar* accountID);
gchar** dbus_get_contact_details(gchar* accountID, gchar* contactID);
gchar** dbus_get_contact_entries(gchar* accountID, gchar* contactID);
gchar** dbus_get_contact_entry_details(gchar* accountID, gchar* contactID, gchar* entryID);

/* Instance */
void dbus_register( int pid, gchar * name);
void dbus_unregister(int pid);
int dbus_get_registration_count( void );



#endif
