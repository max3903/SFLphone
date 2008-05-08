/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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
#include <gtk/gtk.h>
#include <actions.h>
#include <calltree.h>
#include <calllist.h>
#include <menus.h>
#include <dbus.h>
#include <mainwindow.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <unistd.h>

GtkWidget   * toolbar;
GtkToolItem * pickupButton;
GtkToolItem * callButton;
GtkToolItem * hangupButton;
GtkToolItem * holdButton;
GtkToolItem * transfertButton;
GtkToolItem * unholdButton;
GtkToolItem * inviteButton;
GtkToolItem * historyButton;
GtkToolItem * mailboxButton;
GtkToolItem * webcamButton;

guint transfertButtonConnId; //The button toggled signal connection ID
gboolean history_shown;

//Conference dialogs
GtkDialog *inviteDialog;
GtkDialog *joinDialog;

//webcam enable/disable dialogs
GtkDialog * enableDialog;
GtkDialog * disableDialog;
//The second call to make a conference
call_t * callConf;

/**
 * Show popup menu
 */
  static gboolean
popup_menu (GtkWidget *widget, gpointer user_data)
{
  show_popup_menu(widget, NULL);
  return TRUE;
}

  static gboolean
button_pressed(GtkWidget* widget, GdkEventButton *event, gpointer user_data)
{
  if (event->button == 3 && event->type == GDK_BUTTON_PRESS)
  {
    show_popup_menu(widget,  event);
    return TRUE;
  }
  return FALSE;
}

/**
 * Make a call
 */
  static void
call_button(GtkWidget *widget, gpointer data)
{
  call_t * selectedCall;
  printf("Call button pressed\n");
  if(call_list_get_size(current_calls)>0)
    sflphone_pick_up();
  else if(call_list_get_size(active_calltree) > 0)
  {
    printf("Calling a called num\n");
    selectedCall = call_get_selected(active_calltree);
    if(!selectedCall->to){
      selectedCall->to = call_get_number(selectedCall);
      selectedCall->from = g_strconcat("\"\" <", selectedCall->to, ">",NULL);
    }
    gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(historyButton), FALSE);
    printf("call : from : %s to %s\n", selectedCall->from, selectedCall->to);
    call_list_add(current_calls, selectedCall);
    update_call_tree_add(current_calls, selectedCall);
    sflphone_place_call(selectedCall);
  }else
    sflphone_new_call();
}

/**
 * Hang up the line
 */
  static void 
hang_up( GtkWidget *widget, gpointer   data )
{
  sflphone_hang_up();
}

/**
 * Hold the line
 */
  static void 
hold( GtkWidget *widget, gpointer   data )
{
  sflphone_on_hold();
}

/**
 * Transfert the line
 */
  static void 
transfert  (GtkToggleToolButton *toggle_tool_button,
    gpointer             user_data)
{
  gboolean up = gtk_toggle_tool_button_get_active(toggle_tool_button);
  if(up)
  {
    sflphone_set_transfert();
  }
  else
  {
    sflphone_unset_transfert();
  }
}

/**
 * Unhold call
 */
  static void 
unhold( GtkWidget *widget, gpointer   data )
{
  sflphone_off_hold();
}

/**
 * Enable/Disable webcam
 */
static void webcamStatusChange( GtkWidget *widget, gpointer data )
{
  g_print("Changing webcam status ...\n");
  call_t * selectedCall = call_get_selected(current_calls);

  if( selectedCall)
  {
    if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON (webcamButton)))    //If we are enabling the webcam
    {
      main_window_glWidget(TRUE);
      sflphone_set_video();
    }
    else  //If we are disabling the webcam
    {
      main_window_glWidget(FALSE);
      sflphone_unset_video();
    }
  }
}

  static void
enable_yes_button(GtkButton *button, gpointer user_data)
{
  gtk_dialog_response(enableDialog, GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(GTK_WIDGET(enableDialog));

  main_window_glWidget(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON (webcamButton)));

  sflphone_set_video();
  g_print("Info: enabling webcam");	
}

  static void
