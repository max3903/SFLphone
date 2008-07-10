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

#include "dbus.h"
#include "sflphone_const.h"
#include "voicemailwindow.h"


/** Local variables */
GtkWidget *VMWindow;

GtkWidget *treeview;
GtkWidget *textview;
gchar     *g_currently_playing = "";


/** Enum for Treeview */
enum {
    IMG_COLUMN,
    TEXT_COLUMN,
    N_COLUMN
};


/**
 * Gets the selected row from the treeview
 */
GtkTreeIter
getSelectedItem( void )
{
	GtkTreeSelection *selection;
	GtkTreeIter      iter;

	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW( treeview ) );
	gtk_tree_selection_get_selected( GTK_TREE_SELECTION( selection ) , NULL , &iter );
	return iter;
}


/**
 * Gets the name of the selected item
 * @param iter the selected treeview item
 */
gchar *
getSelectedItemName( GtkTreeIter iter )
{
	GtkTreeModel *model;
	gchar        *tmp;
	gchar        *name;

	model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
	gtk_tree_model_get( model , &iter, TEXT_COLUMN , &tmp , -1 );
	/** Gets only the name of the file, not the other informations */
	strncpy( name , tmp , 7 );
	name[7] = '\0';
	g_free( tmp );
	return name;
}


/**
 * Gets the name of the parent of the selected item
 * @param iter the selected treeview item
 */
gchar *
getSelectedItemParentName( GtkTreeIter iter )
{
	gchar        *folder;
	GtkTreeModel *model;
	GtkTreeIter  parent;
	
	model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
	/** Gets the selected voicemail's parent name */
	gtk_tree_model_iter_parent( GTK_TREE_MODEL( model ) , &parent , &iter );
	gtk_tree_model_get( GTK_TREE_MODEL( model ) , &parent , TEXT_COLUMN , &folder , -1 );
	return folder;
}


/**
 * Checks whether any item of the treeview is selected : TRUE, FALSE otherwise
 */
gboolean
isItemSelected( void )
{
	GtkTreeSelection *selection;
	GtkTreeModel     *model;

	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW( treeview ) );
	return gtk_tree_selection_get_selected( GTK_TREE_SELECTION( selection ) , &model , NULL );
}


/**
 * Checks whether the current selected item is a voicemail and not a folder
 */
gboolean
isAValidItem( void )
{
	GtkTreeModel *model;
	GtkTreeIter  iter;
	gchar        *path;
	gboolean     isValid;
	
	model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
	iter = getSelectedItem();
	path = gtk_tree_model_get_string_from_iter( GTK_TREE_MODEL( model ) , &iter );
	if( strstr( path , ":" ) == NULL )
		isValid = FALSE;
	else
		isValid = TRUE;
	g_free( path );
	return isValid;
}


/**
 * Stops currently playing voicemail (if any)
 */
static void
on_stop()
{
	if( strcmp( g_currently_playing , "" ) != 0 )
	{
		GtkTreeIter  iter;
		GtkTreeModel *model;
		GdkPixbuf    *pixbuf;
		gchar        *text;
		
		g_print( "Player -- Stop\n" );
		model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
		/** Gets nth row to stop */
		gtk_tree_model_get_iter_from_string( GTK_TREE_MODEL( model ) , &iter , g_currently_playing );
		/** Gets text row */
		gtk_tree_model_get( GTK_TREE_MODEL( model ) , &iter , IMG_COLUMN , &pixbuf , TEXT_COLUMN , &text , -1 );
		/** Sets new image */
		pixbuf = gdk_pixbuf_new_from_file_at_scale( ICONS_DIR "/play.svg" , 20/*width*/ , -1/*height*/ , TRUE/*preserve-ratio*/ , NULL/*error*/ );
		/** Updates selected row */
		gtk_tree_store_set( GTK_TREE_STORE( model ) , &iter , IMG_COLUMN , pixbuf , TEXT_COLUMN , text , -1 );
		/** Updates to none voicemail playing */
		g_currently_playing = "";
		/** Stops the current voicemail */
		dbus_stop_voicemail();
	}
}


static void
on_pause()
{
	if( isItemSelected() )
	{
		GtkTreeIter iter = getSelectedItem();
		g_print("Player -- Pause\n");
		
		//iter = getSelectedItem();
		//model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
		/** Gets text row */
		//gtk_tree_model_get( GTK_TREE_MODEL( model ), &iter, IMG_COLUMN, &pixbuf, TEXT_COLUMN, &text, -1 );
		/** Sets new image */
		//pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/play.svg", NULL);
		/** Updates selected row */
		//gtk_tree_store_set( GTK_TREE_STORE( model ), &iter, IMG_COLUMN, pixbuf, TEXT_COLUMN, text, -1 );*/
	}
}


