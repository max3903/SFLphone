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

#include "contactwindow.h"

#include <accountlist.h>
#include <actions.h>
#include <contactlist.h>
#include <config.h>
#include <mainwindow.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib/gprintf.h>

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

// Defined types that tell if a row is an account, a contact or an entry
#define TYPE_ACCOUNT	0
#define TYPE_CONTACT	1
#define TYPE_ENTRY		2

// Main dialog
GtkDialog* contactWindowDialog = NULL;

// View and model of tree
GtkWidget* contactTreeView;
GtkTreeStore* contactTreeStore;

// Popup menus for different types of row
GtkWidget* accountMenu;
GtkWidget* contactMenu;
GtkWidget* entryMenu;

// Secondary dialogs for contacts and entries
GtkDialog* contactDialog;
GtkDialog* entryDialog;

// Widgets for the contact dialog
GtkWidget* contactIDEntry;
GtkWidget* contactFirstNameEntry;
GtkWidget* contactLastNameEntry;
GtkWidget* contactEmailEntry;

// Widgets for the entry dialog
GtkWidget* entryIDEntry;
GtkWidget* entryTextEntry;
GtkWidget* entryTypeEntry;
GtkWidget* entryIsShownCheckButton;
GtkWidget* entryIsSubcribedCheckButton;

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
		account_t* account = account_list_get_nth(i);
		if(strcmp((gchar*)g_hash_table_lookup(account->properties, ACCOUNT_ENABLED), "TRUE") == 0)
		{
			GtkTreeIter accountIter;
			// Append the account in the list
			gtk_tree_store_append(contactTreeStore, &accountIter, NULL);
			gtk_tree_store_set(contactTreeStore, &accountIter,
					CONTACT_WINDOW_TYPE, TYPE_ACCOUNT,
					CONTACT_WINDOW_ID, account->accountID,
					CONTACT_WINDOW_CALL_CONSOLE_ACTIVE, TRUE,
					CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT, TRUE,		// TODO Should be a function to pass all contacts if all selected or not
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
						CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT, TRUE,		// TODO Should be a function to pass all contacts if all selected or not
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

static gboolean
button_press_event(GtkWidget* treeView, GdkEventButton* event, GtkWidget* nothing)
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
				switch(type)
				{
				case TYPE_ACCOUNT:
					gtk_menu_popup(GTK_MENU(accountMenu), NULL, NULL, NULL, NULL, event->button, event->time);
					break;
				case TYPE_CONTACT:
					gtk_menu_popup(GTK_MENU(contactMenu), NULL, NULL, NULL, NULL, event->button, event->time);
					break;
				case TYPE_ENTRY:
					gtk_menu_popup(GTK_MENU(entryMenu), NULL, NULL, NULL, NULL, event->button, event->time);
					break;
				default:
					g_printerr("Row type unknown");
					break;
				}
				// Do not return true as we also want selection of row
			}
		}
		gtk_tree_path_free(path);
	}
	return FALSE;
}

static void
new_contact_activated(GtkMenuItem* item, GtkTreeView* treeView)
{
	GtkTreeIter iter;
	gchar* accountID = NULL;
	
	// Find model and selection to get the account ID
	GtkTreeModel* model = gtk_tree_view_get_model(treeView);
	GtkTreeSelection* selection = gtk_tree_view_get_selection(treeView);
	gtk_tree_selection_get_selected(selection, &model, &iter);
	gtk_tree_model_get(model, &iter,
			CONTACT_WINDOW_ID, &accountID,
			-1);
	
	show_contact_dialog(accountID, NULL);
	g_free(accountID);
}