enable_no_button(GtkButton *button, gpointer user_data)
{
  gtk_dialog_response(enableDialog, GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(GTK_WIDGET(enableDialog));
}

  static void
disable_yes_button(GtkButton *button, gpointer user_data)
{
  gtk_dialog_response(disableDialog, GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(GTK_WIDGET(disableDialog));
  main_window_glWidget(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON (webcamButton)));

  sflphone_unset_video();	
  g_print("Info: disabling webcam");
}

  static void
disable_no_button(GtkButton *button, gpointer user_data)
{
  gtk_dialog_response(disableDialog, GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(GTK_WIDGET(disableDialog));
}

  static void
enable_checkbox(GtkToggleButton *togglebutton, gpointer user_data)
{
  gboolean status = gtk_toggle_button_get_active(togglebutton);
  set_enable_webcam_checkbox_status(!status);
}

  static void
disable_checkbox(GtkToggleButton *togglebutton, gpointer user_data)
{
  gboolean status = gtk_toggle_button_get_active(togglebutton);
  set_disable_webcam_checkbox_status(!status);
}

/**
 * Dialog window on webcam activation
 */
  void
create_enable_webcam_window()
{
  GtkWidget *enableVBox;
  GtkWidget *enableLabel;
  GtkWidget *enableYesButton;
  GtkWidget *enableNoButton;
  GtkWidget *enableCheckBox;

  enableDialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Enable webcam",
	GTK_WINDOW(get_main_window()),
	GTK_DIALOG_DESTROY_WITH_PARENT,
	NULL));

  gtk_dialog_set_has_separator(enableDialog, FALSE);
  gtk_window_set_default_size(GTK_WINDOW(enableDialog), 100, 100);
  gtk_container_set_border_width(GTK_CONTAINER(enableDialog), 0);

  enableVBox = GTK_DIALOG (enableDialog)->vbox;
  gtk_widget_show (enableVBox);

  enableLabel = gtk_label_new (("Do you want to enable webcam capture?"));
  gtk_widget_show (enableLabel);
  gtk_box_pack_start (GTK_BOX (enableVBox), enableLabel, TRUE, TRUE, 0);

  enableCheckBox = gtk_check_button_new_with_label("Don't show this dialog again");
  gtk_box_pack_start(GTK_BOX(enableVBox), enableCheckBox, FALSE, FALSE, 0);
  gtk_widget_show(enableCheckBox);

  enableYesButton = gtk_button_new_with_mnemonic (("Yes"));
  gtk_widget_show (enableYesButton);
  gtk_dialog_add_action_widget (GTK_DIALOG (enableDialog), enableYesButton, 0);

  enableNoButton = gtk_button_new_with_mnemonic (("No"));
  gtk_widget_show (enableNoButton);
  gtk_dialog_add_action_widget (GTK_DIALOG (enableDialog), enableNoButton, 0);

  g_signal_connect(G_OBJECT(enableYesButton), "clicked", G_CALLBACK (enable_yes_button), NULL);
  g_signal_connect(G_OBJECT(enableNoButton), "clicked", G_CALLBACK (enable_no_button), NULL);
  g_signal_connect(G_OBJECT(enableCheckBox), "toggled", G_CALLBACK (enable_checkbox), NULL);

  gtk_dialog_run(enableDialog);
}

/**
 * Dialog window on webcam activation
 */
  void
create_disable_webcam_window()
{
  GtkWidget *disableVBox;
  GtkWidget *disableLabel;
  GtkWidget *disableYesButton;
  GtkWidget *disableNoButton;
  GtkWidget *disableCheckBox;

  disableDialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Enable webcam",
	GTK_WINDOW(get_main_window()),
	GTK_DIALOG_DESTROY_WITH_PARENT,
	NULL));

  gtk_dialog_set_has_separator(disableDialog, FALSE);
  gtk_window_set_default_size(GTK_WINDOW(disableDialog), 100, 100);
  gtk_container_set_border_width(GTK_CONTAINER(disableDialog), 0);

  disableVBox = GTK_DIALOG (disableDialog)->vbox;
  gtk_widget_show (disableVBox);

  disableLabel = gtk_label_new (("Do you want to disable webcam capture?"));
  gtk_widget_show (disableLabel);
  gtk_box_pack_start (GTK_BOX (disableVBox), disableLabel, TRUE, TRUE, 0);

  disableCheckBox = gtk_check_button_new_with_label("Don't show this dialog again");
  gtk_box_pack_start(GTK_BOX(disableVBox), disableCheckBox, FALSE, FALSE, 0);
  gtk_widget_show(disableCheckBox);

  disableYesButton = gtk_button_new_with_mnemonic (("Yes"));
  gtk_widget_show (disableYesButton);
  gtk_dialog_add_action_widget (GTK_DIALOG (disableDialog), disableYesButton, 0);

  disableNoButton = gtk_button_new_with_mnemonic (("No"));
  gtk_widget_show (disableNoButton);
  gtk_dialog_add_action_widget (GTK_DIALOG (disableDialog), disableNoButton, 0);

  g_signal_connect(G_OBJECT(disableYesButton), "clicked", G_CALLBACK (disable_yes_button), NULL);
  g_signal_connect(G_OBJECT(disableNoButton), "clicked", G_CALLBACK (disable_no_button), NULL);
  g_signal_connect(G_OBJECT(disableCheckBox), "toggled", G_CALLBACK (disable_checkbox), NULL);

  gtk_dialog_run(disableDialog);
}

/**
 * Invite 3rd person to make a conference call
 */
  static void