/**
 * Plays selected voicemail (if any)
 */
static void
on_play()
{
	if( isItemSelected() )
	{
		/** Voicemail selected */
		if( isAValidItem() )
		{
			GtkTreeIter  iter;
			GtkTreeIter  parent;
			GtkTreeModel *model;
			GdkPixbuf    *pixbuf;
			gchar        *text;
			gchar        *folder;
			
			model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
			iter = getSelectedItem();
			/** If not the same voicemail to play */
			if( strcmp( g_currently_playing , gtk_tree_model_get_string_from_iter( GTK_TREE_MODEL( model ) , &iter ) ) != 0 )
			{
				/** First, stop playing last voicemail */
				if( strcmp( g_currently_playing , "" ) != 0 )
				{
					on_stop();
				}
				/** Second, play selected voicemail */
				iter = getSelectedItem();
				/** Gets text row */
				gtk_tree_model_get( GTK_TREE_MODEL( model ) , &iter , IMG_COLUMN , &pixbuf , TEXT_COLUMN , &text , -1 );
				/** Sets new image */
				pixbuf = gdk_pixbuf_new_from_file_at_scale( ICONS_DIR "/pause.svg" , 20/*width*/ , -1/*height*/ , TRUE/*preserve-ratio*/ , NULL/*error*/ );
				/** Updates selected row */
				gtk_tree_store_set( GTK_TREE_STORE( model ) , &iter , IMG_COLUMN , pixbuf , TEXT_COLUMN , text , -1 );
				/** Updates voicemail currently playing */
				g_currently_playing = gtk_tree_model_get_string_from_iter( GTK_TREE_MODEL( model ) , &iter );
				/** Gets the selected voicemail's parent name */
				gtk_tree_model_iter_parent( GTK_TREE_MODEL( model ) , &parent , &iter );
				gtk_tree_model_get( GTK_TREE_MODEL( model ) , &parent , TEXT_COLUMN , &folder , -1 );
				/** Really plays the voicemail */
				dbus_play_voicemail( getSelectedItemParentName( iter ) , getSelectedItemName( iter ) );
			}
			else
			{
				on_stop();
			}
		}
		/** Folder selected */
		else
		{
			GtkTreeIter  iter;
			GtkTreeModel *model;
			
			model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
			iter = getSelectedItem();
			/** If folder has child(ren) */
			if( gtk_tree_model_iter_has_child( GTK_TREE_MODEL( model ) , &iter ) )
			{
				/** If collapsed, expand */
				if( ! gtk_tree_view_row_expanded( GTK_TREE_VIEW( treeview ) , gtk_tree_model_get_path( GTK_TREE_MODEL( model ) , &iter ) ) )
				{
					gtk_tree_view_expand_row( GTK_TREE_VIEW( treeview ) , gtk_tree_model_get_path( GTK_TREE_MODEL( model ) , &iter ) , TRUE );
				}
				else /** Collapse */
				{
					gtk_tree_view_collapse_row( GTK_TREE_VIEW( treeview ) , gtk_tree_model_get_path( GTK_TREE_MODEL( model ) , &iter ) );
				}
			}
		}
	}
}


/**
 * Deletes the selected voicemail and asks for sure
 */
static void
on_delete()
{
	GtkTreeModel *model;
	GtkTreeIter  iter;
	GtkWidget    *dialog;
	gchar        *message;
	
	if( isItemSelected() )
	{
		/** It's a voicemail */
		if( isAValidItem() )
			message = "Do you really want to delete this voicemail ?";
		else /** It's a folder */
			message = "Do you really want to delete this folder ?\nAll voicemails will be definitely erased";
		/** Dialog box opening */
		dialog = gtk_message_dialog_new( GTK_WINDOW( VMWindow ),
										 GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_WARNING,
										 GTK_BUTTONS_OK_CANCEL,
										 _(message) );
		gtk_widget_show_all( dialog );
		/** Dialog box with question and user's answer */
		if( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_OK )
		{
			model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
			iter = getSelectedItem();
			gtk_tree_store_remove( GTK_TREE_STORE( model ), &iter );
			g_currently_playing = "";
		}
		/** Dialog box destruction */
		gtk_widget_destroy( dialog );
	}
}

/**
 * Gets a voicemail's informations to fill details textfield
 */