static void
edit_contact_activated(GtkMenuItem* item, GtkTreeView* treeView)
{
	GtkTreeIter iter;
	gchar* contactID = NULL;
	gchar* accountID = NULL;
	
	// Find model and selection to get the contact ID
	GtkTreeModel* model = gtk_tree_view_get_model(treeView);
	GtkTreeSelection* selection = gtk_tree_view_get_selection(treeView);
	gtk_tree_selection_get_selected(selection, &model, &iter);
	gtk_tree_model_get(model, &iter,
			CONTACT_WINDOW_ID, &contactID,
			-1);
	
	// Get path and then parent path
	GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_path_up(path);
	
	// Get the account ID of the parent iter
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter,
			CONTACT_WINDOW_ID, &accountID,
			-1);
		
	show_contact_dialog(accountID, contactID);
	
	gtk_tree_path_free(path);
	g_free(contactID);
	g_free(accountID);
}

static void
new_entry_activated(GtkMenuItem* item, GtkTreeView* treeView)
{
	GtkTreeIter iter;
	gchar* contactID;
	gchar* accountID;
	
	// Find model and selection to get the contact ID
	GtkTreeModel* model = gtk_tree_view_get_model(treeView);
	GtkTreeSelection* selection = gtk_tree_view_get_selection(treeView);
	gtk_tree_selection_get_selected(selection, &model, &iter);
	gtk_tree_model_get(model, &iter,
			CONTACT_WINDOW_ID, &contactID,
			-1);
	
	// Get path and then parent path
	GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_path_up(path);
	
	// Get the account ID of the parent iter
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter,
			CONTACT_WINDOW_ID, &accountID,
			-1);
		
	show_entry_dialog(accountID, contactID, NULL);
	
	gtk_tree_path_free(path);
	g_free(contactID);
	g_free(accountID);
}

static void
edit_entry_activated(GtkMenuItem* item, GtkTreeView* treeView)
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
	GtkWidget* newContactMenuItem = gtk_menu_item_new_with_label(_("New contact"));
	g_signal_connect(G_OBJECT(newContactMenuItem), "activate", G_CALLBACK(new_contact_activated), contactTreeView);
	
	GtkWidget* editContactMenuItem = gtk_menu_item_new_with_label(_("Edit contact"));
	g_signal_connect(G_OBJECT(editContactMenuItem), "activate", G_CALLBACK(edit_contact_activated), contactTreeView);

	GtkWidget* newEntryMenuItem = gtk_menu_item_new_with_label(_("New entry"));
	g_signal_connect(G_OBJECT(newEntryMenuItem), "activate", G_CALLBACK(new_entry_activated), contactTreeView);

	GtkWidget* editEntryMenuItem = gtk_menu_item_new_with_label(_("Edit entry"));
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
}

/**
 * Show contact window
 */
void
show_contact_window()
{
	GtkWidget* scrolledWindow;
	GtkCellRenderer* renderer;
	GtkTreeViewColumn* treeViewColumn;

	// Create dialog and set properties
	contactWindowDialog = GTK_DIALOG(gtk_dialog_new_with_buttons (_("Contacts"),
				GTK_WINDOW(get_main_window()),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_ACCEPT,
				NULL));
	gtk_dialog_set_has_separator(contactWindowDialog, FALSE);
	gtk_window_set_default_size(GTK_WINDOW(contactWindowDialog), 500, 600);
	gtk_container_set_border_width(GTK_CONTAINER(contactWindowDialog), 0);

	// Put contacts in a scrollable window
	scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(contactWindowDialog->vbox), scrolledWindow, TRUE, TRUE, 0);
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
	gtk_tree_view_column_set_title(treeViewColumn, _("Contacts"));
	
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
	
	// Fill tree model
	contact_window_fill_contact_list();
	
	// Signal sent when button pressed over the tree view
	g_signal_connect(G_OBJECT(contactTreeView), "button-press-event", G_CALLBACK(button_press_event), NULL);
	
	// Add view in scrolled window and show
	gtk_container_add(GTK_CONTAINER(scrolledWindow), contactTreeView);
	gtk_container_set_border_width(GTK_CONTAINER(contactTreeView), 10);
	gtk_widget_show(contactTreeView);
	
	gtk_dialog_run(contactWindowDialog);
	
	// Close all when dialog stops running
	contact_window_clear_contact_list();
	gtk_widget_destroy(GTK_WIDGET(contactWindowDialog));
	contactWindowDialog = NULL;
}

