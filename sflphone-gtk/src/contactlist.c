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

#include "actions.h"
#include "callconsolewindow.h"
#include "contactlist.h"
#include "contactwindow.h"
#include "dbus.h"

#include <string.h>

/**
 * Hash table that will contain information on all contacts and their entries
 * It is the underlying model of the contact window and call console and all
 * modifications to these views are regrouped here and thus synchronized
 * The keys of the hash table are the accountIDs and the values are GQueues
 * that contain contact_t structs which contain a GQueue of contact_entry_t
 */
GHashTable* contactHashTable = NULL;

/** Custom function to free memory in hash table for contact lists */
void
contact_hash_value_destroy_func(void* valuePointer)
{
	GQueue* contactList = (GQueue*)valuePointer;
	g_queue_free(contactList);
}

void
contact_hash_table_init()
{
	// Create new table with hash and equal function for keys and destroy functions for key and value
	if(contactHashTable == NULL)
		contactHashTable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, contact_hash_value_destroy_func);
}

void
contact_hash_table_clear()
{
	// Destroys the hash table that will use its destroy functions on keys and values
	g_hash_table_destroy(contactHashTable);
	contactHashTable = NULL;
}

void
contact_hash_table_add_contact_list(gchar* accountID)
{
	// Dynamically allocate a queue in the hash table
	g_hash_table_insert(contactHashTable, g_strdup(accountID), g_queue_new());
	
	// TODO Create a new row in the contact window for the new account
}

void
contact_hash_table_remove_contact_list(gchar* accountID)
{
	// TODO Remove all contacts and then remove the contact list
	
	// TODO Remove everything in the views for this account
}

GQueue*
contact_hash_table_get_contact_list(const gchar* accountID)
{
	return (GQueue*)g_hash_table_lookup(contactHashTable, accountID);
}

void
contact_list_add(gchar* accountID, contact_t* contact, gboolean update)
{
	// Modify the data
	g_queue_push_tail(contact_hash_table_get_contact_list(accountID), (void*)contact);

	// Modify view
	if(update) contact_window_add_contact(accountID, contact);
	
	// Send modifications to server
	if(update) dbus_set_contact(accountID, contact->_contactID, contact->_firstName, contact->_lastName, contact->_email);
}

void
contact_list_edit(gchar* accountID, contact_t* contact)
{
	// Modify the data in the contact list
	contact_t* oldContact = contact_list_get(contact_hash_table_get_contact_list(accountID), contact->_contactID);
	oldContact->_firstName = contact->_firstName;
	oldContact->_lastName = contact->_lastName;
	oldContact->_email = contact->_email;

	// Modify views
	contact_window_edit_contact(accountID, contact);
	call_console_edit_contact(accountID, contact);
	
	// Send modifications to server
	dbus_set_contact(accountID, contact->_contactID, contact->_firstName, contact->_lastName, contact->_email);
}

void
contact_list_remove(gchar* accountID, gchar* contactID)
{
	// Modify data
	GQueue* contactList = contact_hash_table_get_contact_list(accountID);
	GList* contacts = g_queue_find_custom(contactList, contactID, compare_contact_contactID);
	if(contacts)
	{
		// Remove all entries and remove contact
		contact_t* contact = (contact_t*)contacts->data;
		g_queue_clear(contact->_entryList);
		g_queue_remove(contactList, contacts->data);
	}
	
	// Modify both views
	// TODO
	contact_window_remove_contact(accountID, contactID);
	call_console_remove_contact(accountID, contactID);
	
	// Send modifications to server
	// TOSEE
	//dbus_remove_contact(accountID, contactID);
}

guint
contact_list_get_size(GQueue* contactList)
{
	return g_queue_get_length(contactList);
}

contact_t*
contact_list_get(GQueue* contactList, const gchar* contactID)
{
	GList* contacts = g_queue_find_custom(contactList, contactID, compare_contact_contactID);
	if(contacts)
		return (contact_t*)contacts->data;
	else
		return NULL;
}

