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

/** 
 * @file	contactlist.h
 * @brief	A hash table that holds contact lists mapped by their account ID
 * 			It holds contacts, their entries and their presence information if needed
 */

typedef struct {
	gchar*		_status;				// Presence status
	gchar*		_additionalInfo;		// Additional info on status
} presence_t;

typedef struct {
	gchar*		_entryID;				// Contact number, can be a phone number, an extension... (202, 5141231234, sip:141@...)
	gchar*		_text;					// Simplified textual representation (Poste 202, 514-123-1234, Poste 141)
	gchar*		_type;					// Work, home, cell...
	gboolean	_isShownInConsole;		// Is shown in the call console
	gboolean	_isSubscribed;			// Is contact subscribed to presence
	presence_t*	_presence;				// Presence information obtained if entry is subscribed
} contact_entry_t;

typedef struct {
	gchar*		_contactID;				// Unique id for contact by account
	gchar*		_firstName;
	gchar*		_lastName;
	gchar*		_email;
	gchar*		_group;					// Contacts can be regrouped
	gchar*		_subGroup;				// up to two levels of hierarchy
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
void contact_hash_table_add_contact_list(gchar* accountID);
GQueue* contact_hash_table_get_contact_list(gchar* accountID);
void contact_hash_table_clear_contact_list(GQueue* contactList);

/**
 * Functions to add, edit and remove a contact in a contact list
 */
void contact_list_add(GQueue* contactList, contact_t* contact);
void contact_list_edit(GQueue* contactList, gchar* contactID, contact_t* newContact);
void contact_list_remove(GQueue* contactList, gchar* contactID);

/**
 * Functions to get a particular contact in the list and to get the list size
 */
guint contact_list_get_size(GQueue* contactList);
contact_t* contact_list_get(GQueue* contactList, const gchar* contactID);
contact_t* contact_list_get_nth(GQueue* contactList, guint index);

/**
 * Functions to add, get, edit and remove a contact entry in a contact
 */
void contact_list_entry_add(contact_t* contact, contact_entry_t* entry);
void contact_list_entry_edit(contact_t* contact, gchar* entryID, contact_entry_t* newEntry);
void contact_list_entry_remove(contact_t* contact, gchar* entryID);

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

#endif

// TOSEE
//void contact_show_in_console(GQueue* contactList, gchar* contactID, gboolean show);
//void contact_subscribe(GQueue* contactList, gchar* contactID, gboolean subscribe);
//void contact_edit(GQueue* contactList, gchar* contactID, gchar* newContactID, gchar* firstName, gchar* lastName);

//void contact_list_move_contact_up(GQueue* contactList, guint index);
//void contact_list_move_contact_down(GQueue* contactList, guint index);
//void contact_list_update_to_daemon(GQueue* contactList);

