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


/*
 * HOW TO COMPILE ALONE
 * gcc `pkg-config --libs --cflags gtk+-2.0` voicemailwindow.c -o voice
 * ./voice
 */

#include <stdlib.h>
#include <stdio.h>
#include <voicemailwindow.h>
#include <config.h>
//#define ICONS_DIR "../pixmaps"
#include <gtk/gtk.h>


/** Local variables */
GtkWidget * VMWindow;

GtkWidget * treeview;
gint currently_playing = -1;


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
getItemSelected( void )
{
	GtkTreeSelection *selection;
	GtkTreeModel     *model;
	GtkTreeIter      iter;

	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW( treeview ) );
	gtk_tree_selection_get_selected( selection, &model, &iter );
	return iter;
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
	if( gtk_tree_selection_get_selected( selection, &model, NULL ) )
		return TRUE;
	else
		return FALSE;
}


/**
 * Stops currently playing voicemail (if any)
 */
static void
on_stop()
{
	if( currently_playing != -1 )
	{
		GtkTreeIter iter;
		GtkTreeModel * model;
		GdkPixbuf * pixbuf;
		gchar * text;
		
		g_print( "Player -- Stop\n" );
		model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
		/** Gets nth row to stop */
		gtk_tree_model_iter_nth_child( model, &iter, NULL, currently_playing );
		/** Gets text row */
		gtk_tree_model_get( GTK_TREE_MODEL( model ), &iter, IMG_COLUMN, &pixbuf, TEXT_COLUMN, &text, -1 );
		/** Sets new image */
		pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/play.svg", NULL);
		/** Updates selected row */
		gtk_list_store_set( GTK_LIST_STORE( model ), &iter, IMG_COLUMN, pixbuf, TEXT_COLUMN, text, -1 );
		/** Updates to none voicemail playing */
		currently_playing = -1;
	}
}


static void
on_pause()
{
	if( isItemSelected() )
	{
		GtkTreeIter iter = getItemSelected();
		g_print("Player -- Pause\n");
		
		//iter = getItemSelected();
		//model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
		/** Gets text row */
		//gtk_tree_model_get( GTK_TREE_MODEL( model ), &iter, IMG_COLUMN, &pixbuf, TEXT_COLUMN, &text, -1 );
		/** Sets new image */
		//pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/play.svg", NULL);
		/** Updates selected row */
		//gtk_list_store_set( GTK_LIST_STORE( model ), &iter, IMG_COLUMN, pixbuf, TEXT_COLUMN, text, -1 );*/
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
		GtkTreeIter iter;
		GtkTreeModel * model;
		GdkPixbuf * pixbuf;
		gchar * text;
		gboolean valid;
		
		model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
		iter = getItemSelected();
		/** If not the same voicemail to play */
		if( currently_playing != atoi( gtk_tree_model_get_string_from_iter( model, &iter ) ) )
		{
			/** First, stop playing last voicemail */
			if( currently_playing != -1 )
			{
				on_stop();
			}
			g_print( "Player -- Play\n" );
			/** Second, play selected voicemail */
			iter = getItemSelected();
			/** Gets text row */
			gtk_tree_model_get( GTK_TREE_MODEL( model ), &iter, IMG_COLUMN, &pixbuf, TEXT_COLUMN, &text, -1 );
			/** Sets new image */
			pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/pause.svg", NULL);
			/** Updates selected row */
			gtk_list_store_set( GTK_LIST_STORE( model ), &iter, IMG_COLUMN, pixbuf, TEXT_COLUMN, text, -1 );
			/** Updates voicemail currently playing */
			currently_playing = atoi( gtk_tree_model_get_string_from_iter( model, &iter ) );
		}
		else
		{
			on_stop();
		}
	}
}


/**
 * Deletes the selected voicemail and asks for sure
 */
