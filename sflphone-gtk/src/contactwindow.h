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
 * @brief The Contacts window.
 */
void contact_window_fill_contact_list();
void contact_window_clear_contact_list();
void show_contact_window();
void show_contact_dialog(gchar* accountID, gchar* contactID);
void show_entry_dialog(gchar* accountID, gchar* contactID, gchar* entryID);

/**
 * Called from the contact list to update the view
 */
void contact_window_add_account(account_t* account);
void contact_window_remove_account(gchar* accountID);
void contact_window_add_contact(gchar* accountID, contact_t* contact);
void contact_window_edit_contact(gchar* accountID, contact_t* contact);
void contact_window_remove_contact(gchar* accountID, gchar* contactID);
void contact_window_add_entry(gchar* accountID, gchar* contactID, contact_entry_t* entry);
void contact_window_edit_entry(gchar* accountID, gchar* contactID, contact_entry_t* entry);
void contact_window_remove_entry(gchar* accountID, gchar* contactID, gchar* entryID);

#endif 
