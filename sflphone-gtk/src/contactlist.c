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
contact_hash_table_clear()
{
	// TODO Clear all contact lists
	g_hash_table_destroy(contactHashTable);
	contactHashTable = NULL;
}

GQueue*
contact_hash_table_add_contact_list(gchar* accountID)
{
	GQueue* contactList;
	contactList = g_queue_new();
	g_hash_table_insert(contactHashTable, accountID, contactList);
	
	return contactList;
}

GQueue*
contact_hash_table_get_contact_list(gchar* accountID)
{
	GQueue* contactList;
	contactList = (GQueue*)g_hash_table_lookup(contactHashTable, accountID);
	
	return contactList;
}

void
contact_hash_table_clear_contact_list(GQueue* contactList)
{
	// TODO
	g_queue_free(contactList);
}

void
contact_list_add(GQueue* contactList, contact_t* contact)
{
	g_queue_push_tail(contactList, (gpointer*) contact);
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

gint
compare_contact_contactID(gconstpointer a, gconstpointer b)
{
	contact_t* contact = (contact_t*)a;
	if(strcmp(contact->_contactID, (gchar*)b) == 0)
		return 0;
	else
		return 1;
}
