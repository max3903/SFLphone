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
 *  GNU General Public License for more details.sfl	
 *                                                                              
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <glib/gprintf.h>

#include "dbus.h"
#include "mainwindow.h"
#include "sflphone_const.h"
#include "mail.h"
#include "voicemailwindow.h"


/** Local variables */
GtkWidget *VMWindow = NULL;

GtkWidget *textview    = NULL;
GtkWidget *btnPlayStop = NULL;
gchar     *currentPlay = ""; // The path of the voicemail which is currently playing


/** Enum for Treeview */
enum {
	VMV_IMG_COLUMN,
	VMV_TEXT_COLUMN,
	VMV_DATA_COLUMN,
	VMV_N_COLUMN
};


/**
 * Gets the selected row from the treeview
 * @return GtkTreeIter the iter corresponding to the currently selected tree node
 */
GtkTreeIter
getSelectedItem(void)
{
	GtkTreeSelection *selection;
	GtkTreeIter      iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(voicemailInbox->treeview));
	gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), NULL, &iter);
	return iter;
}


/**
 * Gets the name of the parent of the selected item
 * @param iter the selected treeview item
 * @return gchar* the name of the parent (folder) of the selected node (voicemail)
 */
gchar *
getSelectedItemParentName(GtkTreeIter iter)
{
	gchar        *folder;
	GtkTreeIter  parent;
	GValue       val = { 0 , };
	
	/** Sets position to the selected voicemail's parent */
	gtk_tree_model_iter_parent(GTK_TREE_MODEL(voicemailInbox->treestore), &parent, &iter);
	/** Gets data from parent and extract his name */
	gtk_tree_model_get_value(GTK_TREE_MODEL(voicemailInbox->treestore), &parent, VMV_DATA_COLUMN, &val);
	folder = (gchar *)g_value_get_pointer(&val);
	g_value_unset(&val);
	return folder;
}


/**
 * Checks whether any item of the treeview is selected
 * @return gboolean true if an item is selected (node or leaf), false otherwise
 */
gboolean
isItemSelected(void)
{
	GtkTreeSelection *selection;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(voicemailInbox->treeview));
	return gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), NULL, NULL);
}


/**
 * Checks whether the current selected item is a voicemail and not a folder
 * @return gboolean true if the selected item is a leaf, false otherwise
 */
gboolean
isAValidItem(void)
{
	GtkTreeIter  iter;
	gchar        *path;
	gboolean     isValid;
	
	iter = getSelectedItem();
	path = gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(voicemailInbox->treestore), &iter);
	if( strstr(path, ":") == NULL )
		isValid = FALSE;
	else
		isValid = TRUE;
	g_free(path);
	return isValid;
}


/*********************************************************************************************************/
// SIGNALS

/**
 * Stops currently playing voicemail (if any)
 */
static void
on_stop(void)
{
	if( strcmp(currentPlay, "") != 0 )
	{
		g_print("Player -- Stop\n");
		dbus_stop_voicemail();
	}
}


/**
 * Plays selected voicemail (if any)
 */
static void
on_play(void)
{
	if( isItemSelected() )
	{
		GtkTreeIter iter = getSelectedItem();
		/** A voicemail is selected */
		if( isAValidItem() )
		{
			/** If not the same voicemail to play */
			if( strcmp(currentPlay, gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(voicemailInbox->treestore), &iter)) != 0 )
			{
				mail_t *mail;
				GValue val = { 0 , };
				/** Stops playing last voicemail, if any */
				if( strcmp(currentPlay, "") != 0 )
				{
					on_stop();
				}
				gtk_tree_model_get_value(GTK_TREE_MODEL(voicemailInbox->treestore), &iter, VMV_DATA_COLUMN, &val);
				/** Gets the value of the selected voicemail */
				mail = (mail_t *)g_value_get_pointer(&val);
				g_value_unset(&val);
				/** Really plays the voicemail */
				dbus_play_voicemail(mail->folder, mail->name);
			}
			else /** It's the same, so stop it */
			{
				on_stop();
			}
		}
		/** A folder is selected */
		else
		{
			/** If folder has child(ren) */
			if( gtk_tree_model_iter_has_child(GTK_TREE_MODEL(voicemailInbox->treestore), &iter) )
			{
				/** If collapsed, expand */
				if( ! gtk_tree_view_row_expanded(GTK_TREE_VIEW(voicemailInbox->treeview), gtk_tree_model_get_path(GTK_TREE_MODEL(voicemailInbox->treestore), &iter)) )
					gtk_tree_view_expand_row(GTK_TREE_VIEW(voicemailInbox->treeview), gtk_tree_model_get_path(GTK_TREE_MODEL(voicemailInbox->treestore), &iter), TRUE);
				else /** Collapse */
					gtk_tree_view_collapse_row(GTK_TREE_VIEW(voicemailInbox->treeview), gtk_tree_model_get_path(GTK_TREE_MODEL(voicemailInbox->treestore), &iter));
			}
		}
	}
}


