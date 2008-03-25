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

#include "contactlist.h"
#include "dbus.h"

#include <string.h>

GHashTable* contactHashTable = NULL;

void
contact_hash_table_init()
{
	if(contactHashTable == NULL)
		contactHashTable = g_hash_table_new(NULL, g_str_equal);
}

void
contact_hash_table_clear_list(void* a, void* b)
{
	gchar* key = (gchar*)a;
	contact_hash_table_clear_contact_list(contact_hash_table_get_contact_list(key));
}

void
contact_hash_table_clear()
{
	// TODO Unregister observers (contact and call console windows)
	// Clear all contact lists
	GList* list = g_hash_table_get_keys(contactHashTable);
	g_list_foreach(list, contact_hash_table_clear_list, NULL);
	g_hash_table_destroy(contactHashTable);
	contactHashTable = NULL;
}

GQueue*
contact_hash_table_add_contact_list(gchar* accountID)
{
	GQueue* contactList = g_queue_new();
	g_hash_table_insert(contactHashTable, accountID, contactList);
	return contactList;
}

GQueue*
contact_hash_table_get_contact_list(gchar* accountID)
{
	return (GQueue*)g_hash_table_lookup(contactHashTable, accountID);
}

void
contact_hash_table_clear_contact_list(GQueue* contactList)
{
	// TODO Clear all contact and entries
	g_queue_free(contactList);
}

void
contact_list_add(GQueue* contactList, contact_t* contact)
{
	g_queue_push_tail(contactList, (void*) contact);
}

void
contact_list_edit(GQueue* contactList, contact_t* oldContact, contact_t* newContact)
{
	// TODO	
}

void
contact_list_remove(GQueue* contactList, contact_t* contact)
{
	// TODO
}

guint
contact_list_get_size(GQueue* contactList)
{
	return g_queue_get_length(contactList);
}

contact_t*
contact_list_get(GQueue* contactList, const gchar* contactID)
{
	// TOSEE
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
contact_list_entry_add(contact_t* contact, contact_entry_t* entry)
{
	g_queue_push_tail(contact->_entryList, (void*) entry);
}

void
contact_list_entry_edit(contact_t* contact, contact_entry_t* oldEntry, contact_entry_t* newEntry)
{
	// TODO
}

void
contact_list_entry_remove(contact_t* contact, contact_entry_t* entry)
{
	// TODO
}

guint
contact_list_entry_get_size(contact_t* contact)
{
	return g_queue_get_length(contact->_entryList);
}

contact_entry_t* contact_list_entry_get(contact_t* contact, const gchar* entryID)
{
	// TOSEE
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
	contact->_contactID = contactID;
	contact->_firstName = contactDetails[0];
	contact->_lastName = contactDetails[1];
	contact->_email = contactDetails[2];
	contact->_group = contactDetails[3];
	contact->_subGroup = contactDetails[4];
	contact->_entryList = g_queue_new();
	return contact;
}

contact_entry_t*
contact_list_new_contact_entry_from_details(gchar* contactEntryID, gchar** contactEntryDetails)
{
	contact_entry_t* contactEntry = g_new0(contact_entry_t, 1);
	contactEntry->_entryID = contactEntryID;
	contactEntry->_text = contactEntryDetails[0];
	contactEntry->_type = contactEntryDetails[1];
	if(strcmp(contactEntryDetails[2], "TRUE") == 0)
		contactEntry->_isShownInConsole = TRUE;
	else
		contactEntry->_isShownInConsole = FALSE;
	if(strcmp(contactEntryDetails[3], "TRUE") == 0)
		contactEntry->_isSubscribed = TRUE;
	else
		contactEntry->_isSubscribed = FALSE;
	contactEntry->_presence = NULL;
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