inviteUser( GtkWidget *widget, gpointer data )
{
  call_t * selectedCall = call_get_selected(current_calls);
  if(selectedCall)
  {
    if(selectedCall->state == CALL_STATE_CURRENT)
    {
      dbus_hold(selectedCall);
      create_invite_window();
    }
  }

}

  static void
invite_call_button(GtkButton *button, gpointer user_data)
{
  char buf[20];
  gboolean answer;

  //Initialize call struct
  callConf = g_new0 (call_t, 1);
  strcpy(buf, gtk_entry_get_text(user_data));
  printf("buffer: %s \n", buf);
  callConf->state = CALL_STATE_DIALING;
  callConf->from = g_strconcat("\"\" <>", NULL);
  callConf->callID = g_new0(gchar, 30);
  g_sprintf(callConf->callID, "%d", rand()); 
  callConf->to = g_strdup(buf);

  //TODO: Place a new call

  gtk_dialog_response(inviteDialog, GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(GTK_WIDGET(inviteDialog));

  //Wait for positive answer then show the join dialog

  if(answer)
  {
    create_join_window();
  }
  else
  {
    //TODO: popup an error window
    call_t * selectedCall = call_get_selected(current_calls);
    if(selectedCall)
    {
      if(selectedCall->state == CALL_STATE_HOLD)
      {
	dbus_unhold(selectedCall);
      }
    }
    printf("connexion not established \n");	
  }
}

  static void
invite_cancel_button(GtkButton *button, gpointer user_data)
{
  call_t * selectedCall = call_get_selected(current_calls);
  if(selectedCall)
  {
    if(selectedCall->state == CALL_STATE_HOLD)
    {
      dbus_unhold(selectedCall);
    }
  }
  gtk_dialog_response(inviteDialog, GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(GTK_WIDGET(inviteDialog));
}

/* Limits the entry to numbers only */
  void
insert_text_handler (GtkEntry *entry, const gchar *text, gint length,
    gint *position, gpointer data)
{
  GtkEditable *editable = GTK_EDITABLE(entry);
  int i, count=0;
  gchar *result = g_new (gchar, length);

  for (i=0; i < length; i++) 
  {
    if (isdigit(text[i]))
      result[count++] = text[i];
  }

  if (count > 0) 
  {
    g_signal_handlers_block_by_func (G_OBJECT (editable),G_CALLBACK (insert_text_handler), data);
    gtk_editable_insert_text (editable, result, count, position);
    g_signal_handlers_unblock_by_func (G_OBJECT (editable), G_CALLBACK (insert_text_handler), data);
  }

  g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");

  g_free (result);
}

  void
create_invite_window()
{
  GtkWidget *dialogVBox;
  GtkWidget *confVBox;
  GtkWidget *phoneLabel;
  GtkWidget *phoneEntry;
  GtkWidget *inviteCallButton;
  GtkWidget *inviteCancelButton;

  inviteDialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Invite user",
	GTK_WINDOW(get_main_window()),
	GTK_DIALOG_DESTROY_WITH_PARENT,
	NULL));

  gtk_dialog_set_has_separator(inviteDialog, FALSE);
  gtk_window_set_default_size(GTK_WINDOW(inviteDialog), 100, 100);
  gtk_container_set_border_width(GTK_CONTAINER(inviteDialog), 0);

  dialogVBox = GTK_DIALOG (inviteDialog)->vbox;
  gtk_widget_show (dialogVBox);

  confVBox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (dialogVBox), confVBox, TRUE, TRUE, 0);
  gtk_widget_show (confVBox);

  phoneLabel = gtk_label_new (("Enter the phone number to call then press the Call button"));
  gtk_widget_show (phoneLabel);
  gtk_box_pack_start (GTK_BOX (confVBox), phoneLabel, TRUE, TRUE, 0);

  phoneEntry = gtk_entry_new ();
  gtk_widget_show (phoneEntry);
  gtk_box_pack_start (GTK_BOX (confVBox), phoneEntry, TRUE, TRUE, 0);

  inviteCallButton = gtk_button_new_with_mnemonic (("Call"));
  gtk_widget_show (inviteCallButton);
  gtk_dialog_add_action_widget (GTK_DIALOG (inviteDialog), inviteCallButton, 0);

  inviteCancelButton = gtk_button_new_with_mnemonic (("Cancel"));
  gtk_widget_show (inviteCancelButton);
  gtk_dialog_add_action_widget (GTK_DIALOG (inviteDialog), inviteCancelButton, 0);

  g_signal_connect(G_OBJECT(inviteCallButton), "clicked", G_CALLBACK (invite_call_button), phoneEntry);
  g_signal_connect(G_OBJECT(inviteCancelButton), "clicked", G_CALLBACK (invite_cancel_button), NULL);
  g_signal_connect(G_OBJECT(phoneEntry), "insert_text", G_CALLBACK(insert_text_handler), NULL);

  gtk_dialog_run(inviteDialog);
}

  static void