/**
 * Deletes the selected voicemail and asks for sure
 */
static void
on_delete(void)
{
	if( isItemSelected() )
	{
		GtkWidget   *dialog;
		gchar       *message;

		/** It's a voicemail */
		if( isAValidItem() )
			message = "Do you really want to delete this voicemail ?";
		else /** It's a folder */
			message = "Do you really want to delete this folder ?\nAll voicemails will be definitely erased";
		/** Dialog box opening */
		dialog = gtk_message_dialog_new( GTK_WINDOW(VMWindow),
										 GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_WARNING,
										 GTK_BUTTONS_OK_CANCEL,
										 _(message) );
		gtk_widget_show_all(dialog);
		/** Dialog box with question and user's answer */
		if( gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK )
		{
			GtkTreeIter iter;
			mail_t *mail;
			GValue val = { 0 , };

			iter = getSelectedItem();
			gtk_tree_model_get_value(GTK_TREE_MODEL(voicemailInbox->treestore), &iter, VMV_DATA_COLUMN, &val);
			/** Gets the value of the selected voicemail */
			mail = (mail_t *)g_value_get_pointer(&val);
			g_value_unset(&val);
			dbus_remove_voicemail(mail->folder, mail->name);
			gtk_tree_store_remove(voicemailInbox->treestore, &iter);
			currentPlay = "";
		}
		/** Dialog box destruction */
		gtk_widget_destroy(dialog);
	}
}

/**
 * Gets a voicemail's informations to fill details textfield
 */
static void
on_cursor_changed(void)
{
	if( isItemSelected() )
	{
		GtkTextBuffer *buf;
		GtkTextIter   iterStart;
		GtkTextIter   iterEnd;
	
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
		/* Deletes old data from the text buffer */
		gtk_text_buffer_get_start_iter(buf, &iterStart);
		gtk_text_buffer_get_end_iter(buf, &iterEnd);
		gtk_text_buffer_delete(buf, &iterStart, &iterEnd);

		/** It's a voicemail */
		if( isAValidItem() )
		{
			GtkTreeIter iter;
			mail_t      *mail;
			GValue      val = { 0 , };
			
			iter  = getSelectedItem();
			/** Gets the selected voicemail and its infos */
			gtk_tree_model_get_value(GTK_TREE_MODEL(voicemailInbox->treestore), &iter, VMV_DATA_COLUMN, &val);
			mail = (mail_t *)g_value_get_pointer(&val);
			g_value_unset(&val);
			/** Fills details box with the voicemail infos */
			gtk_text_buffer_insert(buf, &iterStart, mail->all, -1);
		}
	}
}


/**
 * Signal emitted when voicemail viewer is closing
 */
static void
on_delete_event(void)
{
	gtk_widget_destroy(VMWindow);
	VMWindow = NULL;
}


/**
 * Receiving signal from server that listening to voicemail is started
 */
void
voicemail_is_playing(void)
{
	/** If the voicemail is not currently playing, change the pixbuf */
	if( strcmp(currentPlay, "") == 0 )
	{
		GtkTreeIter iter;
		GdkPixbuf   *pixbuf;
		GtkWidget   *img;
		
		g_print("Receiving voicemail_is_playing\n");
		iter = getSelectedItem();
		/** Sets new image */
		pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/stop.svg" , NULL/*error*/);
		/** Updates selected row */
		gtk_tree_store_set(voicemailInbox->treestore, &iter, VMV_IMG_COLUMN, pixbuf, -1);
		/** Updates voicemail currently playing */
		currentPlay = gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(voicemailInbox->treestore), &iter);
		/** Modify the play/stop button image */
		img = gtk_image_new_from_stock("gtk-media-stop", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(btnPlayStop), img);
	}
}


/**
 * Receiving signal from server that listening to voicemail is stopped 
 */
