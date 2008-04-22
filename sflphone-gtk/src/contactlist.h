/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
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

// TODO Complete and change list of icons (create new icons in pixmaps/presence)
// Location of pixmaps icons to represent different presence status

// Special statuses icons
#define PRESENCE_NOT_SUBSCRIBED_ICON	""
#define PRESENCE_NOT_INITIALIZED_ICON	ICONS_DIR "/transfert.svg"
#define PRESENCE_NOT_SUPPORTED_ICON		ICONS_DIR "/fail.svg"

// Active statuses icons
#define PRESENCE_UNKNOWN_ICON			ICONS_DIR "/unhold.svg"
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
#define PRESENCE_DO_NOT_DISTURB_ICON	ICONS_DIR "/refuse.svg"

/* 
 * Entry types used for saving entry type in contact list
 * Do not translate directly as it is used in all langages
 */
#define ENTRY_TYPE_WORK		"work"
#define ENTRY_TYPE_HOME		"home"
#define ENTRY_TYPE_MOBILE	"mobile"
#define ENTRY_TYPE_PAGET	"paget"
#define ENTRY_TYPE_OTHER	"other"

/** 
 * @file contactlist.h
 * @brief A hash table that holds contact lists mapped by their account ID
 * It contain contacts, their entries and their presence information if needed
 * It is the underlying model of the contact window and the call console
 * Each of them calls the contact list when needing modifications on the model
 * so that synchronisation in V can occur
 */

/**
 * Structure for a contact entry, usually a number with properties and presence information
 */
typedef struct {
	gchar*		_entryID;				/// Contact number, can be a phone number, an extension... (202, 5141231234, sip:141@...)
	gchar*		_text;					/// Simplified textual representation (Poste 202, 514-123-1234, Poste 141)
	gchar*		_type;					/// Work, home, cell...
	gboolean	_isShownInConsole;		/// Is shown in the call console
	gboolean	_isSubscribed;			/// Is contact subscribed to presence
	gchar*		_presenceStatus;		/// Presence information obtained if entry is subscribed
	gchar*		_presenceInfo;			/// Additional info on presence
} contact_entry_t;

typedef struct {
	gchar*		_contactID;				/// Unique id for contact by account
	gchar*		_firstName;				/// First name of the contact
	gchar*		_lastName;				/// Last name of the contact, optional
	gchar*		_email;					/// Email of the contact for practical purposes
	GQueue*		_entryList;				/// List of entries for each contact
} contact_t;

/**
 * Initialize an empty hash table by defining the required
 * functions to allocate and free keys and values
 */
void contact_hash_table_init();

/**
 * Destroy the hash table and set it to null
 */
void contact_hash_table_clear();

/**
 * Add a new contact list in the hash table corresponding to account
 */
void contact_hash_table_add_contact_list(gchar* accountID);

/**
 * Get the contact queue of the corresponding account
 */
GQueue* contact_hash_table_get_contact_list(const gchar* accountID);

/**
 * Function called by the signal from the daemon when an account changes
 * It should repopulate the call console accordingly
 */
void contact_list_accounts_changed();

/**
 * Add a new contact in the contact queue of corresponding account
 * @param accountID ID of the account to add the contact in
 * @param contact Informations on the contact to add
 * @param update True if it needs to modify the windows and send back information to the daemon
 */
void contact_list_add(gchar* accountID, contact_t* contact, gboolean update);

/**
 * Edit the contact in the contact queue of corresponding account
 * @param accountID ID of the account of the contact to edit
 * @param contact New information for the contact
 */
void contact_list_edit(gchar* accountID, contact_t* contact);

/**
 * Remove the contact in the contact queue of corresponding account
 * @param accountID ID of the account of the contact to remove
 * @param contactID ID of the contact to remove
 */
void contact_list_remove(gchar* accountID, gchar* contactID);

/**
 * Get size of the contact queue
 * @param contactList Queue to get size from
 * @return Size of the queue
 */
guint contact_list_get_size(GQueue* contactList);

/**
 * Get the contact corresponding to ID in a contact list
 * @param contactList The contact queue of an account to get the contact from
 * @param contactID ID of the contact to get in queue
 * @return The contact corresponding to contactID, null if not found
 */