contact_t*
contact_list_get_nth(GQueue* contactList, guint index)
{
	return g_queue_peek_nth(contactList, index);
}

void
contact_list_entry_add(gchar* accountID, gchar* contactID, contact_entry_t* entry, gboolean update)
{
	contact_t* contact = contact_list_get(contact_hash_table_get_contact_list(accountID), contactID);
	// Assert that this entry ID is not already in use or return
	if(contact_list_entry_get(contact, entry->_entryID) != NULL) return;
	// Modify the data in contact list
	g_queue_push_tail(contact->_entryList, (void*)entry);
	
	// Modify the views
	if(update)
	{
		contact_window_add_entry(accountID, contactID, entry);
		call_console_add_entry(accountID, contactID, entry);
	}

	// Send modifications to server
	if(update)
	{
		gchar isShown[6], isSubscribed[6];
		if(entry->_isShownInConsole) strcpy(isShown, "TRUE");
		else strcpy(isShown, "FALSE");
		if(entry->_isSubscribed) strcpy(isSubscribed, "TRUE");
		else strcpy(isSubscribed, "FALSE");
		dbus_set_contact_entry(accountID, contactID, entry->_entryID,
				entry->_text, entry->_type, isShown, isSubscribed);
	}
}

void
contact_list_entry_edit(gchar* accountID, gchar* contactID, contact_entry_t* entry)
{
	// Modify contact list
	contact_entry_t* oldEntry = contact_list_entry_get(contact_list_get(contact_hash_table_get_contact_list(accountID), contactID), entry->_entryID);
	oldEntry->_text = entry->_text;
	oldEntry->_type = entry->_type;
	oldEntry->_isShownInConsole = entry->_isShownInConsole;
	oldEntry->_isSubscribed = entry->_isSubscribed;
		
	// Modify both views
	contact_window_edit_entry(accountID, contactID, entry);
	call_console_edit_entry(accountID, contactID, entry);
	
	// Send modifications to server
	dbus_set_contact_entry(accountID, contactID, entry->_entryID, entry->_text, entry->_type,
			entry->_isShownInConsole ? "TRUE" : "FALSE", entry->_isSubscribed ? "TRUE" : "FALSE");
}

void
contact_list_entry_remove(gchar* accountID, gchar* contactID, gchar* entryID)
{
	// Modify contact list
	contact_t* contact = contact_list_get(contact_hash_table_get_contact_list(accountID), contactID);
	GList* entries = g_queue_find_custom(contact->_entryList, entryID, compare_contact_contactEntryID);
	if(entries) g_queue_remove(contact->_entryList, entries->data);
	
	// Modify both views
	contact_window_remove_entry(accountID, contactID, entryID);
	call_console_remove_entry(accountID, contactID, entryID);
	
	// Send modifications to server
	// FIXME
	//dbus_remove_contact_entry(accountID, contactID, entryID);
}

void
contact_list_entry_change_presence_status(const gchar*accountID, const gchar* entryID, const gchar* status, const gchar* additionalInfo)
{
	GQueue* contactList = contact_hash_table_get_contact_list(accountID);
	if(contactList != NULL)
	{
		// Look in all contacts if the specified entry can be found
		int i;
		contact_t* contact;
		for(i = 0; i < contact_list_get_size(contactList); i++)
		{
			contact = contact_list_get_nth(contactList, i);
			if(contact != NULL)
			{
				contact_entry_t* entry = contact_list_entry_get(contact, entryID);
				if(entry != NULL)
				{
					entry->_presenceStatus = g_strdup(status);
					entry->_presenceInfo = g_strdup(additionalInfo);

					// Apply changes to call console
					call_console_change_entry_presence_status(accountID, contact->_contactID, entryID, status, additionalInfo);
					
					// Do not return since the same entry could have more than one contact
				}
			}
		}
	}
}

guint
contact_list_entry_get_size(contact_t* contact)
{
	return g_queue_get_length(contact->_entryList);
}

