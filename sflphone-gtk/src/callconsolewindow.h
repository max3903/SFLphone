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

/**
 * @file contactwindow.h
 * @brief The Contacts window.
 */
void call_console_window_fill_contact_list();
void show_call_console_window(gboolean show);

/**
 * Called from the contact list to update the view
 */
// NOW
void call_console_add_entry();
void call_console_edit_entry();
void call_console_remove_entry();
void call_console_change_entry_presence_status(const gchar* accountID, const gchar* contactID,
		const gchar* entryID, const gchar* presence, const gchar* additionalInfo);

#endif 
