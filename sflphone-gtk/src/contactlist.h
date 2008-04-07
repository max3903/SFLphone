/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
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

#ifndef __CONTACTLIST_H__
#define __CONTACTLIST_H__

#include "../../src/contact/presencestatus.h"

#include <gtk/gtk.h>

// TODO Complete and change list of icons
// Location of pixmaps icons to represent different presence status
// Active statuses icons
#define PRESENCE_UNSUBSCRIBED_ICON		ICONS_DIR "/unhold.svg"
#define PRESENCE_UNKNOWN_ICON			ICONS_DIR "/fail.svg"
#define PRESENCE_RINGING_ICON			ICONS_DIR "/ring.svg"
#define PRESENCE_ON_THE_PHONE_ICON		ICONS_DIR "/current.svg"
#define PRESENCE_ON_HOLD_ICON			ICONS_DIR "/hold.svg"

// Passive statuses icons
#define PRESENCE_ONLINE_ICON			ICONS_DIR "/dial.svg"
#define PRESENCE_BUSY_ICON				ICONS_DIR "/dial.svg"
#define PRESENCE_BE_RIGHT_BACK_ICON		ICONS_DIR "/dial.svg"
#define PRESENCE_AWAY_ICON				ICONS_DIR "/dial.svg"
#define PRESENCE_OUT_TO_LUNCH_ICON		ICONS_DIR "/dial.svg"
#define PRESENCE_OFFLINE_ICON			ICONS_DIR "/hang_up.svg"
#define PRESENCE_DO_NOT_DISTURB_ICON	ICONS_DIR "/dial.svg"

/** 
 * @file	contactlist.h
 * @brief	A hash table that holds contact lists mapped by their account ID
 * 			It holds contacts, their entries and their presence information if needed
 */

typedef struct {
	gchar*		_entryID;				// Contact number, can be a phone number, an extension... (202, 5141231234, sip:141@...)
	gchar*		_text;					// Simplified textual representation (Poste 202, 514-123-1234, Poste 141)
	gchar*		_type;					// Work, home, cell...
	gboolean	_isShownInConsole;		// Is shown in the call console
	gboolean	_isSubscribed;			// Is contact subscribed to presence
	gchar*		_presenceStatus;		// Presence information obtained if entry is subscribed
	gchar*		_presenceInfo;			// Additional info on presence
} contact_entry_t;

typedef struct {
	gchar*		_contactID;				// Unique id for contact by account
	gchar*		_firstName;
	gchar*		_lastName;
	gchar*		_email;
	GQueue*		_entryList;				// List of entries for each contact
} contact_t;

/**
 * Functions to initialize and clear the hash table
 */
void contact_hash_table_init();
void contact_hash_table_clear();

/*
 * Functions to add, get and clear contact lists contained in the hash map
 */
// TODO
void contact_hash_table_add_contact_list(gchar* accountID);
void contact_hash_table_remove_contact_list(gchar* accountID);
GQueue* contact_hash_table_get_contact_list(const gchar* accountID);

/**
 * Functions to add, edit and remove a contact in a contact list
 */
// TODO
void contact_list_add(gchar* accountID, contact_t* contact, gboolean update);
void contact_list_edit(gchar* accountID, contact_t* contact);
void contact_list_remove(gchar* accountID, gchar* contactID);

/**
 * Functions to get a particular contact in the list and to get the list size
 */
guint contact_list_get_size(GQueue* contactList);
contact_t* contact_list_get(GQueue* contactList, const gchar* contactID);
contact_t* contact_list_get_nth(GQueue* contactList, guint index);

/**
 * Functions to add, get, edit and remove a contact entry in a contact
 */
// TODO
void contact_list_entry_add(gchar* accountID, gchar* contactID, contact_entry_t* entry, gboolean update);
//void contact_list_entry_edit(contact_t* contact, contact_entry_t* oldEntry, contact_entry_t* newEntry);
//void contact_list_entry_remove(contact_t* contact, contact_entry_t* entry);

/**
 * Function when an update on presence status is received from server and only entry is known, not the contact
 */
void contact_list_entry_change_presence_status(const gchar* accountID, const gchar* entryID, const gchar* status, const gchar* additionalInfo);

/**
 * Functions to get a particular contact entry in the list and to get the list size
 */
guint contact_list_entry_get_size(contact_t* contact);
contact_entry_t* contact_list_entry_get(contact_t* contact, const gchar* entryID);
contact_entry_t* contact_list_entry_get_nth(contact_t* contact, guint index);

/**
 * Create contact and contact entry struct from ID and string array of details
 */
contact_t* contact_list_new_contact_from_details(gchar* contactID, gchar** details);
contact_entry_t* contact_list_new_contact_entry_from_details(gchar* contactEntryID, gchar** details);

/**
 * Compare function to find contact in list by contact ID
 */
gint compare_contact_contactID(gconstpointer a, gconstpointer b);
gint compare_contact_contactEntryID(gconstpointer a, gconstpointer b);

/** 
 * Presence translation and icons from defined type obtained by DBus
 */
const gchar* contact_list_presence_status_translate(const gchar* presenceStatus);
const gchar* contact_list_presence_status_get_icon_string(const gchar* presenceStatus);

#endif