join_button(GtkButton *button, gpointer user_data)
{
  call_t * selectedCall = call_get_selected(current_calls);
  if(selectedCall)
  {
    if(selectedCall->state == CALL_STATE_HOLD)
    {
      //Join calls
      if(dbus_join_conference(selectedCall, callConf))
      {
	update_menus();
	//TODO: update calltree icons
      }
    }
    selectedCall->state = CALL_STATE_CONF;
    callConf->state = CALL_STATE_CONF;
  }

  toolbar_update_buttons ();
  gtk_dialog_response(joinDialog, GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(GTK_WIDGET(joinDialog));
}

  static void
join_cancel_button(GtkButton *button, gpointer user_data)
{
  //TODO: hangup call with the callee
  call_t * selectedCall = call_get_selected(current_calls);
  if(selectedCall)
  {
    if(selectedCall->state == CALL_STATE_HOLD)
    {
      dbus_hang_up(callConf);
      dbus_unhold(selectedCall);
    }
  }
  gtk_dialog_response(joinDialog, GTK_RESPONSE_DELETE_EVENT);
  gtk_widget_destroy(GTK_WIDGET(joinDialog));
}

  void
create_join_window()
{
  GtkWidget *joinVBox;
  GtkWidget *joinLabel;
  GtkWidget *joinButton;
  GtkWidget *joinCancelButton;


  joinDialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Invite user",
	GTK_WINDOW(get_main_window()),
	GTK_DIALOG_DESTROY_WITH_PARENT,
	NULL));

  gtk_dialog_set_has_separator(joinDialog, FALSE);
  gtk_window_set_default_size(GTK_WINDOW(joinDialog), 100, 100);
  gtk_container_set_border_width(GTK_CONTAINER(joinDialog), 0);

  joinVBox = GTK_DIALOG (joinDialog)->vbox;
  gtk_widget_show (joinVBox);

  joinLabel = gtk_label_new (("The connection has been established \n Press join to start the conference"));
  gtk_widget_show (joinLabel);
  gtk_box_pack_start (GTK_BOX (joinVBox), joinLabel, TRUE, TRUE, 0);

  joinButton = gtk_button_new_with_mnemonic (("Join"));
  gtk_widget_show (joinButton);
  gtk_dialog_add_action_widget (GTK_DIALOG (joinDialog), joinButton, 0);

  joinCancelButton = gtk_button_new_with_mnemonic (("Cancel"));
  gtk_widget_show (joinCancelButton);
  gtk_dialog_add_action_widget (GTK_DIALOG (joinDialog), joinCancelButton, 0);

  g_signal_connect(G_OBJECT(joinButton), "clicked", G_CALLBACK (join_button), NULL);
  g_signal_connect(G_OBJECT(joinCancelButton), "clicked", G_CALLBACK (join_cancel_button), NULL);

  gtk_dialog_run(joinDialog);
}

  static void
toggle_history(GtkToggleToolButton *toggle_tool_button, gpointer user_data)
{
  GtkTreeSelection *sel;
  if(history_shown){
    active_calltree = current_calls;
    gtk_widget_hide(history->tree);
    gtk_widget_show(current_calls->tree);
    history_shown = FALSE;
  }else{
    active_calltree = history;
    gtk_widget_hide(current_calls->tree);
    gtk_widget_show(history->tree);
    history_shown = TRUE;
  }
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (active_calltree->view));
  g_signal_emit_by_name(sel, "changed");
  toolbar_update_buttons();

}

  static void
call_mailbox( GtkWidget* widget , gpointer data )
{
  account_t* current = account_list_get_current();
  call_t* mailboxCall = g_new0( call_t , 1);
  mailboxCall->state = CALL_STATE_DIALING;
  mailboxCall->from = g_strconcat("\"\" <>", NULL);
  mailboxCall->callID = g_new0(gchar, 30);
  g_sprintf(mailboxCall->callID, "%d", rand());
  mailboxCall->to = g_strdup(g_hash_table_lookup(current->properties, ACCOUNT_MAILBOX));
  mailboxCall->accountID = g_strdup(current->accountID);
  g_print("TO : %s\n" , mailboxCall->to);
  call_list_add( current_calls , mailboxCall );
  update_call_tree_add( current_calls , mailboxCall );    
  update_menus();
  sflphone_place_call( mailboxCall );
}

  void 
