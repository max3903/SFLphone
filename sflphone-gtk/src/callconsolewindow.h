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

#ifndef __CALLCONSOLEWINDOW_H__
#define __CALLCONSOLEWINDOW_H__

#include <gtk/gtk.h>

#include <contactlist.h>

/**
 * @file callconsolewindow.h
 * @brief Call console window to display dynamically contacts and presence
 * The call console window is used to present in a dynamic and simple way
 * information on contacts, their entries and presence information on them
 * The information presented depends on the contacts and their entries shown
 * in the contact window. It is possible to select which entries will be shown
 * in the call console and which will be subscribed to presence information
 * The call console is a list store which showns 4 columns. The first is the
 * presence icon related to the status of the entry, then the contact who this
 * entry belongs to, the name of the entry and finally the presence in text
 */

/** 
 * Empties and fill the model, meaning all the entries for registered accounts
 * only if the call console is not null
 */
void call_console_window_fill_contact_list();

/**
 * Clear the model
 */
void call_console_window_clear_contact_list();

/**
 * Show or close the call console window
 * Will also create and fill the model
 * @param show True if the window must be shown
 */
void show_call_console_window(gboolean show);

/**
 * Edit row in the call console with new information on contact
 * @param accountID ID of the account of the contact
 * @param contact New contact information
 */
void call_console_edit_contact(gchar* accountID, contact_t* contact);

/**
 * Remove rows in call console related to contact
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact
 */
void call_console_remove_contact(gchar* accountID, gchar* contactID);

/**
 * Add new row in call console described by entry and its related contact
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entry Entry information to be added
 */
void call_console_add_entry(gchar* accountID, gchar* contactID, contact_entry_t* entry);

/**
 * Edit row in call console described by entry and its related contact
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entry Entry information to be modified
 */
void call_console_edit_entry(gchar* accountID, gchar* contactID, contact_entry_t* entry);

/**
 * Remove row in call console described by the IDs
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entryID ID of the entry to be removed
 */
void call_console_remove_entry(gchar* accountID, gchar* contactID, gchar* entryID);

/**
 * Change presence only for row related to entry because signal was received from sflphoned
 * @param accountID ID of the account of the contact
 * @param contactID ID of the contact of the entry
 * @param entryID ID of the entry to be changed
 * @param presence New presence status
 * @param additionalInfo New optional information on the status
 */
void call_console_change_entry_presence_status(const gchar* accountID, const gchar* contactID,
		const gchar* entryID, const gchar* presence, const gchar* additionalInfo);

#endif
