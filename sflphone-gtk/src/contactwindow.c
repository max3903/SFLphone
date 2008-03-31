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
	CONTACT_WINDOW_TYPE,						// String to know the type of the current row (account, contact or entry)
	CONTACT_WINDOW_ID,							// ID of the account, the contact or the entry
	CONTACT_WINDOW_CALL_CONSOLE_ACTIVE,			// Toggle renderer active
	CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT,	// Toggle renderer inconsistent
	CONTACT_WINDOW_ICON,						// String of icon
	CONTACT_WINDOW_TEXT,						// Text for account, contact or contact entry presentation
	COUNT_CONTACT_WINDOW,						// Column count
};

// Location of pixmaps icons to represent different row types
// TMP Change pixmpas for each type
#define CONTACT_WINDOW_ACCOUNT_ICON		ICONS_DIR "/dial.svg"
#define CONTACT_WINDOW_CONTACT_ICON		ICONS_DIR "/ring.svg"
#define CONTACT_WINDOW_ENTRY_ICON		ICONS_DIR "/current.svg"

#define TYPE_ACCOUNT	0
#define TYPE_CONTACT	1
#define TYPE_ENTRY		2

GtkWidget* contactTreeView;
GtkTreeStore* contactTreeStore;

GtkWidget* accountMenu;
GtkWidget* contactMenu;
GtkWidget* entryMenu;

/**
 * Fills the treelist with contacts and entries grouped by accounts
 */
void
contact_window_fill_contact_list()
{
	gtk_tree_store_clear(contactTreeStore);
		
	// Fill contacts for all loaded accounts
	int i;
	for(i = 0; i < account_list_get_size(); i++)
	{
		account_t* account = account_list_get_nth (i);
		if(account->state == ACCOUNT_STATE_REGISTERED &&
				strcmp((gchar*)g_hash_table_lookup(account->properties, ACCOUNT_ENABLED), "TRUE") == 0)
		{
			GtkTreeIter accountIter;
			// Append the account in the list
			gtk_tree_store_append(contactTreeStore, &accountIter, NULL);
			gtk_tree_store_set(contactTreeStore, &accountIter,
					CONTACT_WINDOW_TYPE, TYPE_ACCOUNT,
					CONTACT_WINDOW_ID, account->accountID,
					CONTACT_WINDOW_CALL_CONSOLE_ACTIVE, TRUE,
					CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT, TRUE,		// Should be a function to pass all contacts if all selected or not
					CONTACT_WINDOW_ICON, gdk_pixbuf_new_from_file(CONTACT_WINDOW_ACCOUNT_ICON, NULL),
					CONTACT_WINDOW_TEXT, (gchar*)g_hash_table_lookup(account->properties, ACCOUNT_ALIAS),
					-1);
			
			// Get contact list for account
			GQueue* contactList = contact_hash_table_get_contact_list(account->accountID);
			int j;
			for(j = 0; j < contact_list_get_size(contactList); j++)
			{
				contact_t* contact = contact_list_get_nth(contactList, j);
				GtkTreeIter contactIter;
				// Append the contact in the list
				gchar fullName[1000];
				sprintf(fullName, "%s %s", contact->_firstName, contact->_lastName);
				gtk_tree_store_append(contactTreeStore, &contactIter, &accountIter);
				gtk_tree_store_set(contactTreeStore, &contactIter,
						CONTACT_WINDOW_TYPE, TYPE_CONTACT,
						CONTACT_WINDOW_ID, contact->_contactID,
						CONTACT_WINDOW_CALL_CONSOLE_ACTIVE, TRUE,
						CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT, TRUE,		// Should be a function to pass all contacts if all selected or not
						CONTACT_WINDOW_ICON, gdk_pixbuf_new_from_file(CONTACT_WINDOW_CONTACT_ICON, NULL),
						CONTACT_WINDOW_TEXT, fullName,
						-1);
				
				int k;
				for(k = 0; k < contact_list_entry_get_size(contact); k++)
				{
					contact_entry_t* entry = contact_list_entry_get_nth(contact, k);
					GtkTreeIter contactEntryIter;
					// Append the contact entry in the list
					gtk_tree_store_append(contactTreeStore, &contactEntryIter, &contactIter);
					gtk_tree_store_set(contactTreeStore, &contactEntryIter,
							CONTACT_WINDOW_TYPE, TYPE_ENTRY,
							CONTACT_WINDOW_ID, entry->_entryID,					// The contact string is also used as a unique ID
							CONTACT_WINDOW_CALL_CONSOLE_ACTIVE, entry->_isShownInConsole,
							CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT, FALSE,	// Never inconsistent because at bottom level of tree
							CONTACT_WINDOW_ICON, gdk_pixbuf_new_from_file(CONTACT_WINDOW_ENTRY_ICON, NULL),
							CONTACT_WINDOW_TEXT, entry->_text,
							-1);
				}
			}
			// Expand accounts nodes by default
			GtkTreePath* path = gtk_tree_model_get_path(GTK_TREE_MODEL(contactTreeStore), &accountIter);
			gtk_tree_view_expand_to_path(GTK_TREE_VIEW(contactTreeView), path);
			gtk_tree_path_free(path);
		}
	}
}