static void
on_cursor_changed( GtkWidget *widget, GdkEvent *event )
{
	GtkTextBuffer *buf;
	GtkTextIter iterStart;
	GtkTextIter iterEnd;
	
	if( isItemSelected() )
	{
		buf = gtk_text_view_get_buffer( GTK_TEXT_VIEW( textview ) );
		/* Deletes old data from the text buffer */
		gtk_text_buffer_get_start_iter( buf , &iterStart );
		gtk_text_buffer_get_end_iter( buf , &iterEnd );
		gtk_text_buffer_delete( buf , &iterStart , &iterEnd );

		/** It's a voicemail */
		if( isAValidItem() )
		{
			GtkTreeIter iter;
			gchar *infos;

			iter  = getSelectedItem();
			infos = dbus_get_voicemail_info( getSelectedItemParentName( iter ) , getSelectedItemName( iter ) );
			gtk_text_buffer_insert( buf , &iterStart , infos , -1 );
		}
	}
	else
	{
		
	}
}

/*********************************************************************************************************/

static void
do_popup_menu( GtkWidget *widget, GdkEventButton *event )
{
	GtkWidget *menu, *menu_item;
	int button, event_time;
	
	menu = gtk_menu_new();
	g_signal_connect( menu, "deactivate", G_CALLBACK( gtk_widget_destroy ), NULL );
	
	if( strcmp( g_currently_playing , "" ) == 0 )
	{
		menu_item = gtk_image_menu_item_new_from_stock( "gtk-media-play", NULL );
	}
	else
	{
		menu_item = gtk_image_menu_item_new_from_stock( "gtk-media-stop", NULL );
	}
	g_signal_connect( GTK_MENU_ITEM( menu_item ), "button_press_event", G_CALLBACK( on_play ), NULL );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu ), menu_item );
	menu_item = gtk_image_menu_item_new_from_stock( "gtk-delete", NULL );
	g_signal_connect( GTK_MENU_ITEM( menu_item ), "button_press_event", G_CALLBACK( on_delete ), NULL );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu ), menu_item );
	
	if( event )
	{
		button = event->button;
		event_time = event->time;
	}
	else
	{
		button = 0;
		event_time = gtk_get_current_event_time();
	}
	gtk_menu_attach_to_widget( GTK_MENU( menu ), widget, NULL );
	gtk_widget_show_all( menu );
	gtk_menu_popup( GTK_MENU( menu ), NULL, NULL, NULL, NULL, 
					button, event_time );
	
}

static gboolean
my_widget_button_press_event_handler( GtkWidget *widget, GdkEventButton *event )
{
	/* Ignore double-clicks and triple-clicks */
	if( event->button == 3 && event->type == GDK_BUTTON_PRESS && isItemSelected() )
	{
		do_popup_menu( widget, event );
		return TRUE;
	}
	return FALSE;
}

static gboolean
my_widget_popup_menu_handler( GtkWidget *widget )
{
	do_popup_menu( widget, NULL );
	return TRUE;
}


/*********************************************************************************************************/

/**
 * Creates the model to display the treeview
 */
void
create_tree( void )
{
	GtkTreeStore      * store;
	GtkTreeViewColumn * column;
	GtkCellRenderer   * cellRenderer;
	
	/** Model creation */
	store = gtk_tree_store_new( N_COLUMN, GDK_TYPE_PIXBUF, G_TYPE_STRING );
	/** View creation */
	gtk_tree_view_set_model( GTK_TREE_VIEW( treeview ), GTK_TREE_MODEL( store ) );

	/** First column creation */
	cellRenderer = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes( "I", cellRenderer, "pixbuf", IMG_COLUMN, NULL );
	/** Adding column to view */
	gtk_tree_view_append_column( GTK_TREE_VIEW( treeview ), column );

	/** Second column creation */
	cellRenderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( "L", cellRenderer, "text", TEXT_COLUMN, NULL );
	/** Adding column to view */
	gtk_tree_view_append_column( GTK_TREE_VIEW( treeview ), column );
	
	/** Adding right-click signal for "popup" menu */
	g_signal_connect( treeview, "button-press-event", G_CALLBACK( my_widget_button_press_event_handler ), NULL );
	g_signal_connect( treeview, "popup-menu",         G_CALLBACK( my_widget_popup_menu_handler ),         NULL );
}


/**
 * Updates treeview by adding a new element
 */
void
update_tree( gchar * text )
{
	GtkTreeIter  iter;
	GtkTreeModel *model;;
	GdkPixbuf    *pixbuf;

	pixbuf = gdk_pixbuf_new_from_file_at_scale( ICONS_DIR "/folder.svg", 20/*width*/, -1/*height*/, TRUE/*preserve_ratio*/, NULL/*error*/ );
	model  = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
	/* New line creation */
	gtk_tree_store_append( GTK_TREE_STORE( model ) , &iter , NULL );
	/* Data updates */
 	gtk_tree_store_set( GTK_TREE_STORE( model ) , &iter , IMG_COLUMN , pixbuf , TEXT_COLUMN , text , -1 );
}