void
show_contact_dialog(gchar* accountID, gchar* contactID)
{
	gint response;
	gboolean isNewContact;
	
	gchar* contactIDText = NULL;
	gchar* firstNameText = "";
	gchar* lastNameText = "";
	gchar* emailText = "";

	GtkWidget* label;
	GtkWidget* table;

	// Load contact list for account
	GQueue* contactList = contact_hash_table_get_contact_list(accountID);
	if(contactList == NULL)
	{
		g_printerr("Contact list not found for %s\n", accountID);
	}
	else
	{
		// If it is a new contact, generate contact ID
		if(contactID == NULL)
		{
			isNewContact = TRUE;
			do
			{
				if(contactIDText != NULL) g_free(contactIDText);
				contactIDText = g_new0(gchar, 30);
				g_sprintf(contactIDText, "%d", rand());
			}
			while(contact_list_get(contactList, contactIDText) != NULL);
		}
		else
		{
			isNewContact = FALSE;
			// Try to find contact
			contact_t* contact = contact_list_get(contactList, contactID);
			if(contact != NULL)
			{
				// Load dialog with current contact values
				contactIDText = g_strdup(contact->_contactID);
				firstNameText = g_strdup(contact->_firstName);
				lastNameText = g_strdup(contact->_lastName);
				emailText = g_strdup(contact->_email);
			}
			else
			{
				g_printerr("Contact not found");
				return;
			}
		}

		// Create dialog with values
		contactDialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
				_("Contact settings"),
				GTK_WINDOW(contactWindowDialog),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_STOCK_SAVE,
				GTK_RESPONSE_ACCEPT,
				GTK_STOCK_CANCEL,
				GTK_RESPONSE_CANCEL,
				NULL));
		gtk_dialog_set_has_separator(contactDialog, TRUE);
		gtk_window_set_default_size(GTK_WINDOW(contactDialog), 500, -1);
		gtk_container_set_border_width(GTK_CONTAINER(contactDialog), 0);

		// Create table that will hold all entries
		table = gtk_table_new(3, 2, FALSE);
		gtk_table_set_row_spacings(GTK_TABLE(table), 10);
		gtk_table_set_col_spacings(GTK_TABLE(table), 10);

		// Add first name entry
		label = gtk_label_new_with_mnemonic(_("_First name"));
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		contactFirstNameEntry = gtk_entry_new();
		gtk_label_set_mnemonic_widget(GTK_LABEL(label), contactFirstNameEntry);
		gtk_entry_set_text(GTK_ENTRY(contactFirstNameEntry), firstNameText);
		gtk_table_attach(GTK_TABLE(table), contactFirstNameEntry, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		
		// Add last name entry
		label = gtk_label_new_with_mnemonic(_("_Last name"));
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		contactLastNameEntry = gtk_entry_new();
		gtk_label_set_mnemonic_widget(GTK_LABEL(label), contactLastNameEntry);
		gtk_entry_set_text(GTK_ENTRY(contactLastNameEntry), lastNameText);
		gtk_table_attach(GTK_TABLE(table), contactLastNameEntry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		
		// Add email entry
		label = gtk_label_new_with_mnemonic(_("_Email"));
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		contactEmailEntry = gtk_entry_new();
		gtk_label_set_mnemonic_widget(GTK_LABEL(label), contactEmailEntry);
		gtk_entry_set_text(GTK_ENTRY(contactEmailEntry), emailText);
		gtk_table_attach(GTK_TABLE(table), contactEmailEntry, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		
		// Pack and show table
		gtk_box_pack_start(GTK_BOX(contactDialog->vbox), table, TRUE, TRUE, 0);
		gtk_container_set_border_width (GTK_CONTAINER(table), 10);
		gtk_widget_show_all(table);
		
		// Run dialog, get ok or cancel response and destroy
		response = gtk_dialog_run(contactDialog);
		
		if(response == GTK_RESPONSE_ACCEPT)
		{
			// Build contact from text entries
			contact_t* contact = g_new0(contact_t, 1);
			contact->_contactID = contactIDText;
			contact->_firstName = g_strdup(gtk_entry_get_text(GTK_ENTRY(contactFirstNameEntry)));
			contact->_lastName = g_strdup(gtk_entry_get_text(GTK_ENTRY(contactLastNameEntry)));
			contact->_email = g_strdup(gtk_entry_get_text(GTK_ENTRY(contactEmailEntry)));
			
			// The model of the tree view is not modified directly but the contact list will
			// be updated and will propagate changes to the contact window and the call console.
			if(isNewContact)
				contact_list_add(accountID, contact, TRUE);
			else
				contact_list_edit(accountID, contact);
		}
		
		gtk_widget_destroy(GTK_WIDGET(contactDialog));
	}
}

void
show_entry_dialog(gchar* accountID, gchar* contactID, gchar* entryID)
{
	gint response;
	gboolean isNewEntry;
	
	gchar* entryIDText = "";
	gchar* textText = "";
	gchar* typeText = "";
	gboolean isShownCheck = FALSE;
	gboolean isSubscribedCheck = FALSE;

	GtkWidget* label;
	GtkWidget* table;

	// Load contact from account
	contact_t* contact = contact_list_get(contact_hash_table_get_contact_list(accountID), contactID);
	if(contact == NULL)
	{
		g_printerr("Contact not found for %s\n", accountID);
	}
	else
	{
		if(entryID == NULL)
		{
			isNewEntry = TRUE;
		}
		else
		{
			isNewEntry = FALSE;
			// Try to find entry
			contact_entry_t* entry = contact_list_entry_get(contact, entryID);
			if(entry != NULL)
			{
				// Load dialog with current contact values
				entryIDText = g_strdup(entry->_entryID);
				textText = g_strdup(entry->_text);
				typeText = g_strdup(entry->_type);
				isShownCheck = entry->_isShownInConsole;
				isSubscribedCheck = entry->_isSubscribed;
			}
			else
			{
				g_printerr("Entry not found");
				return;
			}
		}

		// Create dialog with values
		entryDialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
				_("Entry settings"),
				GTK_WINDOW(contactWindowDialog),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_STOCK_SAVE,
				GTK_RESPONSE_ACCEPT,
				GTK_STOCK_CANCEL,
				GTK_RESPONSE_CANCEL,
				NULL));
		gtk_dialog_set_has_separator(entryDialog, TRUE);
		gtk_window_set_default_size(GTK_WINDOW(entryDialog), 500, -1);
		gtk_container_set_border_width(GTK_CONTAINER(entryDialog), 0);

		// Create table that will hold all entries
		table = gtk_table_new(5, 2, FALSE);
		gtk_table_set_row_spacings(GTK_TABLE(table), 10);
		gtk_table_set_col_spacings(GTK_TABLE(table), 10);

		// Add  contact entry ID entry
		label = gtk_label_new_with_mnemonic(_("_Entry"));
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
		gtk_widget_set_tooltip_text(GTK_WIDGET(label), _("Contact number as you would type it when calling\nExample : 5142765468"));
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		entryIDEntry = gtk_entry_new();
		gtk_label_set_mnemonic_widget(GTK_LABEL(label), entryIDEntry);
		gtk_entry_set_text(GTK_ENTRY(entryIDEntry), entryIDText);
		gtk_table_attach(GTK_TABLE(table), entryIDEntry, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		
		// Add text entry
		label = gtk_label_new_with_mnemonic(_("_Alias"));
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
		gtk_widget_set_tooltip_text(GTK_WIDGET(label), _("Contact number alias used for presentation\nExample : 514-276-5468"));
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		entryTextEntry = gtk_entry_new();
		gtk_label_set_mnemonic_widget(GTK_LABEL(label), entryTextEntry);
		gtk_entry_set_text(GTK_ENTRY(entryTextEntry), textText);
		gtk_table_attach(GTK_TABLE(table), entryTextEntry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		
		// Add type entry
		label = gtk_label_new_with_mnemonic(_("_Type"));
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
		gtk_widget_set_tooltip_text(GTK_WIDGET(label), _("Type of contact (work, home, cell)"));
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		entryTypeEntry = gtk_entry_new();
		gtk_label_set_mnemonic_widget(GTK_LABEL(label), entryTypeEntry);
		gtk_entry_set_text(GTK_ENTRY(entryTypeEntry), typeText);
		gtk_table_attach(GTK_TABLE(table), entryTypeEntry, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		
		// Add is shown in call console check box
		label = gtk_label_new_with_mnemonic(_("_Show in call console"));
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
		gtk_widget_set_tooltip_text(GTK_WIDGET(label), _("Check if you want to see this entry in the call console"));
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		entryIsShownCheckButton = gtk_check_button_new();
		gtk_label_set_mnemonic_widget(GTK_LABEL(label), entryIsShownCheckButton);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entryIsShownCheckButton), isShownCheck);
		gtk_table_attach(GTK_TABLE(table), entryIsShownCheckButton, 1, 2, 3, 4, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		
		// Add is subscribed check box
		label = gtk_label_new_with_mnemonic(_("_Presence subscription"));
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
		gtk_widget_set_tooltip_text(GTK_WIDGET(label), _("Check if this entry should subscribe to presence information"));
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		entryIsSubcribedCheckButton = gtk_check_button_new();
		gtk_label_set_mnemonic_widget(GTK_LABEL(label), entryIsSubcribedCheckButton);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entryIsSubcribedCheckButton), isSubscribedCheck);
		gtk_table_attach(GTK_TABLE(table), entryIsSubcribedCheckButton, 1, 2, 4, 5, GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		
		// Pack and show table
		gtk_box_pack_start(GTK_BOX(entryDialog->vbox), table, TRUE, TRUE, 0);
		gtk_container_set_border_width (GTK_CONTAINER(table), 10);
		gtk_widget_show_all(table);
		
		// Run dialog, get ok or cancel response and destroy
		response = gtk_dialog_run(entryDialog);
		
		if(response == GTK_RESPONSE_ACCEPT)
		{
			// Build entry from entries
			contact_entry_t* entry = g_new0(contact_entry_t, 1);
			entry->_entryID = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryIDEntry)));
			entry->_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryTextEntry)));
			entry->_type = g_strdup(gtk_entry_get_text(GTK_ENTRY(entryTypeEntry)));
			entry->_isShownInConsole = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(entryIsShownCheckButton));
			entry->_isSubscribed = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(entryIsSubcribedCheckButton));
			
			// The model of the tree view is not modified directly but the entry list will
			// be updated and will propagate changes to the entry window and the call console.
			if(isNewEntry)
				contact_list_entry_add(accountID, contactID, entry, TRUE);
			else
				contact_list_entry_edit(accountID, contactID, entry);
			
			// TODO If entry ID changed we must do a remove and add
		}
		
		gtk_widget_destroy(GTK_WIDGET(entryDialog));
	}
}