contact_entry_t* contact_list_entry_get(contact_t* contact, const gchar* entryID)
{
	GList* entries = g_queue_find_custom(contact->_entryList, entryID, compare_contact_contactEntryID);
	if(entries)
		return (contact_entry_t*)entries->data;
	else
		return NULL;
}

contact_entry_t* contact_list_entry_get_nth(contact_t* contact, guint index)
{
	return g_queue_peek_nth(contact->_entryList, index);
}

contact_t*
contact_list_new_contact_from_details(gchar* contactID, gchar** contactDetails)
{
	contact_t* contact = g_new0(contact_t, 1);
	contact->_contactID = g_strdup(contactID);
	contact->_firstName = g_strdup(contactDetails[0]);
	contact->_lastName = g_strdup(contactDetails[1]);
	contact->_email = g_strdup(contactDetails[2]);
	contact->_entryList = g_queue_new();
	return contact;
}

contact_entry_t*
contact_list_new_contact_entry_from_details(gchar* contactEntryID, gchar** contactEntryDetails)
{
	contact_entry_t* contactEntry = g_new0(contact_entry_t, 1);
	contactEntry->_entryID = g_strdup(contactEntryID);
	contactEntry->_text = g_strdup(contactEntryDetails[0]);
	contactEntry->_type = g_strdup(contactEntryDetails[1]);
	if(strcmp(contactEntryDetails[2], "TRUE") == 0)
		contactEntry->_isShownInConsole = TRUE;
	else
		contactEntry->_isShownInConsole = FALSE;
	if(strcmp(contactEntryDetails[3], "TRUE") == 0)
		contactEntry->_isSubscribed = TRUE;
	else
		contactEntry->_isSubscribed = FALSE;
	contactEntry->_presenceStatus = g_strdup(contactEntryDetails[4]);
	contactEntry->_presenceInfo = g_strdup(contactEntryDetails[5]);
	
	return contactEntry;
}

gint
compare_contact_contactID(gconstpointer a, gconstpointer b)
{
	contact_t* contact = (contact_t*)a;
	if(strcmp(contact->_contactID, (gchar*)b) == 0)
		return 0;
	else
		return 1;
}

gint
compare_contact_contactEntryID(gconstpointer a, gconstpointer b)
{
	contact_entry_t* contactEntry = (contact_entry_t*)a;
	if(strcmp(contactEntry->_entryID, (gchar*)b) == 0)
		return 0;
	else
		return 1;
}