void
voicemail_is_stopped(void)
{
	GtkTreeIter iter;
	GdkPixbuf   *pixbuf;
	GtkWidget   *img;
	
	g_print("Receiving voicemail_is_stopped\n");
	iter = getSelectedItem();
	/** Gets nth row to stop */
	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(voicemailInbox->treestore), &iter, currentPlay);
	/** Sets new image */
	pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/play.svg", NULL/*error*/);
	/** Updates selected row, just modify the image */
	gtk_tree_store_set(voicemailInbox->treestore, &iter, VMV_IMG_COLUMN, pixbuf, -1);
	/** Updates to none voicemail playing */
	currentPlay = "";
	/** Change the play/stop button image */
	img = gtk_image_new_from_stock("gtk-media-play", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(btnPlayStop), img);
}


/**
 * Receiving signal from server in order to show a warning/error message
 * @param gchar* the message to be displayed
 */
void
voicemail_catch_error(gchar *err)
{
	g_print("Receiving voicemail_catch_error\n");
	main_window_error_message(err);
}


/*********************************************************************************************************/
// POPUP

/**
 * Displays a popup menu when treeview is right-clicked
 * @param GtkWidget* the treeview (auto filled from right click signal)
 * @param GdkEventButton* the mouse click event
 */
static void
show_voicemail_window_popup_menu(GtkWidget *widget, GdkEventButton *event)
{
	GtkWidget *menu;
	GtkWidget *menu_item;
	int       button;
	int       event_time;
	
	menu = gtk_menu_new();
	g_signal_connect(menu, "deactivate", G_CALLBACK(gtk_widget_destroy), NULL);

	/** Dsiplays a play/stop button if the selected item is a voicemail */
	if( isAValidItem() )
	{
		/** If a voicemail is not playing, displays a "play" button */	
		if( strcmp(currentPlay, "") == 0 )
		{
			menu_item = gtk_image_menu_item_new_from_stock("gtk-media-play", NULL);
			g_signal_connect(GTK_MENU_ITEM(menu_item), "button_press_event", G_CALLBACK(on_play), NULL);
		}
		else /** Else, displays a "stop" button" */
		{
			menu_item = gtk_image_menu_item_new_from_stock("gtk-media-stop", NULL);
			g_signal_connect(GTK_MENU_ITEM(menu_item), "button_press_event", G_CALLBACK(on_stop), NULL);
		}
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	}
	menu_item = gtk_image_menu_item_new_from_stock("gtk-delete", NULL);
	g_signal_connect(GTK_MENU_ITEM(menu_item), "button_press_event", G_CALLBACK(on_delete), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

	/** If the event comes from the mouse */
	if( event )
	{
		button = event->button;
		event_time = event->time;
	}
	else /** Else, fills with rigth params */
	{
		button = 0;
		event_time = gtk_get_current_event_time();
	}
	gtk_menu_attach_to_widget(GTK_MENU(menu), widget, NULL);
	gtk_widget_show_all(menu);
	gtk_menu_popup( GTK_MENU(menu),
					NULL/*parent_menu_shell*/,
					NULL/*parent_menu_item*/,
					NULL/*function*/,
					NULL/*data*/, 
					button/*mouse_button*/,
					event_time/*time*/ );
}


/**
 * Event handler to show the voicemail popup menu
 * @param GtkWidget* the treeview
 * @param GdkEventButton* the click event (from mouse or keyboard)
 * @return gboolean true to stop other handlers from being invoked for the event, false to propagate the event further
 */
static gboolean
popup_button_press(GtkWidget *widget, GdkEventButton *event)
{
	/* Ignore double-clicks and triple-clicks */
	if( event->button == 3 && event->type == GDK_BUTTON_PRESS && isItemSelected() )
	{
		show_voicemail_window_popup_menu(widget, event);
		return TRUE;
	}
	return FALSE;
}


/*********************************************************************************************************/
// TREEVIEW CREATION / UPDATE

/**
 * Creates the model to display the treeview
 */
void
create_tree(void)
{
	GtkTreeViewColumn * column;
	GtkCellRenderer   * cellRenderer;
	
	/** Model creation */
	voicemailInbox->treestore = gtk_tree_store_new(VMV_N_COLUMN, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER);
	/** View creation */
	gtk_tree_view_set_model(GTK_TREE_VIEW(voicemailInbox->treeview), GTK_TREE_MODEL(voicemailInbox->treestore));

	/** First column creation */
	cellRenderer = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes("I", cellRenderer, "pixbuf", VMV_IMG_COLUMN, NULL);
	/** Adding column to view */
	gtk_tree_view_append_column(GTK_TREE_VIEW(voicemailInbox->treeview), column);

	/** Second column creation */
	cellRenderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("L", cellRenderer, "markup", VMV_TEXT_COLUMN, NULL);
	/** Adding column to view */
	gtk_tree_view_append_column(GTK_TREE_VIEW(voicemailInbox->treeview), column);
	
	/** Adding right-click signal for "popup" menu */
	g_signal_connect(voicemailInbox->treeview, "button-press-event", G_CALLBACK(popup_button_press), NULL);
}


/**
 * Updates treeview by adding a new element
 * @param gchar* the 'markuped' name of the folder (example : '<b>INBOX</b>')
 * @param gchar* the single name of the folder     (example : 'INBOX')
 */
void
update_tree(gchar * markup_text, gchar * text)
{
	GtkTreeIter iter;
	GdkPixbuf   *pixbuf;
	
	//pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/folder.svg", NULL/*error*/ );
	pixbuf = gdk_pixbuf_new_from_file_at_scale(ICONS_DIR "/folder.svg", 20/*width*/, -1/*height*/, TRUE/*preserve_ratio*/, NULL/*error*/);
	/* New line creation */
	gtk_tree_store_append(voicemailInbox->treestore, &iter, NULL);
	/* Data updates */
 	gtk_tree_store_set( voicemailInbox->treestore, &iter,
 						VMV_IMG_COLUMN           , pixbuf,
 						VMV_TEXT_COLUMN          , _(markup_text),
 						VMV_DATA_COLUMN          , text,
 						-1 );
}


/**
 * Updates treeview by adding childs (voicemails) to folders
 * @param gchar** the list of all the voicemail's name
 * @param int the number of the folder to fill voicemails in (example : Inbox = 0, Old = 1...)
 */
void
update_tree_complete(gchar **voicemails, int numline)
{
	GtkTreeIter iterParent;
	GtkTreeIter iterChild;
	GdkPixbuf   *pixBuf;
	gchar       line;
	gchar       **list;
	
	/** Gets the nth line to append voicemails */
	g_sprintf(&line, "%i", numline);
	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(voicemailInbox->treestore), &iterParent, &line);
	pixBuf = gdk_pixbuf_new_from_file(ICONS_DIR "/play.svg", NULL/*error*/);
	
	for( list = voicemails ; *voicemails ; voicemails++ ) {
		GHashTable *infos;
		mail_t     *ret;
		gchar      *text;
		
		/* New child line creation */
		gtk_tree_store_append(voicemailInbox->treestore, &iterChild, &iterParent);
		
		ret = g_new(mail_t, 1);
		infos = (GHashTable *)dbus_get_voicemail(getSelectedItemParentName(iterChild), *voicemails);
		if( infos == NULL ) 
			break;
		ret->name      = g_hash_table_lookup(infos, "Name");
		ret->folder    = g_hash_table_lookup(infos, "Folder");
		ret->from      = g_hash_table_lookup(infos, "From");
		ret->date      = g_hash_table_lookup(infos, "Date");
		ret->all       = g_hash_table_lookup(infos, "All");
		ret->isPlaying = FALSE;

		text = g_strdup_printf("From <b>%s</b>\non <i>%s</i>", ret->from, ret->date);

		/** Updating datas */
		gtk_tree_store_set( voicemailInbox->treestore, &iterChild,
							VMV_IMG_COLUMN            , pixBuf,
							VMV_TEXT_COLUMN           , _(text),
							VMV_DATA_COLUMN           , ret,
							-1);
		g_free(text);
	}
}