toolbar_update_buttons ()
{
  gtk_widget_set_sensitive( GTK_WIDGET(callButton),       FALSE);
  gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),     FALSE);
  gtk_widget_set_sensitive( GTK_WIDGET(holdButton),       FALSE);
  gtk_widget_set_sensitive( GTK_WIDGET(transfertButton),  FALSE);
  gtk_widget_set_sensitive( GTK_WIDGET(mailboxButton) ,   FALSE);
  gtk_widget_set_sensitive( GTK_WIDGET(unholdButton),     FALSE);
  gtk_widget_set_sensitive( GTK_WIDGET(webcamButton),     FALSE);
  gtk_widget_set_sensitive( GTK_WIDGET(inviteButton),     FALSE);	
  g_object_ref(holdButton);
  g_object_ref(unholdButton);
  gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(holdButton));
  gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(unholdButton));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), holdButton, 3);
  g_object_ref(callButton);
  g_object_ref(pickupButton);
  gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(callButton));
  gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(pickupButton));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), callButton, 0);

  g_object_ref(inviteButton);
  gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(inviteButton));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), inviteButton, -1);

  gtk_signal_handler_block(GTK_OBJECT(transfertButton),transfertButtonConnId);
  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(transfertButton), FALSE);
  gtk_signal_handler_unblock(transfertButton, transfertButtonConnId);

  call_t * selectedCall = call_get_selected(active_calltree);
  if (selectedCall)
  {
    switch(selectedCall->state) 
    {
      case CALL_STATE_INCOMING:
	gtk_widget_set_sensitive( GTK_WIDGET(pickupButton),     TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),	TRUE);
	g_object_ref(callButton);	
	gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(callButton));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), pickupButton, 0);
	break;
      case CALL_STATE_HOLD:
	gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),     TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(unholdButton),     TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(callButton),       TRUE);
	g_object_ref(holdButton);
	gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(holdButton));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), unholdButton, 3);
	break;
      case CALL_STATE_RINGING:
	gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),     TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(callButton),     TRUE);
	break;
      case CALL_STATE_DIALING:
	if( active_calltree != history )  gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),     TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(pickupButton),       TRUE);
	g_object_ref(callButton);
	gtk_container_remove(GTK_CONTAINER(toolbar), GTK_WIDGET(callButton));
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), pickupButton, 0);
	break;
      case CALL_STATE_CURRENT:
	gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),     TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(holdButton),       TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(transfertButton),  TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(callButton),       TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(webcamButton),     TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(inviteButton),     TRUE);
	break;
      case CALL_STATE_BUSY:
      case CALL_STATE_FAILURE:
	gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),     TRUE);
	break; 
      case CALL_STATE_TRANSFERT:
	gtk_signal_handler_block(GTK_OBJECT(transfertButton),transfertButtonConnId);
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(transfertButton), TRUE);
	gtk_signal_handler_unblock(transfertButton, transfertButtonConnId);
	gtk_widget_set_sensitive( GTK_WIDGET(callButton),       TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),     TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(holdButton),       TRUE);
	gtk_widget_set_sensitive( GTK_WIDGET(transfertButton),  TRUE);
	break;
      case CALL_STATE_CONF:
	gtk_widget_set_sensitive( GTK_WIDGET(callButton),       FALSE);
	gtk_widget_set_sensitive( GTK_WIDGET(inviteButton),     FALSE);
	gtk_widget_set_sensitive( GTK_WIDGET(transfertButton),  FALSE);
	gtk_widget_set_sensitive( GTK_WIDGET(holdButton),       FALSE);
	gtk_widget_set_sensitive( GTK_WIDGET(hangupButton),     TRUE);
	break;
      default:
	g_warning("Should not happen!");
	break;
    }
  }
  else 
  {
    if( account_list_get_size() > 0 )
    {
      gtk_widget_set_sensitive( GTK_WIDGET(callButton), TRUE );
      gtk_widget_set_sensitive( GTK_WIDGET(mailboxButton), TRUE );
    }
    else
    {
      gtk_widget_set_sensitive( GTK_WIDGET(callButton), FALSE);
    }
  }
}

/* Call back when the user click on a call in the list */
  static void 
selected(GtkTreeSelection *sel, void* data) 
{
  GtkTreeIter  iter;
  GValue val;
  GtkTreeModel *model = (GtkTreeModel*)active_calltree->store;

  if (! gtk_tree_selection_get_selected (sel, &model, &iter))
    return;

  val.g_type = 0;
  gtk_tree_model_get_value (model, &iter, 2, &val);

  call_select(active_calltree, (call_t*) g_value_get_pointer(&val));
  g_value_unset(&val);

  toolbar_update_buttons();
}

/* A row is activated when it is double clicked */
  void
row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, void *data) 
{
  call_t * selectedCall = call_get_selected(current_calls);
  if (selectedCall)
  {
    switch(selectedCall->state)  
    {
      case CALL_STATE_INCOMING:
	dbus_accept(selectedCall);
	break;
      case CALL_STATE_HOLD:
	dbus_unhold(selectedCall);
	break;
      case CALL_STATE_RINGING:
      case CALL_STATE_CURRENT:
      case CALL_STATE_BUSY:
      case CALL_STATE_FAILURE:
	break;
      case CALL_STATE_DIALING:
	sflphone_place_call (selectedCall);
	break;
      default:
	g_warning("Should not happen!");
	break;
    }
  }
}

  GtkWidget *