/**
 * Clear the model
 */
void
contact_window_clear_contact_list()
{
	gtk_tree_store_clear(contactTreeStore);
	contactTreeStore = NULL;
}

static gboolean button_press_event(GtkWidget* treeView, GdkEventButton* event, GtkWidget* nothing)
{
	GtkTreeSelection* selection;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeIter iter;
	gint type;

	if((event->button == 3) && (event->type == GDK_BUTTON_PRESS))
	{
		// Get the path of current selected
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
		if(gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeView), event->x, event->y, &path, NULL, NULL, NULL))
		{
			if(gtk_tree_model_get_iter(model, &iter, path))
			{
				// Get the type of the row to know which popup menu to activate
				gtk_tree_model_get(model, &iter, CONTACT_WINDOW_TYPE, &type, -1);
				gtk_tree_path_free(path);

				if(type == TYPE_ACCOUNT)
				{
					gtk_menu_popup(GTK_MENU(accountMenu), NULL, NULL, NULL, NULL, event->button, event->time);
				}
				if(type == TYPE_CONTACT)
				{
					gtk_menu_popup(GTK_MENU(contactMenu), NULL, NULL, NULL, NULL, event->button, event->time);
				}
				if(type == TYPE_ENTRY)
				{
					gtk_menu_popup(GTK_MENU(entryMenu), NULL, NULL, NULL, NULL, event->button, event->time);
				}
				// Do not return as we also want selection of row
			}
		}
	}
	return FALSE;
}

static void new_contact_activated(GtkMenuItem* item, GtkTreeView* treeRow)
{
	// TODO
	g_print("New contact activated");
}

static void edit_contact_activated(GtkMenuItem* item, GtkTreeView* treeRow)
{
	// TODO
	g_print("Edit contact activated");
}

static void new_entry_activated(GtkMenuItem* item, GtkTreeView* treeRow)
{
	// TODO
	g_print("New entry activated");
}

static void edit_entry_activated(GtkMenuItem* item, GtkTreeView* treeRow)
{
	// TODO
	g_print("Edit entry activated");
}

/**
 * 
 */
