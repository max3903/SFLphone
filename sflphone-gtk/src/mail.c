/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
 *  Author: Florian Desportes <florian.depsortes@savoirfairelinux.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h> // For pixmaps
#include <glib.h> // For GHashTable

#include "dbus.h"
#include "sflphone_const.h"
#include "mail.h"

enum {
	VM_IMG_COLUMN,
	VM_TEXT_COLUMN,
	VM_DATA_COLUMN,
	VM_N_COLUMN
};

mailtab_t*
mailtab_init(void)
{
	mailtab_t* ret;
	ret = malloc(sizeof(mailtab_t));

	ret->store     = NULL;
	ret->view      = NULL;
	ret->tree      = NULL;

	create_mail_view(ret);
	mail_list_init(ret);
	
	return ret;
}

static void
on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	GtkTreeIter  iter;
	GValue       val = { 0 , };
	GtkTreeModel *model;
	mail_t       *mail;
	
	if( ! gtk_tree_selection_get_selected(gtk_tree_view_get_selection(tree_view), &model, &iter) )
		return;

	gtk_tree_model_get_value(model, &iter, 2, &val);
	mail = (mail_t*) g_value_get_pointer(&val);
	g_value_unset(&val);
	if( mail != NULL )
	{
		g_print("\n++ on_row_activated\n");
		if( ! mail->isPlaying )
		{
			mail->isPlaying = TRUE;
			g_print(" ++ play => %s/%s\n", mail->folder, mail->name);
			dbus_play_voicemail(mail->folder, mail->name);
			voicemailInbox->selectedMail = mail;
		}
		else
		{
			dbus_stop_voicemail();
		}
	}
//	g_free(mail);
}


/**
 * Receiving signal from server that listening to voicemail is started
 */
void
mail_is_playing(void)
{
	if( voicemailInbox->selectedMail == NULL )
	{
		GtkTreeIter      iter;
		GtkTreeModel     *model;
		GdkPixbuf        *pixbuf;
		GtkTreeSelection *selection;
		GValue           val;
		mail_t           *mail;
		
		g_print("mail_is_playing\n");
		
		model     = gtk_tree_view_get_model(GTK_TREE_VIEW(voicemailInbox->view));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(voicemailInbox->view));
		gtk_tree_selection_get_selected(selection, &model, &iter);

		gtk_tree_model_get_value(model, &iter, 2, &val);
		mail = (mail_t*) g_value_get_pointer(&val);
		g_value_unset(&val);
		voicemailInbox->selectedMail = mail;
	
		/** Sets new image */
//		pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/stop.svg", NULL/*error*/);
		pixbuf = gdk_pixbuf_new_from_file_at_scale( ICONS_DIR "/stop.svg",
													40 /*width*/,
													-1 /*height*/,
													TRUE /*preserve_aspect_ratio*/,
													NULL/*error*/ );
		/** Updates selected row */
//		gtk_tree_store_set(GTK_TREE_STORE(model), &iter, VM_IMG_COLUMN, pixbuf, -1);
		gtk_list_store_set(GTK_LIST_STORE(voicemailInbox->store), &iter, VM_IMG_COLUMN, pixbuf, -1);
	}
}


/**
 * Receiving signal from server that listening to voicemail is stopped 
 */
void
mail_is_stopped(void)
{
	GtkTreeIter      iter;
	GtkTreeModel     *model;
	GdkPixbuf        *pixbuf;
	GtkTreeSelection *selection;
	GValue           val = { 0 , };
	mail_t           *mail;

	g_print("mail_is_stopped\n");

	model     = gtk_tree_view_get_model(GTK_TREE_VIEW(voicemailInbox->view));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(voicemailInbox->view));
	/** Gets nth row to stop */
	gtk_tree_selection_get_selected(selection, &model, &iter);
	/** Sets new image */
//	pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/play.svg", NULL/*error*/);
	pixbuf = gdk_pixbuf_new_from_file_at_scale( ICONS_DIR "/play.svg",
												40 /*width*/,
												-1 /*height*/,
												TRUE /*preserve_aspect_ratio*/,
												NULL/*error*/ );
	/** Updates selected row, just modify the image */
//	gtk_tree_store_set( GTK_TREE_STORE(model), &iter,
//						VM_IMG_COLUMN, pixbuf,
//						-1);
	gtk_list_store_set( GTK_LIST_STORE(voicemailInbox->store), &iter,
						VM_IMG_COLUMN, pixbuf,
						-1);

	gtk_tree_model_get_value(model, &iter, 2, &val);
	mail = (mail_t*) g_value_get_pointer(&val);
	g_value_unset(&val);
	mail->isPlaying = FALSE;
	voicemailInbox->selectedMail = NULL;
}


void
mail_catch_error(gchar *err)
{
	g_print("------- voicemail_catch_error -----------\n");
	main_window_error_message(err);
}