void
contact_window_add_account()
{
	// TODO
}

void
contact_window_remove_account()
{
	// TODO
}

void
contact_window_add_contact(gchar* accountID, contact_t* contact)
{
	// Only if the contact window is shown
	if(contactWindowDialog == NULL) return;
	
	// Get the iteration corresponding to the account
	GtkTreeModel* model;
	GtkTreePath* path;
	GtkTreeIter iter;
	GtkTreeIter newIter;
	gchar* id;
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(contactTreeView));
	if(!gtk_tree_model_get_iter_first(model, &iter))
	{
		g_printerr("Error when adding contact in contact window");
		return;
	}
	do
	{
		// Get the ID of the current account iter
		gtk_tree_model_get(model, &iter,
				CONTACT_WINDOW_ID, &id,
				-1);
		if(id == NULL)
		{
			g_printerr("Error, null account row ID");
			return;
		}
		if(strcmp(id, accountID) == 0)
		{
			// Append the contact in the list
			gchar fullName[200];
			sprintf(fullName, "%s %s", contact->_firstName, contact->_lastName);
			gtk_tree_store_append(contactTreeStore, &newIter, &iter);
			gtk_tree_store_set(contactTreeStore, &newIter,
					CONTACT_WINDOW_TYPE, TYPE_CONTACT,
					CONTACT_WINDOW_ID, contact->_contactID,
					CONTACT_WINDOW_CALL_CONSOLE_ACTIVE, TRUE,
					CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT, TRUE,		// Should be a function to pass all contacts if all selected or not
					CONTACT_WINDOW_ICON, gdk_pixbuf_new_from_file(CONTACT_WINDOW_CONTACT_ICON, NULL),
					CONTACT_WINDOW_TEXT, fullName,
					-1);
			return;
		}
		
		// Go to next account until null
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_path_next(path);
		gtk_tree_model_get_iter(model, &iter, path);
	}
	while(path != NULL);
}