/**
 * Updates treeview by adding childs (voicemails) to folders
 */
void
update_tree_complete( gchar **voicemails , int numline )
{
	GtkTreeIter  iterParent;
	GtkTreeIter  iterChild;
	GtkTreeModel *model;;
	GdkPixbuf    *pixBuf;
	gchar        *line;
	gint         i;
	
	g_sprintf( line , "%i\0" , numline );
	pixBuf = gdk_pixbuf_new_from_file_at_scale( ICONS_DIR "/play.svg" , 20/*width*/, -1/*height*/, TRUE/*preserve_ratio*/, NULL/*error*/ );
	model  = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );

	/** Gets the nth line to append voicemails */
	gtk_tree_model_get_iter_from_string( model , &iterParent , line );
	
	for( i=0 ; voicemails[i] ; i++ ) {
		g_print("  == update_tree_complete -> %s\n", voicemails[i]);
		/* New child line creation */
		gtk_tree_store_append( GTK_TREE_STORE( model ) , &iterChild , &iterParent );
		/* Updating datas */
		gtk_tree_store_set( GTK_TREE_STORE( model ) , &iterChild,
							IMG_COLUMN  , pixBuf,
							TEXT_COLUMN , voicemails[i],
							-1);
	}
}


/*********************************************************************************************************/

/**
 * Voice Mail Window creation
 */