static void
on_delete()
{
	GtkTreeModel * model;
	GtkTreeIter iter;
	GtkWidget * dialog;
	GtkWidget * label;
	gboolean deleted;
	
	if( isItemSelected() )
	{
		g_print("Player -- Delete\n");
		/** Dialog box opening */
		dialog = gtk_message_dialog_new( GTK_WINDOW( VMWindow ),
										 GTK_DIALOG_DESTROY_WITH_PARENT,
										 GTK_MESSAGE_WARNING,
										 GTK_BUTTONS_OK_CANCEL,
										 "Do you really want to delete this message ?" );
		gtk_widget_show_all( dialog );
		/** Dialog box with question and user's answer */
		if( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_OK )
		{
			g_print(" on_delete - OK\n");
			model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
			iter = getItemSelected();
			deleted = gtk_list_store_remove( GTK_LIST_STORE( model ), &iter );
			g_print(" has been deleted ? %s\n", deleted==TRUE ? "TRUE" : "FALSE" );
			currently_playing = -1;
		}
		/** Dialog box destruction */
		gtk_widget_destroy( dialog );
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
	
	menu_item = gtk_image_menu_item_new_from_stock( "gtk-media-play", NULL );
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
	GtkListStore      * store;
	GtkTreeViewColumn * column;
	GtkCellRenderer   * cellRenderer;
//	gchar             * text;
//	GdkPixbuf         * pixBuf;
//	gint i;
	
	/** Model creation */
	store = gtk_list_store_new( N_COLUMN, GDK_TYPE_PIXBUF, G_TYPE_STRING );
//	text = g_malloc( 14 );
//	pixBuf = gdk_pixbuf_new_from_file( ICONS_DIR "/play.svg", NULL);


	/** View creation */
	gtk_tree_view_set_model( GTK_TREE_VIEW( treeview ), GTK_TREE_MODEL( store ) );
	/** First column creation */
	cellRenderer = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes( "Image", cellRenderer, "pixbuf", IMG_COLUMN, NULL );
	/** Adding column to view */
	gtk_tree_view_append_column( GTK_TREE_VIEW( treeview ), column );
	/** Second column creation */
	cellRenderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( "Label", cellRenderer, "text", TEXT_COLUMN, NULL );
	/** Adding column to view */
	gtk_tree_view_append_column( GTK_TREE_VIEW( treeview ), column );

	/** Adding elements - only for tests *
	for( i = 1 ; i < 5 ; ++i )
	{
		GtkTreeIter iter;
//		GtkTreeIter iter2;

		//g_sprintf( text, "<b>Mail</b> #%d\0", i );
		text = g_markup_printf_escaped( "<b>Mail</b> #%d\0", i );
		/* New line creation *
		gtk_list_store_append( store, &iter );
		/* Data updates *
		gtk_list_store_set( store, &iter, IMG_COLUMN, pixBuf, TEXT_COLUMN, text, -1 );
	}
	g_free( text );*/
	
	/** Adding signal for "popup" menu */
	g_signal_connect( treeview, "button-press-event", G_CALLBACK( my_widget_button_press_event_handler ), NULL );
	g_signal_connect( treeview, "popup-menu",         G_CALLBACK( my_widget_popup_menu_handler ),         NULL );
}


/**
 * Updates treeview by adding a new element
 */
void
update_tree( gchar * text )
{
	GtkTreeIter    iter;
	GtkTreeModel * model;;
	GdkPixbuf    * pixBuf;

	pixBuf = gdk_pixbuf_new_from_file( ICONS_DIR "/play.svg", NULL);
	model = gtk_tree_view_get_model( GTK_TREE_VIEW( treeview ) );
	/* New line creation */
	gtk_list_store_append( GTK_LIST_STORE( model ), &iter );
	/* Data updates */
	gtk_list_store_set( GTK_LIST_STORE( model ), &iter, IMG_COLUMN, pixBuf, TEXT_COLUMN, text, -1 );
}


/**
 * Signal - Doesn't delete voicemail window, simply hide it
 */
static void
on_window_delete(GtkWidget *pWidget, gpointer pData)
{
	gtk_widget_hide( VMWindow );
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
	GtkTooltips *tooltips;
	
	tooltips = gtk_tooltips_new();
	gint i;
	
	/** Window */
	VMWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW( VMWindow ), "Voicemail viewer" );
	gtk_window_set_position( GTK_WINDOW( VMWindow ), GTK_WIN_POS_CENTER );
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
	
	treeview = gtk_tree_view_new();
	/** TODO : delete create_tree() */
	create_tree();
	for( i = 1 ; i < 6 ; i++ )
	{
		update_tree( g_markup_printf_escaped( "<b>Mail</b> <i>#%d</i>" , i ) );
	}
	/********************************/
	gtk_widget_show( treeview );
	gtk_container_add( GTK_CONTAINER( scrolledwindow ), treeview );
	gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( treeview ), FALSE );
	g_signal_connect( GTK_TREE_VIEW( treeview ), "row-activated", G_CALLBACK( on_play ), NULL );
	
	/** Separator */
	hseparator = gtk_hseparator_new();
	gtk_widget_show( hseparator );
	gtk_box_pack_start( GTK_BOX( vbox ), hseparator, FALSE, FALSE, 10 );
	gtk_widget_set_sensitive( hseparator, FALSE );
	
	/** Hbox */
	hbox = gtk_hbox_new( FALSE, 0 );
	gtk_widget_show( hbox );
	gtk_box_pack_start( GTK_BOX( vbox ), hbox, FALSE, TRUE, 0 );
	
	/** Play Button */
	play_btn = gtk_button_new ();
	gtk_widget_show( play_btn );
	gtk_box_pack_start( GTK_BOX( hbox ), play_btn, FALSE, FALSE, 0 );
	gtk_tooltips_set_tip( tooltips, play_btn, "Play selected voice mail", NULL );
	g_signal_connect( GTK_BUTTON( play_btn ), "clicked", G_CALLBACK( on_play ), NULL );
	/** Play image button */
	play_img = gtk_image_new_from_stock( "gtk-media-play", GTK_ICON_SIZE_BUTTON );
	gtk_widget_show( play_img );
	gtk_container_add( GTK_CONTAINER( play_btn ), play_img );
	
	/** Stop Button */
	stop_btn = gtk_button_new();
	gtk_widget_show( stop_btn );
	gtk_box_pack_start( GTK_BOX( hbox ), stop_btn, FALSE, FALSE, 0 );
	gtk_tooltips_set_tip( tooltips, stop_btn, "Stop selected voice mail", NULL );
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
	gtk_tooltips_set_tip( tooltips, del_btn, "Delete selected voice mail", NULL );
	g_signal_connect( GTK_BUTTON( del_btn ), "clicked", G_CALLBACK( on_delete ), NULL );
	/** Delete image button */
	del_img = gtk_image_new_from_stock( "gtk-delete", GTK_ICON_SIZE_BUTTON );
	gtk_widget_show( del_img );
	gtk_container_add( GTK_CONTAINER( del_btn), del_img );
	
	/** Adding destroy signal to voice mail window */
	g_signal_connect( G_OBJECT( VMWindow ), "delete-event", G_CALLBACK( on_window_delete ), NULL);
	
	gtk_widget_show_all( VMWindow );
}


/**
 * Shows Voice Mail Window or sets focus on it if already launched
 */
void
show_voicemail_window(void)
{
	if( VMWindow == NULL )
		create_voicemail_window();
	else
	{
		gtk_widget_show_all( VMWindow );
		// TODO : sets focus !! doesn't work yet
		gtk_widget_grab_focus( VMWindow );
	}
}

/*
int
main( int argc, char * argv[])
{
	gtk_init(&argc,&argv);
	// Connexion du signal "destroy"
	show_voicemail_window();
	// Demarrage de la boucle evenementielle
	gtk_main();
}
*/
