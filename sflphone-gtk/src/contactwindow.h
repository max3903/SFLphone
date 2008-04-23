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

#ifndef __CONTACTWINDOW_H__
#define __CONTACTWINDOW_H__

#include <gtk/gtk.h>

#include "accountlist.h"
#include "contactlist.h"

/**
 * @file contactwindow.h
 * @brief The window used for editing all contacts and their entries by account
 * It is a tree view whose 3 depths are the accounts, the contacts and the entries
 * There is only one column rendered by 3 different renderers, a check box used to
 * present if the entry is shown in call console, an icon for the type of the row
 * (account, contact or entry), and finally a text renderer to display the name
 * of the accounts, the full names of the contacts and the names of the entries
 * Actions to add, edit and remove contacts and entries are available via the
 * alternate mouse menu and will show popup dialogs depending on actions
 * The modifications to the model are sent directly to the contact list which will
 * update automatically the model, the 2 windows and the state of the daemon
 */

/**
 * Clear contact store and fill all accounts, contacts and entries in tree view
 * Fills the treelist with contacts and entries grouped by accounts
 * Only the enabled accounts will be expanded by default
 */
void contact_window_fill_contact_list();

/**
 * Clear the contact tree store, the model of this window
 */
void contact_window_clear_contact_list();

/**
 * Main function that will create the tree store, fill it and display the window
 */
void show_contact_window();

/**
 * Show the contact dialog for adding or editing a contact
 * Creation or edition of contact will be triggered if accepted
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact to edit or NULL to add a new contact
 */
void show_contact_dialog(gchar* accountID, gchar* contactID);

/**
 * Show the entry dialog for adding or editing an entry
 * Creation or edition of entry will be triggered if accepted
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entryID ID of the entry to edit or NULL to add a new entry
 */
void show_entry_dialog(gchar* accountID, gchar* contactID, gchar* entryID);

/**
 * Add a contact in the contact window tree
 * @param accountID ID of the account of the contact
 * @param contact The contact to add
 */
void contact_window_add_contact(gchar* accountID, contact_t* contact);

/**
 * Edit a contact in the contact window tree
 * @param accountID ID of the account of the contact
 * @param contact The contact to edit
 */
void contact_window_edit_contact(gchar* accountID, contact_t* contact);

/**
 * Remove a contact in the contact window tree
 * @param accountID ID of the account of the contact
 * @param contact The contact to remove
 */
void contact_window_remove_contact(gchar* accountID, gchar* contactID);

/**
 * Add an entry in the contact window tree
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entry The entry to add
 */
void contact_window_add_entry(gchar* accountID, gchar* contactID, contact_entry_t* entry);

/**
 * Edit an entry in the contact window tree
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entry The entry to edit
 */
void contact_window_edit_entry(gchar* accountID, gchar* contactID, contact_entry_t* entry);

/**
 * Remove an entry in the contact window tree
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entry The entry to remove
 */
void contact_window_remove_entry(gchar* accountID, gchar* contactID, gchar* entryID);

#endif