create_toolbar()
{
  GtkWidget *ret;
  GtkWidget *image;

  ret = gtk_toolbar_new();
  toolbar = ret;

  gtk_toolbar_set_orientation(GTK_TOOLBAR(ret), GTK_ORIENTATION_HORIZONTAL);
  gtk_toolbar_set_style(GTK_TOOLBAR(ret), GTK_TOOLBAR_ICONS);

  image = gtk_image_new_from_file( ICONS_DIR "/call.svg");
  callButton = gtk_tool_button_new (image, _("Place a call"));
  gtk_widget_set_tooltip_text(GTK_WIDGET(callButton), _("Place a call"));
  g_signal_connect (G_OBJECT (callButton), "clicked",
      G_CALLBACK (call_button), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(callButton), -1);

  image = gtk_image_new_from_file( ICONS_DIR "/accept.svg");
  pickupButton = gtk_tool_button_new(image, _("Pick up"));
  gtk_widget_set_tooltip_text(GTK_WIDGET(pickupButton), _("Pick up"));
  gtk_widget_set_state( GTK_WIDGET(pickupButton), GTK_STATE_INSENSITIVE);
  g_signal_connect(G_OBJECT (pickupButton), "clicked", 
      G_CALLBACK (call_button), NULL);
  gtk_widget_show_all(GTK_WIDGET(pickupButton));

  image = gtk_image_new_from_file( ICONS_DIR "/hang_up.svg");
  hangupButton = gtk_tool_button_new (image, _("Hang up"));
  gtk_widget_set_tooltip_text(GTK_WIDGET(hangupButton), _("Hang up"));
  gtk_widget_set_state( GTK_WIDGET(hangupButton), GTK_STATE_INSENSITIVE);
  g_signal_connect (G_OBJECT (hangupButton), "clicked",
      G_CALLBACK (hang_up), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(hangupButton), -1);  

  image = gtk_image_new_from_file( ICONS_DIR "/unhold.svg");
  unholdButton = gtk_tool_button_new (image, _("Off Hold"));
  gtk_widget_set_tooltip_text(GTK_WIDGET(unholdButton), _("Off Hold"));
  gtk_widget_set_state( GTK_WIDGET(unholdButton), GTK_STATE_INSENSITIVE);
  g_signal_connect (G_OBJECT (unholdButton), "clicked",
      G_CALLBACK (unhold), NULL);
  //gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(unholdButton), -1);
  gtk_widget_show_all(GTK_WIDGET(unholdButton));

  image = gtk_image_new_from_file( ICONS_DIR "/hold.svg");
  holdButton =  gtk_tool_button_new (image, _("On Hold"));
  gtk_widget_set_tooltip_text(GTK_WIDGET(holdButton), _("On Hold"));
  gtk_widget_set_state( GTK_WIDGET(holdButton), GTK_STATE_INSENSITIVE);
  g_signal_connect (G_OBJECT (holdButton), "clicked",
      G_CALLBACK (hold), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(holdButton), -1);

  image = gtk_image_new_from_file( ICONS_DIR "/transfert.svg");
  transfertButton = gtk_toggle_tool_button_new ();
  gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(transfertButton), image);
  gtk_widget_set_tooltip_text(GTK_WIDGET(transfertButton), _("Transfer"));
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(transfertButton), _("Transfer"));
  gtk_widget_set_state( GTK_WIDGET(transfertButton), GTK_STATE_INSENSITIVE);
  transfertButtonConnId = g_signal_connect (G_OBJECT (transfertButton), "toggled",
      G_CALLBACK (transfert), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(transfertButton), -1); 

  image = gtk_image_new_from_file( ICONS_DIR "/history2.svg");
  historyButton = gtk_toggle_tool_button_new();
  gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(historyButton), image);
  gtk_widget_set_tooltip_text(GTK_WIDGET(historyButton), _("History"));
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(historyButton), _("History"));
  g_signal_connect (G_OBJECT (historyButton), "toggled",
      G_CALLBACK (toggle_history), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(historyButton), -1);  
  history_shown = FALSE;
  active_calltree = current_calls;

  image = gtk_image_new_from_file( ICONS_DIR "/mailbox.svg");
  mailboxButton = gtk_tool_button_new( image , _("Voicemail box"));
  gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(mailboxButton), image);
  if( account_list_get_size() ==0 ) gtk_widget_set_state( GTK_WIDGET(mailboxButton), GTK_STATE_INSENSITIVE );
  gtk_widget_set_tooltip_text(GTK_WIDGET(mailboxButton), _("Voicemail box"));
  g_signal_connect (G_OBJECT (mailboxButton), "clicked",
      G_CALLBACK (call_mailbox), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(mailboxButton), -1);

  image = gtk_image_new_from_file(ICONS_DIR "/webcam.svg");
  webcamButton = gtk_toggle_tool_button_new ();
  gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(webcamButton), image);
  gtk_widget_set_tooltip_text(GTK_WIDGET(webcamButton), _("Enable webcam"));
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(webcamButton), _("Enable webcam"));
  gtk_widget_set_state( GTK_WIDGET(webcamButton), GTK_STATE_INSENSITIVE);
  g_signal_connect (G_OBJECT (webcamButton), "toggled",
      G_CALLBACK (webcamStatusChange), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(webcamButton), -1);   

  image = gtk_image_new_from_file( ICONS_DIR "/conf.svg");
  inviteButton =  gtk_tool_button_new (image, "Invite 3dr Person");
  gtk_widget_set_tooltip_text(GTK_WIDGET(inviteButton), "Invite 3rd Person");
  gtk_widget_set_state( GTK_WIDGET(inviteButton), GTK_STATE_INSENSITIVE);
  //g_signal_connect (G_OBJECT (inviteButton), "clicked", G_CALLBACK (inviteUser), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(ret), GTK_TOOL_ITEM(inviteButton), -1);

  return ret;
}

  void