/*********************************************************************************************************/

/**
 * Voice Mail Window creation
 */
void
create_voicemail_window(void)
{
	GtkWidget   * vbox;
	GtkWidget   * label;
	GtkWidget   * scrolledwindow;
	GtkWidget   * play_img;
	GtkWidget   * del_btn;
	GtkWidget   * del_img;
	GtkWidget   * hseparator;
	GtkWidget   * hbox;
	GtkWidget   * hscale;
	GtkWidget   * expander;
	GtkTooltips * tooltips;
	
	tooltips = gtk_tooltips_new();
	
	/** Window */
	VMWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(VMWindow), _("Voicemail viewer"));
	gtk_window_set_position(GTK_WINDOW(VMWindow), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(VMWindow), 500, 400);
	gtk_window_set_default_icon_from_file(ICONS_DIR "/sflphone.png", NULL);
	GTK_WIDGET_SET_FLAGS(VMWindow, GTK_CAN_FOCUS);
	
	/** Vbox */
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(VMWindow), vbox);
	
	/** Label */
	label = gtk_label_new("");
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	
	/** Scroll area with Tree view */
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_SHADOW_IN);
	
	/** Treeview */
	voicemailInbox->treeview = gtk_tree_view_new();
	create_tree();
	gtk_widget_show(voicemailInbox->treeview);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(voicemailInbox->treeview), -45);
	// Don't show headers
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(voicemailInbox->treeview), FALSE);
	// Adds a color to lines
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(voicemailInbox->treeview), TRUE);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), voicemailInbox->treeview);
	// Adds double-click signal
	g_signal_connect(GTK_TREE_VIEW(voicemailInbox->treeview), "row-activated", G_CALLBACK(on_play), NULL);
	g_signal_connect(GTK_TREE_VIEW(voicemailInbox->treeview), "cursor-changed", G_CALLBACK(on_cursor_changed), NULL);
	
	/** Expander */
	expander = gtk_expander_new(_("Details"));
	gtk_box_pack_start(GTK_BOX(vbox), expander, FALSE, FALSE, 0);
	
	/** Text View */
	textview = gtk_text_view_new();
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), "", -1);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
	gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(textview), GTK_TEXT_WINDOW_RIGHT, 5);
	gtk_container_add(GTK_CONTAINER(expander), textview);
	
	/** Separator */
	hseparator = gtk_hseparator_new();
	gtk_widget_show(hseparator);
	gtk_box_pack_start(GTK_BOX(vbox), hseparator, FALSE, FALSE, 10);
	gtk_widget_set_sensitive(GTK_WIDGET(hseparator), FALSE);
	
	/** Hbox */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);
	
	/** Play Button */
	btnPlayStop = gtk_button_new ();
	gtk_widget_show(btnPlayStop);
	gtk_box_pack_start(GTK_BOX(hbox), btnPlayStop, FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, btnPlayStop, _("Play selected voicemail"), NULL);
	g_signal_connect(GTK_BUTTON(btnPlayStop), "clicked", G_CALLBACK(on_play), NULL);
	/** Play image button */
	play_img = gtk_image_new_from_stock("gtk-media-play", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(play_img);
	gtk_button_set_image(GTK_BUTTON(btnPlayStop), play_img);
	
	/** Hscale for player time */
	hscale = gtk_hscale_new(GTK_ADJUSTMENT( gtk_adjustment_new( 1 /* min */,
																100 /* max */,
																5 /* current */,
																1 /* minor increment */,
																2 /* major increment */,
																10 /* page size */) ));
	gtk_widget_show(hscale);
	gtk_box_pack_start(GTK_BOX(hbox), hscale, TRUE, TRUE, 0);
	gtk_scale_set_draw_value(GTK_SCALE(hscale), FALSE);
	gtk_scale_set_value_pos(GTK_SCALE(hscale), GTK_POS_BOTTOM);
	gtk_scale_set_digits(GTK_SCALE(hscale), 0);
	
	/** Delete Button */
	del_btn = gtk_button_new();
	gtk_widget_show(del_btn);
	gtk_box_pack_start(GTK_BOX(hbox), del_btn, FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, del_btn, _("Delete selected voicemail"), NULL);
	g_signal_connect(GTK_BUTTON(del_btn), "clicked", G_CALLBACK(on_delete), NULL);
	/** Delete image button */
	del_img = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(del_img);
	gtk_container_add(GTK_CONTAINER(del_btn), del_img);

	g_signal_connect(GTK_OBJECT(VMWindow), "delete-event", G_CALLBACK(on_delete_event), NULL );
	gtk_widget_show_all(VMWindow);
}


/**
 * Gets the voicemail window
 * @return GtkWidget* the voicemail viewer window
 */
GtkWidget *
getVoicemailWindow(void)
{
	return VMWindow;
}


/**
 * Shows Voice Mail Window
 */
void
show_voicemail_window(void)
{
	if( dbus_open_connection() == TRUE )
	{
		if( VMWindow == NULL )
		{
			create_voicemail_window();
		}
		else
		{
			mail_list_clear_all(voicemailInbox);
		}

		gchar **list = (gchar**)dbus_get_list_folders();
		gchar **ls;
		gint  i=0;
		for( ls = list ; *list ; list++ ) {
			gchar *folder = *list;
			gchar **tab   = g_strsplit(folder, "|", 2);
			update_tree(tab[0], tab[1]);
			gint count = dbus_get_folder_count(tab[1]);
			if( count != 0 )
			{
				gchar ** voicemails = (gchar **)dbus_get_list_mails(tab[1]);
				update_tree_complete(voicemails, i);
			}
			i++;
			g_free(folder);
		}
	} else {
		main_window_error_message(_("Voicemail daemon is not accessible.\n\nPlease check :\n- SFLphone daemon alive\n- Voicemail server reachable."));
	}
}