static void
contact_window_create_popup_menus()
{
	// Create menu items
	GtkWidget* newContactMenuItem = gtk_menu_item_new_with_label("New contact");
	g_signal_connect(G_OBJECT(newContactMenuItem), "activate", G_CALLBACK(new_contact_activated), contactTreeView);
	
	GtkWidget* editContactMenuItem = gtk_menu_item_new_with_label("Edit contact");
	g_signal_connect(G_OBJECT(editContactMenuItem), "activate", G_CALLBACK(edit_contact_activated), contactTreeView);

	GtkWidget* newEntryMenuItem = gtk_menu_item_new_with_label("New entry");
	g_signal_connect(G_OBJECT(newEntryMenuItem), "activate", G_CALLBACK(new_entry_activated), contactTreeView);

	GtkWidget* editEntryMenuItem = gtk_menu_item_new_with_label("Edit entry");
	g_signal_connect(G_OBJECT(editEntryMenuItem), "activate", G_CALLBACK(edit_entry_activated), contactTreeView);

	// Create different menus and append items
	accountMenu = gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(accountMenu), newContactMenuItem);
	gtk_menu_attach_to_widget(GTK_MENU(accountMenu), contactTreeView, NULL);
	gtk_widget_show_all(accountMenu);

	contactMenu = gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(contactMenu), editContactMenuItem);
	gtk_menu_shell_append(GTK_MENU_SHELL(contactMenu), newEntryMenuItem);
	gtk_menu_attach_to_widget(GTK_MENU(contactMenu), contactTreeView, NULL);
	gtk_widget_show_all(contactMenu);

	entryMenu = gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), editEntryMenuItem);
	gtk_menu_attach_to_widget(GTK_MENU(entryMenu), contactTreeView, NULL);
	gtk_widget_show_all(entryMenu);

	g_signal_connect(G_OBJECT(contactTreeView), "button-press-event", G_CALLBACK(button_press_event), NULL); //accountMenu);
}

/**
 * Show contact window
 */
void
show_contact_window()
{
	GtkDialog* dialog;
	GtkWidget* scrolledWindow;
	GtkCellRenderer* renderer;
	GtkTreeViewColumn* treeViewColumn;

	// Create dialog and set properties
	dialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Contacts",
				GTK_WINDOW(get_main_window()),
				GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_ACCEPT,
				NULL));
	gtk_dialog_set_has_separator(dialog, FALSE);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 400);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 0);

	// Put contacts in a scrollable window
	scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), scrolledWindow, TRUE, TRUE, 0);
	gtk_widget_show(scrolledWindow);
	
	// Create tree store with contact entries regrouped by accounts and contacts
	contactTreeStore = gtk_tree_store_new(COUNT_CONTACT_WINDOW,
			G_TYPE_INT,		// Type of the current row from define (TYPE_ACCOUNT, TYPE_CONTACT, TYPE_ENTRY)
			G_TYPE_STRING,	// ID of the account, contact or entry not shown in tree view
			G_TYPE_BOOLEAN,	// Shown in call console active property
			G_TYPE_BOOLEAN,	// Shown in call console inconsistent property
			GDK_TYPE_PIXBUF,// Icon
			G_TYPE_STRING	// Text
			);
	
	contactTreeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(contactTreeStore));
	
	// Create the only tree view column that will have three renderers
	treeViewColumn = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(treeViewColumn, "Contacts");
	
	// Is shown in call console column renderer active and inconsistent
	renderer = gtk_cell_renderer_toggle_new();
	gtk_tree_view_column_pack_start(treeViewColumn, renderer, FALSE);
	gtk_tree_view_column_set_attributes(treeViewColumn, renderer,
			"inconsistent", CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT,
			"active", CONTACT_WINDOW_CALL_CONSOLE_ACTIVE,
			NULL);
	
	// Icon renderer
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(treeViewColumn, renderer, FALSE);
	gtk_tree_view_column_set_attributes(treeViewColumn, renderer, "pixbuf", CONTACT_WINDOW_ICON, NULL);
	
	// Name renderer
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(treeViewColumn, renderer, TRUE);
	gtk_tree_view_column_set_attributes(treeViewColumn, renderer, "text", CONTACT_WINDOW_TEXT, NULL);
		
	// Append column in tree view
	gtk_tree_view_append_column(GTK_TREE_VIEW(contactTreeView), treeViewColumn);
	
	// Create popup menus for account, contact and entry rows
	contact_window_create_popup_menus();
	
	// Add view in scrolled window and show
	gtk_container_add(GTK_CONTAINER(scrolledWindow), contactTreeView);
	gtk_container_set_border_width(GTK_CONTAINER(contactTreeView), 10);
	gtk_widget_show(contactTreeView);
	
	// Fill tree model
	contact_window_fill_contact_list();

	gtk_dialog_run(dialog);
	
	contact_window_clear_contact_list();
	
	gtk_widget_destroy(GTK_WIDGET(dialog));
}