create_call_tree(calltab_t* tab)
{
  GtkWidget *sw;
  GtkCellRenderer *rend;
  GtkTreeViewColumn *col;
  GtkTreeSelection *sel;

  tab->tree = gtk_vbox_new(FALSE, 10); 

  gtk_container_set_border_width (GTK_CONTAINER (tab->tree), 0);

  sw = gtk_scrolled_window_new( NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_IN);

  tab->store = gtk_list_store_new (3, 
      GDK_TYPE_PIXBUF,// Icon 
      G_TYPE_STRING,  // Description
      G_TYPE_POINTER  // Pointer to the Object
      );

  tab->view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(tab->store));
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(tab->view), FALSE);
  g_signal_connect (G_OBJECT (tab->view), "row-activated",
      G_CALLBACK (row_activated),
      NULL);

  // Connect the popup menu
  g_signal_connect (G_OBJECT (tab->view), "popup-menu",
      G_CALLBACK (popup_menu), 
      NULL);
  g_signal_connect (G_OBJECT (tab->view), "button-press-event",
      G_CALLBACK (button_pressed), 
      NULL);

  rend = gtk_cell_renderer_pixbuf_new();
  col = gtk_tree_view_column_new_with_attributes ("Icon",
      rend,
      "pixbuf", 0,
      NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tab->view), col);

  rend = gtk_cell_renderer_text_new();
  col = gtk_tree_view_column_new_with_attributes ("Description",
      rend,
      "markup", 1,
      NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tab->view), col);
  g_object_unref(G_OBJECT(tab->store));
  gtk_container_add(GTK_CONTAINER(sw), tab->view);

  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (tab->view));
  g_signal_connect (G_OBJECT (sel), "changed",
      G_CALLBACK (selected),
      NULL);

  gtk_box_pack_start(GTK_BOX(tab->tree), sw, TRUE, TRUE, 0);

  gtk_widget_show(tab->tree); 

  //toolbar_update_buttons();
}

  void
update_call_tree_remove (calltab_t* tab, call_t * c)
{
  GtkTreeIter iter;
  GValue val;
  call_t * iterCall;
  GtkListStore* store = tab->store;
  //	GtkWidget* view = tab->view;	UNUSED

  int nbChild = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(store), NULL);
  int i;
  for( i = 0; i < nbChild; i++)
  {
    if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(store), &iter, NULL, i))
    {
      val.g_type = 0;
      gtk_tree_model_get_value (GTK_TREE_MODEL(store), &iter, 2, &val);

      iterCall = (call_t*) g_value_get_pointer(&val);
      g_value_unset(&val);

      if(iterCall == c)
      {
	gtk_list_store_remove(store, &iter);
      }
    }
  }
  call_t * selectedCall = call_get_selected(tab);
  if(selectedCall == c)
    call_select(tab, NULL);
  toolbar_update_buttons();
}

  void