void
create_voicemail_window( void )
{
	GtkWidget * vbox;
	GtkWidget * label;
	GtkWidget * scrolledwindow;
	GtkWidget * play_btn;
	GtkWidget * play_img;
	GtkWidget * stop_btn;
	GtkWidget * stop_img;
	GtkWidget * del_btn;
	GtkWidget * del_img;
	GtkWidget * hseparator;
	GtkWidget * hbox;
	GtkWidget * hscale;
	GtkWidget * expander;
	GtkTooltips *tooltips;
	
	tooltips = gtk_tooltips_new();
	gint i;
	
	/** Window */
	VMWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW( VMWindow ), _("Voicemail viewer") );
	gtk_window_set_position( GTK_WINDOW( VMWindow ) , GTK_WIN_POS_CENTER_ON_PARENT );
	gtk_window_set_default_size( GTK_WINDOW( VMWindow ), 500, 400 );
	gtk_window_set_default_icon_from_file( ICONS_DIR "/sflphone.png", NULL );
	GTK_WIDGET_SET_FLAGS( VMWindow, GTK_CAN_FOCUS );
	
	/** Vbox */
	vbox = gtk_vbox_new( FALSE, 0 );
	gtk_widget_show( vbox );
	gtk_container_add( GTK_CONTAINER( VMWindow ), vbox );
	
	/** Label */
	label = gtk_label_new("");
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vbox ), label, FALSE, FALSE, 0 );
	
	/** Scroll area with Tree view */
	scrolledwindow = gtk_scrolled_window_new( NULL, NULL );
	gtk_widget_show( scrolledwindow );
	gtk_box_pack_start( GTK_BOX( vbox ), scrolledwindow, TRUE, TRUE, 0 );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolledwindow ), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS );
	gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( scrolledwindow ), GTK_SHADOW_IN );
	
	/** Treeview */
	treeview = gtk_tree_view_new();
	create_tree();
	gtk_widget_show( treeview );
	gtk_tree_view_set_level_indentation( GTK_TREE_VIEW( treeview ) , -20 );
	// Don't show headers
	gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( treeview ) , FALSE );
	// Adds a color to lines
	gtk_tree_view_set_rules_hint( GTK_TREE_VIEW( treeview ), TRUE );
	gtk_container_add( GTK_CONTAINER( scrolledwindow ), treeview );
	// Adds double-click signal
	g_signal_connect( GTK_TREE_VIEW( treeview ), "row-activated", G_CALLBACK( on_play ), NULL );
	g_signal_connect( GTK_TREE_VIEW( treeview ), "cursor-changed", G_CALLBACK( on_cursor_changed ), NULL );
	
	
	/** Expander */
	expander = gtk_expander_new( _("Details") );
	gtk_box_pack_start( GTK_BOX( vbox ), expander, FALSE, FALSE, 0 );
	
	/** Text View */
	textview = gtk_text_view_new();
	gtk_text_buffer_set_text( gtk_text_view_get_buffer( GTK_TEXT_VIEW( textview ) ) , "" , -1 );
	gtk_text_view_set_editable( GTK_TEXT_VIEW( textview ), FALSE );
	gtk_text_view_set_border_window_size( GTK_TEXT_VIEW( textview ), GTK_TEXT_WINDOW_RIGHT, 5 );
	gtk_container_add( GTK_CONTAINER( expander ), textview );
	
	/** Separator */
	hseparator = gtk_hseparator_new();
	gtk_widget_show( hseparator );
	gtk_box_pack_start( GTK_BOX( vbox ), hseparator, FALSE, FALSE, 10 );
	gtk_widget_set_sensitive( GTK_WIDGET( hseparator ) , FALSE );
	
	/** Hbox */
	hbox = gtk_hbox_new( FALSE, 0 );
	gtk_widget_show( hbox );
	gtk_box_pack_start( GTK_BOX( vbox ), hbox, FALSE, TRUE, 0 );
	
	/** Play Button */
	play_btn = gtk_button_new ();
	gtk_widget_show( play_btn );
	gtk_box_pack_start( GTK_BOX( hbox ), play_btn, FALSE, FALSE, 0 );
	gtk_tooltips_set_tip( tooltips, play_btn, _("Play selected voicemail"), NULL );
	g_signal_connect( GTK_BUTTON( play_btn ), "clicked", G_CALLBACK( on_play ), NULL );
	/** Play image button */
	play_img = gtk_image_new_from_stock( "gtk-media-play", GTK_ICON_SIZE_BUTTON );
	gtk_widget_show( play_img );
	gtk_container_add( GTK_CONTAINER( play_btn ), play_img );
	
	/** Stop Button */
	stop_btn = gtk_button_new();
	gtk_widget_show( stop_btn );
	gtk_box_pack_start( GTK_BOX( hbox ), stop_btn, FALSE, FALSE, 0 );
	gtk_tooltips_set_tip( tooltips, stop_btn, _("Stop selected voicemail"), NULL );
	g_signal_connect( GTK_BUTTON( stop_btn ), "clicked", G_CALLBACK( on_stop ), NULL );
	/** Stop image button */
	stop_img = gtk_image_new_from_stock( "gtk-media-stop", GTK_ICON_SIZE_BUTTON );
	gtk_widget_show( stop_img );
	gtk_container_add( GTK_CONTAINER( stop_btn ), stop_img );
	
	/** Hscale for player time */
	hscale = gtk_hscale_new( GTK_ADJUSTMENT( gtk_adjustment_new( 1 /* min */,100 /* max */,5 /* current */,1 /* minor increment */,2 /* major increment */,10 /* page size */ ) ) );
	gtk_widget_show( hscale );
	gtk_box_pack_start( GTK_BOX( hbox ), hscale, TRUE, TRUE, 0 );
	gtk_scale_set_draw_value( GTK_SCALE( hscale ), FALSE );
	gtk_scale_set_value_pos( GTK_SCALE( hscale ), GTK_POS_BOTTOM );
	gtk_scale_set_digits( GTK_SCALE( hscale ), 0 );
	
	/** Delete Button */
	del_btn = gtk_button_new();
	gtk_widget_show( del_btn );
	gtk_box_pack_start( GTK_BOX( hbox ), del_btn, FALSE, FALSE, 0 );
	gtk_tooltips_set_tip( tooltips, del_btn, _("Delete selected voicemail"), NULL );
	g_signal_connect( GTK_BUTTON( del_btn ), "clicked", G_CALLBACK( on_delete ), NULL );
	/** Delete image button */
	del_img = gtk_image_new_from_stock( "gtk-delete", GTK_ICON_SIZE_BUTTON );
	gtk_widget_show( del_img );
	gtk_container_add( GTK_CONTAINER( del_btn), del_img );

	gtk_widget_show_all( VMWindow );
}


/**
 * Shows Voice Mail Window
 */
void
show_voicemail_window(void)
{
	gint i = 0;
	GtkWidget *dialog;

	create_voicemail_window();
	gchar **errors = (gchar **)dbus_get_list_errors();
	for( i=0 ; errors[i] ; i++ ) {
		main_window_warning_message( errors[i] );
	}
	g_free( errors );
	gchar **list = (gchar**)dbus_get_list_folders();
	for( i=0 ; list[i] ; i++ ) {
		gchar * folder = list[i];
		update_tree( folder );
		gint count = dbus_get_folder_count( folder );
		if( count != 0 )
		{
			gchar ** voicemails = ( gchar ** )dbus_get_list_mails( folder );
			update_tree_complete( voicemails , i );
		}
	}
}