void
create_mail_view(mailtab_t * tab)
{
	GtkCellRenderer   *cellRenderer;
	GtkTreeViewColumn *column;
	GtkWidget         *wid;
	
	tab->tree = gtk_vbox_new(FALSE, 10); 
	gtk_container_set_border_width(GTK_CONTAINER(tab->tree), 0);

	wid = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(wid), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(wid), GTK_SHADOW_IN);

	/** Model creation */
//	tab->store = gtk_tree_store_new( VM_N_COLUMN,
	tab->store = gtk_list_store_new( VM_N_COLUMN,
									 GDK_TYPE_PIXBUF,
									 G_TYPE_STRING,
									 G_TYPE_POINTER );
	/** View creation */
	tab->view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tab->store));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tab->view), FALSE);

	/** First column = PIXBUF */
	cellRenderer = gtk_cell_renderer_pixbuf_new();
	gtk_object_set(GTK_OBJECT(cellRenderer), "width" , 30, NULL);
	gtk_object_set(GTK_OBJECT(cellRenderer), "xpad"  , 0 , NULL);
	column = gtk_tree_view_column_new_with_attributes("P", cellRenderer, "pixbuf", VM_IMG_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tab->view), column);

	/** Second column = STRING */
	cellRenderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("S", cellRenderer, "markup", VM_TEXT_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tab->view), column);

	/** Deleting vars and adding treeview to GUI */
	g_object_unref(G_OBJECT(tab->store));
	gtk_container_add(GTK_CONTAINER(wid), tab->view);
	gtk_box_pack_start(GTK_BOX(tab->tree), wid, TRUE, TRUE, 0);
	g_signal_connect(tab->view, "row-activated", G_CALLBACK(on_row_activated), NULL);
	gtk_widget_show(tab->tree);
}


void
mail_list_init(mailtab_t * tab)
{
	gchar **folders = (gchar**)dbus_get_list_folders();
	gchar **fl;

	for( fl = folders ; *folders ; folders++ ) {
		gchar       **folder = g_strsplit(*folders, "|", 2);
		gint        count = dbus_get_folder_count(folder[1]);
		GtkTreeIter iter;

		gtk_list_store_append(GTK_LIST_STORE(tab->store), &iter);
		gtk_list_store_set( GTK_LIST_STORE(tab->store), &iter,
							VM_TEXT_COLUMN            , _(folder[0]),
							-1);

		if( count != 0 )
		{
			GdkPixbuf *pixBuf;
			gchar     **voicemails = (gchar **)dbus_get_list_mails(folder[1]);
//			g_print("dbus_get_list_mails(%s)\n", n[1]);
			gchar     **lst;
			
			for( lst = voicemails ; *voicemails ; voicemails++ ) {
				gchar      **t;
				mail_t     *ret;
				GHashTable *infos;
				gchar      *text;
				
				/* New child line creation */
//				gtk_tree_store_append(GTK_TREE_STORE(tab->store), &iter, NULL);
				gtk_list_store_append(GTK_LIST_STORE(tab->store), &iter);
			
				/** Splits voicemail information as decribed : "who_called_and_time_of_call|name_of_the_file" */
				ret = g_new(mail_t, 1);
				infos = (GHashTable *)dbus_get_voicemail(folder[1], *voicemails);
				if( infos == NULL )  break;
				ret->name      = g_hash_table_lookup(infos, "Name");
				ret->folder    = g_hash_table_lookup(infos, "Folder");
				ret->from      = g_hash_table_lookup(infos, "From");
				ret->date      = g_hash_table_lookup(infos, "Date");
				ret->isPlaying = FALSE;
				
				text = g_strdup_printf("From <b>%s</b>\non <i>%s</i>", ret->from, ret->date);

//				pixBuf = gdk_pixbuf_new_from_file(ICONS_DIR "/play.svg", NULL/*error*/);
				pixBuf = gdk_pixbuf_new_from_file_at_scale( ICONS_DIR "/play.svg",
															40 /*width*/,
															-1 /*height*/,
															TRUE /*preserve_aspect_ratio*/,
															NULL /*error*/ );
				/** Updating datas */
//				gtk_tree_store_append(GTK_TREE_STORE(model), &iter, &iterParent);
//				gtk_tree_store_set( GTK_TREE_STORE(tab->store), &iter,
//				gtk_object_set(GTK_OBJECT(&iter), "cell-background-gdk", &bgcolor, NULL);
				gtk_list_store_set( GTK_LIST_STORE(tab->store), &iter,
									VM_IMG_COLUMN             , pixBuf,
									VM_TEXT_COLUMN            , _(text),
									VM_DATA_COLUMN            , ret,
									-1);
			}
			g_strfreev(lst);
		}
		g_strfreev(folder);
	}
//	g_strfreev(ls);
}