update_call_tree (calltab_t* tab, call_t * c)
{
  GdkPixbuf *pixbuf;
  GtkTreeIter iter;
  GValue val;
  call_t * iterCall;
  GtkListStore* store = tab->store;
  //	GtkWidget* view = tab->view;	UNUSED

  int nbChild = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(store), NULL);
  int i;
  for( i = 0; i < nbChild; i++)
  {
    if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(store), &iter, NULL, i))
    {
      val.g_type = 0;
      gtk_tree_model_get_value (GTK_TREE_MODEL(store), &iter, 2, &val);

      iterCall = (call_t*) g_value_get_pointer(&val);
      g_value_unset(&val);

      if(iterCall == c)
      {
	// Existing call in the list
	gchar * markup;
	if(c->state == CALL_STATE_TRANSFERT)
	{
	  markup = g_markup_printf_escaped("<b>%s</b>\n"
	      "%s\n<i>Transfert to:</i> %s",  
	      call_get_name(c), 
	      call_get_number(c), 
	      c->to);
	}
	else
	{
	  markup = g_markup_printf_escaped("<b>%s</b>\n"
	      "%s", 
	      call_get_name(c), 
	      call_get_number(c));
	}

	if( tab == current_calls )
	{
	  switch(c->state)
	  {
	    case CALL_STATE_HOLD:
	      pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/hold.svg", NULL);
	      break;
	    case CALL_STATE_RINGING:
	      pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/ring.svg", NULL);
	      break;
	    case CALL_STATE_CURRENT:
	      pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/current.svg", NULL);
	      break;
	    case CALL_STATE_DIALING:
	      pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/dial.svg", NULL);
	      break;
	    case CALL_STATE_FAILURE:
	      pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/fail.svg", NULL);
	      break;
	    case CALL_STATE_BUSY:
	      pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/busy.svg", NULL);
	      break;
	    case CALL_STATE_TRANSFERT:
	      pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/transfert.svg", NULL);
	      break;
	    default:
	      g_warning("Should not happen!");
	  }
	}
	else
	{
	  switch(c->history_state)
	  {
	    case OUTGOING:
	      g_print("Outgoing state\n");
	      pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/outgoing.svg", NULL);
	      break;
	    case INCOMING:
	      g_print("Incoming state\n");
	      pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/incoming.svg", NULL);
	      break;
	    case MISSED:
	      g_print("Missed state\n");
	      pixbuf = gdk_pixbuf_new_from_file( ICONS_DIR "/missed.svg", NULL);
	      break;
	    default:
	      g_print("No history state\n");  
	      break;
	  }
	}
	//Resize it
	if(pixbuf)
	{
	  if(gdk_pixbuf_get_width(pixbuf) > 32 || gdk_pixbuf_get_height(pixbuf) > 32)
	  {
	    pixbuf =  gdk_pixbuf_scale_simple(pixbuf, 32, 32, GDK_INTERP_BILINEAR);
	  }
	}
	g_print("MARKUP UPDATE = %s\n" , markup );
	gtk_list_store_set(store, &iter,
	    0, pixbuf, // Icon
	    1, markup, // Description
	    -1);

	if (pixbuf != NULL)
	  g_object_unref(G_OBJECT(pixbuf));
      } 
    }
  } 
  toolbar_update_buttons();
}

  void 
update_call_tree_add (calltab_t* tab, call_t * c)
{
  if( tab == history && ( call_list_get_size( tab ) > dbus_get_max_calls() ) )
    return;

  GdkPixbuf *pixbuf;
  GtkTreeIter iter;
  GtkTreeSelection* sel;
  //GtkListStore* store = tab->store;
  //GtkWidget* view = tab->view;

  // New call in the list
  gchar * markup;
  markup = g_markup_printf_escaped("<b>%s</b>\n"
      "%s", 
      call_get_name(c), 
      call_get_number(c));

  gtk_list_store_prepend (tab->store, &iter);

  if( tab == current_calls )
  {
    switch(c->state)
    {
      case CALL_STATE_INCOMING:
	pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/ring.svg", NULL);
	break;
      case CALL_STATE_DIALING:
	pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/dial.svg", NULL);
	break;
      case CALL_STATE_RINGING:
	pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/ring.svg", NULL);
	break;
      default:
	g_warning("Should not happen!");
    }
  }
  else{
    switch(c->history_state)
    {
      case INCOMING:
	pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/incoming.svg", NULL);
	break;
      case OUTGOING:
	pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/outgoing.svg", NULL);
	break;
      case MISSED:
	pixbuf = gdk_pixbuf_new_from_file(ICONS_DIR "/missed.svg", NULL);
	break;
      default:
	g_warning("Should not happen!");
    }
  }

  //Resize it
  if(pixbuf)
  {
    if(gdk_pixbuf_get_width(pixbuf) > 32 || gdk_pixbuf_get_height(pixbuf) > 32)
    {
      pixbuf =  gdk_pixbuf_scale_simple(pixbuf, 32, 32, GDK_INTERP_BILINEAR);
    }
  }
  gtk_list_store_set(tab->store, &iter,
      0, pixbuf, // Icon
      1, markup, // Description
      2, c,      // Pointer
      -1);

  if (pixbuf != NULL)
    g_object_unref(G_OBJECT(pixbuf));

  sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tab->view));
  gtk_tree_selection_select_iter(GTK_TREE_SELECTION(sel), &iter);
  toolbar_update_buttons();
}
