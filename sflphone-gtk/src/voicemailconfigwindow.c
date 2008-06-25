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
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libintl.h>

#include "dbus.h"
#include "sflphone_const.h"
#include "voicemailconfigwindow.h"

void
show_voicemail_config_window( void )
{
	GtkWidget *vmconfig;
	GtkWidget *dialogContent;

	GtkWidget *frameIdent;
	GtkWidget *frameServer;

	GtkWidget *hboxFrame;
	GtkWidget *hbox2;
	GtkWidget *hbox3;
	GtkWidget *vboxIdentLabels;
	GtkWidget *vboxIdentEntries;
	GtkWidget *vbox3;
	GtkWidget *vbox4;

	GtkWidget *entry;
	GtkWidget *label;

	GtkWidget *combobox;
	GtkWidget *comboboxentry;
	GtkWidget *comboboxentry_entry;

	GtkWidget *alignment;

	GtkObject *spinbutton1_adj;
	GtkWidget *spinbutton1;

	GtkWidget *checkbutton1;
	GtkWidget *checkbutton2;

	GtkWidget *dialog_action_area1;


	vmconfig = gtk_dialog_new_with_buttons( _("Voicemail configuration"),
											GTK_WINDOW(get_main_window()),
											GTK_DIALOG_MODAL,
											GTK_STOCK_OK,
											GTK_RESPONSE_ACCEPT,
											GTK_STOCK_CANCEL,
											GTK_RESPONSE_CANCEL,
											NULL );
	gtk_window_set_default_size( GTK_WINDOW( vmconfig ) , 300 , 400 );

	dialogContent = GTK_DIALOG( vmconfig )->vbox;
	gtk_widget_show( dialogContent );

	frameIdent = gtk_frame_new( NULL );
	gtk_widget_show( frameIdent );
	gtk_box_pack_start( GTK_BOX( dialogContent ) , frameIdent , TRUE , TRUE , 0 );
	gtk_frame_set_shadow_type( GTK_FRAME( frameIdent ) , GTK_SHADOW_IN );

	alignment = gtk_alignment_new( 0.5 , 0.5 , 1 , 1 );
	gtk_widget_show( alignment );
	gtk_container_add( GTK_CONTAINER( frameIdent ) , alignment );
	gtk_alignment_set_padding( GTK_ALIGNMENT( alignment ) , 0 , 5 , 5 , 5 );

	hboxFrame = gtk_hbox_new( FALSE , 0 );
	gtk_widget_show( hboxFrame );
	gtk_container_add( GTK_CONTAINER( alignment ) , hboxFrame );

	vboxIdentLabels = gtk_vbox_new( FALSE , 0 );
	gtk_widget_show( vboxIdentLabels );
	gtk_box_pack_start( GTK_BOX( hboxFrame ) , vboxIdentLabels , TRUE , TRUE , 0 );

	label = gtk_label_new( _("Account alias") );
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vboxIdentLabels ) , label , TRUE , TRUE , 0 );
	gtk_misc_set_alignment( GTK_MISC( label ) , 0 , 0.5 );

	label = gtk_label_new( _("Login") );
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vboxIdentLabels ) , label , TRUE , TRUE , 0 );
	gtk_misc_set_alignment( GTK_MISC( label ) , 0 , 0.5 );

	label = gtk_label_new( _("Passcode") );
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vboxIdentLabels ) , label , TRUE , TRUE , 0 );
	gtk_misc_set_alignment( GTK_MISC( label ) , 0 , 0.5 );

	label = gtk_label_new( _("Context") );
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vboxIdentLabels ) , label , TRUE , TRUE , 0 );
	gtk_misc_set_alignment( GTK_MISC( label ) , 0 , 0.5 );

	vboxIdentEntries = gtk_vbox_new( FALSE , 0 );
	gtk_widget_show( vboxIdentEntries );
	gtk_box_pack_start( GTK_BOX( hboxFrame ) , vboxIdentEntries , TRUE , TRUE , 0 );

	combobox = gtk_combo_box_new();
	gtk_widget_show( combobox );
	gtk_box_pack_start( GTK_BOX( vboxIdentEntries ) , combobox , TRUE , FALSE , 0 );