const gchar*
contact_list_presence_status_translate(const gchar* presenceStatus)
{
	// TODO Some active statuses are not translated since we want
	// to know in which precise cases these signals are sent by
	// the call manager and should also be defined in the daemon
	
	// Special statuses
	if(strcmp(presenceStatus, PRESENCE_NOT_SUBSCRIBED) == 0)
	{
		return(_("Not subscribed"));
	}
	if(strcmp(presenceStatus, PRESENCE_NOT_INITIALIZED) == 0)
	{
		// Return offline for now because not all subscriptions return a receipt
		return(_("Unreachable"));
	}
	if(strcmp(presenceStatus, PRESENCE_NOT_SUPPORTED) == 0)
	{
		return(_("Not supported"));
	}
	
	// Active statuses
	if(strcmp(presenceStatus, PRESENCE_UNKNOWN) == 0)
	{
		return(PRESENCE_UNKNOWN);
	}
	if(strcmp(presenceStatus, PRESENCE_READY) == 0) {
		return(_("Online"));
	}
	if(strcmp(presenceStatus, PRESENCE_IS_BUSY) == 0) {
		return(PRESENCE_IS_BUSY);
	}
	if(strcmp(presenceStatus, PRESENCE_INVALID) == 0) {
		return(PRESENCE_INVALID);
	}
	if(strcmp(presenceStatus, PRESENCE_UNAVAILABLE) == 0) {
		return(_("Offline"));
	}
	if(strcmp(presenceStatus, PRESENCE_ON_THE_PHONE) == 0) {
		return(_("On the Phone"));
	}
	if(strcmp(presenceStatus, PRESENCE_RINGING) == 0) {
		return(_("Ringing"));
	}
	if(strcmp(presenceStatus, PRESENCE_RING_IN_USE) == 0) {
		return(PRESENCE_RING_IN_USE);
	}
	if(strcmp(presenceStatus, PRESENCE_HOLD_IN_USE) == 0) {
		return(PRESENCE_HOLD_IN_USE);
	}
	if(strcmp(presenceStatus, PRESENCE_ON_HOLD) == 0) {
		return(_("On Hold"));
	}
	// Passive statuses
	if(strcmp(presenceStatus, PRESENCE_ONLINE) == 0) {
		return(_("Online"));
	}
	if(strcmp(presenceStatus, PRESENCE_BUSY) == 0) {
		return(_("Busy"));
	}
	if(strcmp(presenceStatus, PRESENCE_BE_RIGHT_BACK) == 0) {
		return(_("Be Right Back"));
	}
	if(strcmp(presenceStatus, PRESENCE_AWAY) == 0) {
		return(_("Away"));
	}
	if(strcmp(presenceStatus, PRESENCE_OUT_TO_LUNCH) == 0) {
		return(_("Out to Lunch"));
	}
	if(strcmp(presenceStatus, PRESENCE_OFFLINE) == 0) {
		return(_("Offline"));
	}
	if(strcmp(presenceStatus, PRESENCE_DO_NOT_DISTURB) == 0) {
		return(_("Do not Disturb"));
	}
	return _("Unknown");
}

const gchar*
contact_list_presence_status_get_icon_string(const gchar* presenceStatus)
{
	// TODO Complete icons to replace the default unknwon icon

	// Special statuses
	if(strcmp(presenceStatus, PRESENCE_NOT_SUBSCRIBED) == 0)
	{
		return(PRESENCE_NOT_SUBSCRIBED_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_NOT_INITIALIZED) == 0)
	{
		return(PRESENCE_NOT_INITIALIZED_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_NOT_SUPPORTED) == 0)
	{
		return(PRESENCE_NOT_SUPPORTED_ICON);
	}

	// Active statuses
	if(strcmp(presenceStatus, PRESENCE_UNKNOWN) == 0)
	{
		return(PRESENCE_UNKNOWN_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_READY) == 0) {
		return(PRESENCE_ONLINE_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_IS_BUSY) == 0) {
		return(PRESENCE_UNKNOWN_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_INVALID) == 0) {
		return(PRESENCE_UNKNOWN_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_UNAVAILABLE) == 0) {
		return(PRESENCE_OFFLINE_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_ON_THE_PHONE) == 0) {
		return(PRESENCE_ON_THE_PHONE_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_RINGING) == 0) {
		return(PRESENCE_RINGING_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_RING_IN_USE) == 0) {
		return(PRESENCE_RINGING_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_HOLD_IN_USE) == 0) {
		return(PRESENCE_ON_HOLD_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_ON_HOLD) == 0) {
		return(PRESENCE_ON_HOLD_ICON);
	}
	// Passive statuses
	if(strcmp(presenceStatus, PRESENCE_ONLINE) == 0) {
		return(PRESENCE_ONLINE_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_BUSY) == 0) {
		return(PRESENCE_BUSY_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_BE_RIGHT_BACK) == 0) {
		return(PRESENCE_BE_RIGHT_BACK_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_AWAY) == 0) {
		return(PRESENCE_AWAY_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_OUT_TO_LUNCH) == 0) {
		return(PRESENCE_OUT_TO_LUNCH_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_OFFLINE) == 0) {
		return(PRESENCE_OFFLINE_ICON);
	}
	if(strcmp(presenceStatus, PRESENCE_DO_NOT_DISTURB) == 0) {
		return(PRESENCE_DO_NOT_DISTURB_ICON);
	}
	return PRESENCE_UNKNOWN_ICON;
}