contact_t* contact_list_get(GQueue* contactList, const gchar* contactID);

/**
 * Get the contact from its position in the contact queue
 * @param contactList The contact queue of an account to get the contact from
 * @param index The position in the queue
 * @return The contact corresponding to index, null if not found
 */
contact_t* contact_list_get_nth(GQueue* contactList, guint index);

/**
 * Add a new entry in the queue of corresponding contact
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entry Informations on the entry to add
 * @param update True if it needs to modify the windows and send back information to the daemon
 */
void contact_list_entry_add(gchar* accountID, gchar* contactID, contact_entry_t* entry, gboolean update);

/**
 * Edit the entry in the queue of corresponding contact
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entry New informations for the entry to edit
 */
void contact_list_entry_edit(gchar* accountID, gchar* contactID, contact_entry_t* entry);

/**
 * Remove the entry in the queue of corresponding contact
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entryID ID of the entry to remove
 */
void contact_list_entry_remove(gchar* accountID, gchar* contactID, gchar* entryID);

/**
 * Function when an update on presence status is received from server and only entry is known, not the contact
 * @param accountID ID of the account the signal was received from
 * @param entryID ID of the entry which presence changed
 * @param status New status of the entry
 * @param additionalInfo Optional information related to status
 */
void contact_list_entry_change_presence_status(const gchar* accountID, const gchar* entryID, const gchar* status, const gchar* additionalInfo);

/**
 * Get size of the entry queue
 * @param contact Contact to get size of entry queue from
 * @return Size of the queue
 */
guint contact_list_entry_get_size(contact_t* contact);

/**
 * Get the entry corresponding to ID in a entry queue
 * @param contact Contact who has the queue to get the entry from
 * @param entryID ID of the entry to get
 * @return The entry corresponding to entryID, null if not found
 */
contact_entry_t* contact_list_entry_get(contact_t* contact, const gchar* entryID);

/**
 * Get the entry from its position in the queue
 * @param contact The contact containing the queue to get the entry from
 * @param index The position in the queue
 * @return The entry corresponding to index, null if not found
 */
contact_entry_t* contact_list_entry_get_nth(contact_t* contact, guint index);

/**
 * Create contact struct from ID and a string array of details
 * @param contactID ID of the contact to build
 * @param details Null terminated string array of details
 * @return The constructed contact
 */
contact_t* contact_list_new_contact_from_details(gchar* contactID, gchar** details);

/**
 * Create contact entry struct from ID and a string array of details
 * @param contactEntryID ID of the entry to build
 * @param details Null terminated string array of details
 * @return The constructed entry
 */
contact_entry_t* contact_list_new_contact_entry_from_details(gchar* contactEntryID, gchar** details);

/**
 * Comparaison function to find contact in list by contact ID
 * @param a Contact pointer of the contact to compare
 * @param b String pointer of the ID to compare
 * @return 0 if the ID matches the ID of the contact, 1 elsewise
 */
gint compare_contact_contactID(gconstpointer a, gconstpointer b);

/**
 * Comparaison function to find contact in list by entry ID
 * @param a Entry pointer of the entry to compare
 * @param b String pointer of the ID to compare
 * @return 0 if the ID matches the ID of the entry, 1 elsewise
 */
gint compare_contact_contactEntryID(gconstpointer a, gconstpointer b);

/** 
 * Transform defined presence status obtained by DBus to a literal string representation
 * or sends back the same status if the signification of this status is not decided yet
 * or a no presence status if the status sent by the daemon is unknown
 * @param presenceStatus The defined presence status from presencestatus.h
 * @return The gettext translated literal representation of the status
 */
const gchar* contact_list_presence_status_translate(const gchar* presenceStatus);

/**
 * Transform defined presence status obtained by DBus to a string identifying the icon
 * location of the icon used to represent the status or unknown icon if no icon defined
 * @param presenceStatus The defined presence status from presencestatus.h
 * @return The file name of the icon representing the status
 */
const gchar* contact_list_presence_status_get_icon_string(const gchar* presenceStatus);

#endif
