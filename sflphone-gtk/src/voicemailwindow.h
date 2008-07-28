/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Florian DESPORTES <florian.desportes@savoirfairelinux.com>
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

#ifndef _VOICE_MAIL_H_
#define _VOICE_MAIL_H_ 1

#include <sflphone_const.h>
#include <gtk/gtk.h>

//-------------------------------------
// TREEVIEW MAINPULATION
//-------------------------------------

/**
 * Gets the GtkIter corresponding to the selected item from the treeview
 * @return GtkTreeIter the iter related to the selected item
 */
GtkTreeIter getSelectedItem(void);

/**
 * Checks whether an item is selected in the treeview
 * @return gboolean true if any item is selected, false otherwise
 */
gboolean isItemSelected(void);

/**
 * Checks whether a selected item is a folder or a voicemail
 * @return gboolean true if the selected item is a voicemail, false otherwise
 */
gboolean isAValidItem(void);

/**
 * Gets the name of the folder containing the current selected item (voicemail)
 * @param  GtkTreeIter iter corresponding to the selected voicemail
 * @return gchar* the name of the folder containing the voicemail
 */
gchar * getSelectedItemParentName(GtkTreeIter);


//-------------------------------------
// GUI FUNCTIONS
//-------------------------------------

/**
 * Shows the voicemail viewer window and start dialog between GUI and web-agent
 */
void show_voicemail_window(void);

/**
 * Creates litteraly the voicemail window
 */
void create_voicemail_window(void);

/**
 * Creates the treeview and its model, doesn't fill it !
 */
void create_tree(void);

/**
 * Fills the treeview with all the folders
 * @param gchar* the 'markuped' name of the folder (example : '<b>INBOX</b>'
 * @param gchar* the single name of the folder     (example : 'INBOX')
 */
void update_tree(gchar *, gchar *);

/**
 * Fills the treeview with a list of voicemails according to a specified folder
 * @param gchar** the list of all the voicemail's name
 * @param int the number of the folder to fill voicemails in (example : Inbox = 0, Old = 1...)
 */
void update_tree_complete(gchar **, int);


//-------------------------------------
// DBUS SIGNALS
//-------------------------------------

/**
 * Signal from sflphoned indicating a voicemail is playing
 */
void voicemail_is_playing();

/**
 * Signal from sflphoned indicating a voicemail has been stopped or is finished to play
 */
void voicemail_is_stopped();

#endif