//	gtk_combo_box_append_text( GTK_COMBO_BOX( combobox ) , _("default") );

	entry = gtk_entry_new();
	gtk_widget_show( entry );
	gtk_box_pack_start( GTK_BOX( vboxIdentEntries ) , entry , TRUE , FALSE , 0 );
	gtk_entry_set_invisible_char( GTK_ENTRY( entry ) , 9679 );

	entry = gtk_entry_new();
	gtk_widget_show( entry );
	gtk_box_pack_start( GTK_BOX( vboxIdentEntries ) , entry , TRUE , FALSE , 0 );
	gtk_entry_set_invisible_char( GTK_ENTRY( entry ) , 9679 );

	comboboxentry = gtk_combo_box_entry_new_text();
	gtk_widget_show( comboboxentry );
	gtk_box_pack_start( GTK_BOX( vboxIdentEntries ) , comboboxentry , TRUE , FALSE , 0 );
	gtk_combo_box_append_text( GTK_COMBO_BOX( comboboxentry ) , _("default") );

	comboboxentry_entry = gtk_entry_new();
	gtk_widget_show( comboboxentry_entry );
	gtk_container_add( GTK_CONTAINER( comboboxentry ) , comboboxentry_entry );
	gtk_entry_set_invisible_char( GTK_ENTRY( comboboxentry_entry ) , 9679 );

	label = gtk_label_new( _("Identification") );
	gtk_widget_show( label );
	gtk_frame_set_label_widget( GTK_FRAME( frameIdent ) , label );
	gtk_label_set_use_markup( GTK_LABEL( label ) , TRUE );

	frameServer = gtk_frame_new( NULL );
	gtk_widget_show( frameServer );
	gtk_box_pack_start( GTK_BOX( dialogContent ) , frameServer , TRUE , TRUE , 0 );
	gtk_frame_set_shadow_type( GTK_FRAME( frameServer ) , GTK_SHADOW_IN );

	alignment = gtk_alignment_new( 0.5 , 0.5 , 1 , 1 );
	gtk_widget_show( alignment );
	gtk_container_add( GTK_CONTAINER( frameServer ) , alignment );
	gtk_alignment_set_padding( GTK_ALIGNMENT( alignment ) , 0 , 5 , 5 , 5 );

	hbox2 = gtk_hbox_new( FALSE , 0 );
	gtk_widget_show( hbox2 );
	gtk_container_add( GTK_CONTAINER( alignment ) , hbox2 );

	vbox3 = gtk_vbox_new( FALSE , 0 );
	gtk_widget_show( vbox3 );
	gtk_box_pack_start( GTK_BOX( hbox2 ) , vbox3 , TRUE , TRUE , 0 );

	label = gtk_label_new( _("Server address") );
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vbox3 ) , label , TRUE , TRUE , 0 );
	gtk_misc_set_alignment( GTK_MISC( label ) , 0 , 0.5 );

	label = gtk_label_new( _("Server path") );
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vbox3 ) , label , TRUE , TRUE , 0 );
	gtk_misc_set_alignment( GTK_MISC( label ) , 0 , 0.5 );

	label = gtk_label_new( _("Server port") );
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vbox3 ) , label , TRUE , TRUE , 0 );
	gtk_misc_set_alignment( GTK_MISC( label ) , 0 , 0.5 );

	label = gtk_label_new( _("Use HTTPS ?") );
	gtk_widget_show( label );
	gtk_box_pack_start( GTK_BOX( vbox3 ) , label , TRUE , TRUE , 0 );
	gtk_misc_set_alignment( GTK_MISC( label ) , 0 , 0.5 );

	vbox4 = gtk_vbox_new( FALSE , 0 );
	gtk_widget_show( vbox4 );
	gtk_box_pack_start( GTK_BOX( hbox2 ) , vbox4 , TRUE , TRUE , 0 );

	entry = gtk_entry_new();
	gtk_widget_show( entry );
	gtk_box_pack_start( GTK_BOX( vbox4 ) , entry , TRUE , FALSE , 0 );
	gtk_entry_set_invisible_char( GTK_ENTRY( entry ) , 9679 );

	entry = gtk_entry_new();
	gtk_widget_show( entry );
	gtk_box_pack_start( GTK_BOX( vbox4 ) , entry , TRUE , FALSE , 0 );
	gtk_entry_set_invisible_char( GTK_ENTRY( entry ) , 9679 );

	spinbutton1_adj = gtk_adjustment_new( 80 , 0 , 100 , 1 , 10 , 10 );
	spinbutton1 = gtk_spin_button_new( GTK_ADJUSTMENT( spinbutton1_adj ) , 1 , 0 );
	gtk_widget_show( spinbutton1 );
	gtk_box_pack_start( GTK_BOX( vbox4 ) , spinbutton1 , TRUE , FALSE , 0 );

	hbox3 = gtk_hbox_new( FALSE , 0 );
	gtk_widget_show( hbox3 );
	gtk_box_pack_start( GTK_BOX( vbox4 ) , hbox3 , TRUE , TRUE , 0 );

	checkbutton1 = gtk_radio_button_new_with_label_from_widget( NULL , "gtk-yes" );
	gtk_button_set_use_stock( GTK_BUTTON( checkbutton1 ) , TRUE );
	gtk_widget_show( checkbutton1 );
	gtk_box_pack_start( GTK_BOX( hbox3 ) , checkbutton1 , TRUE , FALSE , 0 );

	checkbutton2 = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON( checkbutton1 ) , "gtk-no" );
	gtk_button_set_use_stock( GTK_BUTTON( checkbutton2 ) , TRUE );
	gtk_widget_show( checkbutton2 );
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( checkbutton2 ) , TRUE );
	gtk_box_pack_start( GTK_BOX( hbox3 ) , checkbutton2 , TRUE , FALSE , 0 );

	label = gtk_label_new( _("Voicemail Server") );
	gtk_widget_show( label );
	gtk_frame_set_label_widget( GTK_FRAME( frameServer ) , label );
	gtk_label_set_use_markup( GTK_LABEL( label ) , TRUE );

	dialog_action_area1 = GTK_DIALOG( vmconfig )->action_area;
	gtk_widget_show( dialog_action_area1 );
	gtk_button_box_set_layout( GTK_BUTTON_BOX( dialog_action_area1 ) , GTK_BUTTONBOX_END );

	gtk_widget_show_all( vmconfig );

}