void
contact_window_edit_contact(gchar* accountID, contact_t* contact)
{
	// Only if the contact window is shown
	if(contactWindowDialog == NULL) return;
	
	// Get the iteration corresponding to the account
	GtkTreeModel* model;
	GtkTreePath* accountPath;
	GtkTreePath* contactPath;
	GtkTreeIter iter;
	gchar* id;
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(contactTreeView));
	if(!gtk_tree_model_get_iter_first(model, &iter)) return;
	do
	{
		// Get the ID of the current account iter
		gtk_tree_model_get(model, &iter,
				CONTACT_WINDOW_ID, &id,
				-1);
		if(id == NULL) return;
		if(strcmp(id, accountID) == 0)
		{
			// The account is found so go deeper
			contactPath = gtk_tree_model_get_path(model, &iter);
			gtk_tree_path_down(contactPath);
			// Try to find the account
			gtk_tree_model_get_iter(model, &iter, contactPath);
			do
			{
				// Get the ID of the current contact iter
				gtk_tree_model_get(model, &iter,
						CONTACT_WINDOW_ID, &id,
						-1);
				if(id == NULL) return;
				if(strcmp(id, contact->_contactID) == 0)
				{
					gchar fullName[1000];
					sprintf(fullName, "%s %s", contact->_firstName, contact->_lastName);
					gtk_tree_store_set(contactTreeStore, &iter,
							CONTACT_WINDOW_TYPE, TYPE_CONTACT,
							CONTACT_WINDOW_ID, contact->_contactID,
							CONTACT_WINDOW_CALL_CONSOLE_ACTIVE, TRUE,
							CONTACT_WINDOW_CALL_CONSOLE_INCONSISTENT, TRUE,		// Should be a function to pass all contacts if all selected or not
							CONTACT_WINDOW_ICON, gdk_pixbuf_new_from_file(CONTACT_WINDOW_CONTACT_ICON, NULL),
							CONTACT_WINDOW_TEXT, fullName,
							-1);
					return;
				}
				// Go to next contact until null
				contactPath = gtk_tree_model_get_path(model, &iter);
				gtk_tree_path_next(contactPath);
				gtk_tree_model_get_iter(model, &iter, contactPath);
			}
			while(contactPath != NULL);
		}
		// Go to next account until null
		accountPath = gtk_tree_model_get_path(model, &iter);
		gtk_tree_path_next(accountPath);
		gtk_tree_model_get_iter(model, &iter, accountPath);
	}
	while(accountPath != NULL);
}

void
contact_window_remove_contact(gchar* accountID, gchar* contactID)
{
	// TODO
	// Get the iteration corresponding to the account
	// Get the iteration of the contact
	// Remove iteration
}

void
contact_window_add_entry()
{
	// TODO
}

void
contact_window_edit_entry()
{
	// TODO
}

void
contact_window_remove_entry()
{
	// TODO
}
