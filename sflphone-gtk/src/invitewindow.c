/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
 *  Author: Marilyne Mercier <marilyne.mercier@polymtl.ca>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "invitewindow.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_Join_conf (void)
{
  GtkWidget *Join_conf;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *dialog_action_area1;
  GtkWidget *Join;
  GtkWidget *Cancel;

  Join_conf = gtk_dialog_new ();
  gtk_container_set_border_width (GTK_CONTAINER (Join_conf), 5);
  gtk_widget_set_events (Join_conf, GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
  gtk_window_set_position (GTK_WINDOW (Join_conf), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint (GTK_WINDOW (Join_conf), GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_dialog_set_has_separator (GTK_DIALOG (Join_conf), FALSE);

  dialog_vbox1 = GTK_DIALOG (Join_conf)->vbox;
  gtk_widget_show (dialog_vbox1);

  label1 = gtk_label_new (("The connection has been established \n Press join to start the conference"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label1, TRUE, TRUE, 0);

  dialog_action_area1 = GTK_DIALOG (Join_conf)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  Join = gtk_button_new_with_mnemonic (("Join"));
  gtk_widget_show (Join);
  gtk_dialog_add_action_widget (GTK_DIALOG (Join_conf), Join, 0);

  Cancel = gtk_button_new_with_mnemonic (("Cancel"));
  gtk_widget_show (Cancel);
  gtk_dialog_add_action_widget (GTK_DIALOG (Join_conf), Cancel, 0);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (Join_conf, Join_conf, "Join_conf");
  GLADE_HOOKUP_OBJECT_NO_REF (Join_conf, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (Join_conf, label1, "label1");
  GLADE_HOOKUP_OBJECT_NO_REF (Join_conf, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (Join_conf, Join, "Join");
  GLADE_HOOKUP_OBJECT (Join_conf, Cancel, "Cancel");

  return Join_conf;
}

GtkWidget*
create_Call_conf (void)
{
  GtkWidget *Call_conf;
  GtkWidget *dialog_vbox2;
  GtkWidget *vbox1;
  GtkWidget *Phone_number;
  GtkWidget *entry1;
  GtkWidget *dialog_action_area2;
  GtkWidget *Call;
  GtkWidget *Cancel;

  Call_conf = gtk_dialog_new ();
  gtk_widget_set_events (Call_conf, GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
  gtk_window_set_position (GTK_WINDOW (Call_conf), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint (GTK_WINDOW (Call_conf), GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_dialog_set_has_separator (GTK_DIALOG (Call_conf), FALSE);

  dialog_vbox2 = GTK_DIALOG (Call_conf)->vbox;
  gtk_widget_show (dialog_vbox2);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox2), vbox1, TRUE, TRUE, 0);

  Phone_number = gtk_label_new (("Enter the phone number to call then press the Call button"));
  gtk_widget_show (Phone_number);
  gtk_box_pack_start (GTK_BOX (vbox1), Phone_number, TRUE, TRUE, 0);

  entry1 = gtk_entry_new ();
  gtk_widget_show (entry1);
  gtk_box_pack_start (GTK_BOX (vbox1), entry1, TRUE, TRUE, 0);
  gtk_widget_set_events (entry1, GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry1), 9679);

  dialog_action_area2 = GTK_DIALOG (Call_conf)->action_area;
  gtk_widget_show (dialog_action_area2);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area2), GTK_BUTTONBOX_END);

  Call = gtk_button_new_with_mnemonic (("Call"));
  gtk_widget_show (Call);
  gtk_dialog_add_action_widget (GTK_DIALOG (Call_conf), Call, 0);

  Cancel = gtk_button_new_with_mnemonic (("Cancel"));
  gtk_widget_show (Cancel);
  gtk_dialog_add_action_widget (GTK_DIALOG (Call_conf), Cancel, 0);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (Call_conf, Call_conf, "Call_conf");
  GLADE_HOOKUP_OBJECT_NO_REF (Call_conf, dialog_vbox2, "dialog_vbox2");
  GLADE_HOOKUP_OBJECT (Call_conf, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (Call_conf, Phone_number, "Phone_number");
  GLADE_HOOKUP_OBJECT (Call_conf, entry1, "entry1");
  GLADE_HOOKUP_OBJECT_NO_REF (Call_conf, dialog_action_area2, "dialog_action_area2");
  GLADE_HOOKUP_OBJECT (Call_conf, Call, "Call");
  GLADE_HOOKUP_OBJECT (Call_conf, Cancel, "Cancel");

  return Call_conf;
}

