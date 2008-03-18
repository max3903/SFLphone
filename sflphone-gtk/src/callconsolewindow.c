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

#include "../../src/contact/presencestatus.h"

#include <accountlist.h>
#include <contactlist.h>
#include <config.h>
#include <mainwindow.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

/**
 * Defines the column of the tree model for each renderer of a row
 */
enum {
	CALL_CONSOLE_WINDOW_ICON,				// Icon for presence status
	CALL_CONSOLE_WINDOW_NAME,				// Name of the contact for the entry
	CALL_CONSOLE_WINDOW_CONTACT,			// Contact entry (extension, phone number...)
	CALL_CONSOLE_WINDOW_PRESENCE_STATUS,	// Text presenting presence status and additional information
	COUNT_CALL_CONSOLE_WINDOW,				// Column count
};

// Location of pixmaps icons to represent different presence status
// TMP Should be related to presencestatus.h
#define PRESENCE_STATUS_ONLINE_ICON		ICONS_DIR "/current.svg"

GtkDialog* dialog = NULL;			// This window
GtkWidget* contactTreeView;			// View
GtkListStore* contactListStore;		// Model

/**
 * Fills the treelist with accounts
 */
void
call_console_window_fill_contact_list()
{
	gtk_list_store_clear(contactListStore);
		
	// Fill contacts for all loaded accounts
	int i;
	for(i = 0; i < account_list_get_size(); i++)
	{
		account_t* account = account_list_get_nth (i);
		if(account->state == ACCOUNT_STATE_REGISTERED &&
				strcmp((gchar*)g_hash_table_lookup(account->properties, ACCOUNT_ENABLED), "TRUE") == 0)
		{			
			// Get contact list for account
			GQueue* contactList = contact_hash_table_get_contact_list(account->accountID);
			int j;
			for(j = 0; j < contact_list_get_size(contactList); j++)
			{
				contact_t* contact = contact_list_get_nth(contactList, j);
				
				int k;
				for(k = 0; k < contact_list_entry_get_size(contact); k++)
				{
					contact_entry_t* entry = contact_list_entry_get_nth(contact, k);
					GtkTreeIter iter;
					// Append the contact entry in the list if shown in call console is true
					if(entry->_isShownInConsole)
					{
						gtk_list_store_append(contactListStore, &iter);
						gtk_list_store_set(contactListStore, &iter,
								CALL_CONSOLE_WINDOW_ICON, gdk_pixbuf_new_from_file(PRESENCE_STATUS_ONLINE_ICON, NULL),
								CALL_CONSOLE_WINDOW_NAME, contact->_firstName,
								CALL_CONSOLE_WINDOW_CONTACT, entry->_text,
								CALL_CONSOLE_WINDOW_PRESENCE_STATUS, "Put the presence status in text and additional info",
								-1);
					}
				}
			}
		}
	}
}

/**
 * Show contact window
 */
GtkDialog*
show_call_console_window(gboolean show)
{
	GtkWidget* scrolledWindow;
	GtkCellRenderer* renderer;
	GtkTreeViewColumn* treeViewColumn;
	
	// Close window if show is false
	if(!show)
	{
		if(dialog != NULL)
			gtk_widget_destroy(GTK_WIDGET(dialog));
		dialog = NULL;
		return NULL;
	}

	// Create dialog and set properties
	dialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Call console",
				GTK_WINDOW(get_main_window()),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				NULL));
	gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
	gtk_dialog_set_has_separator(dialog, FALSE);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 600, 400);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 0);
	
	// Put contacts in a scrollable window
	scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), scrolledWindow, TRUE, TRUE, 0);
	gtk_widget_show(scrolledWindow);
	
	// Create list store with contact entries regrouped by accounts and contacts
	contactListStore = gtk_list_store_new(COUNT_CALL_CONSOLE_WINDOW,
			GDK_TYPE_PIXBUF,	// Icon
			G_TYPE_STRING,		// Shown in call console active property
			G_TYPE_STRING,		// Shown in call console inconsistent property
			G_TYPE_STRING		// Text
			);
	
	contactTreeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(contactListStore));
	
	// Presence status icon column
	renderer = gtk_cell_renderer_pixbuf_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes("", renderer, "pixbuf", CALL_CONSOLE_WINDOW_ICON, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(contactTreeView), treeViewColumn);

	// Name column
	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes("Contact name", renderer, "text", CALL_CONSOLE_WINDOW_NAME, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(contactTreeView), treeViewColumn);

	// Contact column
	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes("Contact", renderer, "text", CALL_CONSOLE_WINDOW_CONTACT, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(contactTreeView), treeViewColumn);

	// Presence status column
	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes("Presence status", renderer, "text", CALL_CONSOLE_WINDOW_PRESENCE_STATUS, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(contactTreeView), treeViewColumn);
		
	gtk_container_add(GTK_CONTAINER(scrolledWindow), contactTreeView);
	gtk_container_set_border_width(GTK_CONTAINER(contactTreeView), 10);
	gtk_widget_show(contactTreeView);
	
	// Fill tree model
	call_console_window_fill_contact_list();

	// Place window side to side with main window
	GtkWindow* mainWindow = GTK_WINDOW(get_main_window());
	GdkScreen* screen = gtk_window_get_screen(mainWindow);
	gint width, height, rootX, rootY, screenW, screenH;
	screenW = gdk_screen_get_width(screen);
	screenH = gdk_screen_get_height(screen);
	gtk_window_get_size(mainWindow, &width, &height);
	gtk_window_get_position(mainWindow, &rootX, &rootY);
	gtk_window_set_gravity(GTK_WINDOW(dialog), GDK_WINDOW_EDGE_NORTH_WEST);
	gtk_window_move(GTK_WINDOW(dialog), rootX + width + 8, rootY);
	
	// Show window
	gtk_widget_show(GTK_WIDGET(dialog));
	
	return dialog;
}
