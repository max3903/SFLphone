/*
 *  Copyright (C) 2007-2008 Savoir-Faire Linux inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
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

#include <actions.h>
#include <mainwindow.h>
#include <accountlist.h>
#include <accountwindow.h>

// From version 2.16, gtk provides the functionalities libsexy used to provide
#if GTK_CHECK_VERSION(2,16,0)
#else
#include <libsexy/sexy-icon-entry.h>
#endif

#include <string.h>
#include <dbus/dbus.h>
#include <config.h>
#include <gtk/gtk.h>

/** Local variables */
account_t * currentAccount;

GtkDialog * dialog;
GtkWidget * hbox;
GtkWidget * frame;
GtkWidget * table;
GtkWidget * label;
GtkWidget * entryID;
GtkWidget * entryAlias;
GtkWidget * entryProtocol;
GtkWidget * entryUsername;
GtkWidget * entryHostname;
GtkWidget * entryPassword;
GtkWidget * entryMailbox;
GtkWidget * expander;
GtkWidget * entryEnabled;
GtkWidget * tableSRTP;
GtkWidget * enableSRTP;
GtkWidget * keyExchangeCombo;
GtkWidget * enableHelloHash;
GtkWidget * enableSASConfirm;
GtkWidget * enableZrtpNotSuppOther;
GtkWidget * displaySasOnce;

/* Signal to entryProtocol 'changed' */
    void
change_protocol (account_t * currentAccount UNUSED)
{
    (gchar *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(entryProtocol));
}

    void
key_exchange_changed (account_t * currentAccount UNUSED)
{
    gchar *keyExchange = (gchar *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(keyExchangeCombo));
    if(strncmp(keyExchange, "ZRTP", 4) == 0) {
        gtk_widget_set_sensitive( GTK_WIDGET( enableSASConfirm ) , TRUE );
        gtk_widget_set_sensitive( GTK_WIDGET( enableHelloHash ) , TRUE );
    } else {
        gtk_widget_set_sensitive( GTK_WIDGET( enableSASConfirm ) , FALSE );
        gtk_widget_set_sensitive( GTK_WIDGET( enableHelloHash ) , FALSE );
    }
}
    int
is_iax_enabled(void)
{
    int res = dbus_is_iax2_enabled();
    if(res == 1)
        return TRUE;
    else
        return FALSE;
}

    void
show_account_window (account_t * a)
{
    gint response;

    currentAccount = a;

    // Default settings
    gchar * curAccountID = "";
    gchar * curAccountEnabled = "TRUE";
    gchar * curAccountType = "SIP";
    gchar * curAlias = "";
    gchar * curUsername = "";
    gchar * curHostname = "";
    gchar * curPassword = "";
    /* TODO: add curProxy, and add boxes for Proxy support */
    gchar * curMailbox = "";
    gchar * curKeyExchange = ZRTP;
    gchar * curSRTPEnabled = "FALSE";
    gchar * curSasConfirm = "TRUE";
    gchar * curHelloEnabled = "TRUE";
    gchar * curZrtpNotSuppOther = "TRUE";
    gchar * curDisplaySasOnce = "FALSE";

#if GTK_CHECK_VERSION(2,16,0)
#else
    GtkWidget *image;
#endif

    // Load from SIP/IAX/Unknown ?
    if(a)
    {
        curAccountID = a->accountID;
        curAccountType = g_hash_table_lookup(currentAccount->properties, ACCOUNT_TYPE);
        curAccountEnabled = g_hash_table_lookup(currentAccount->properties, ACCOUNT_ENABLED);
        curAlias = g_hash_table_lookup(currentAccount->properties, ACCOUNT_ALIAS);
        curHostname = g_hash_table_lookup(currentAccount->properties, ACCOUNT_HOSTNAME);
        curPassword = g_hash_table_lookup(currentAccount->properties, ACCOUNT_PASSWORD);
        curUsername = g_hash_table_lookup(currentAccount->properties, ACCOUNT_USERNAME);
        curMailbox = g_hash_table_lookup(currentAccount->properties, ACCOUNT_MAILBOX);
        curKeyExchange = g_hash_table_lookup(currentAccount->properties, ACCOUNT_KEY_EXCHANGE);
        curSRTPEnabled = g_hash_table_lookup(currentAccount->properties, ACCOUNT_SRTP_ENABLED);
        curHelloEnabled = g_hash_table_lookup(currentAccount->properties, ACCOUNT_ZRTP_HELLO_HASH);
        curSasConfirm = g_hash_table_lookup(currentAccount->properties, ACCOUNT_ZRTP_DISPLAY_SAS);
        curZrtpNotSuppOther = g_hash_table_lookup(currentAccount->properties, ACCOUNT_ZRTP_NOT_SUPP_WARNING);
        curDisplaySasOnce = g_hash_table_lookup(currentAccount->properties, ACCOUNT_DISPLAY_SAS_ONCE);
    }
    else
    {
        currentAccount = g_new0(account_t, 1);
        currentAccount->properties = g_hash_table_new(NULL, g_str_equal);
        curAccountID = "new";
    }

    dialog = GTK_DIALOG(gtk_dialog_new_with_buttons (_("Account settings"),
                GTK_WINDOW(get_main_window()),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_STOCK_APPLY,
                GTK_RESPONSE_ACCEPT,
                GTK_STOCK_CANCEL,
                GTK_RESPONSE_CANCEL,
                NULL));
    gtk_window_set_policy( GTK_WINDOW(dialog), FALSE, FALSE, FALSE );
    gtk_dialog_set_has_separator(dialog, TRUE);
    gtk_container_set_border_width (GTK_CONTAINER(dialog), 0);

    gnome_main_section_new (_("Account Parameters"), &frame);
    gtk_box_pack_start(GTK_BOX(dialog->vbox), frame, FALSE, FALSE, 0);
    gtk_widget_show(frame);

    table = gtk_table_new ( 10, 2  ,  FALSE/* homogeneous */);
    gtk_table_set_row_spacings( GTK_TABLE(table), 10);
    gtk_table_set_col_spacings( GTK_TABLE(table), 10);
    gtk_widget_show(table);
    gtk_container_add( GTK_CONTAINER( frame) , table );


#ifdef DEBUG
    label = gtk_label_new_with_mnemonic ("ID:");
    gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
    entryID = gtk_entry_new();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryID);
    gtk_entry_set_text(GTK_ENTRY(entryID), curAccountID);
    gtk_widget_set_sensitive( GTK_WIDGET(entryID), FALSE);
    gtk_table_attach ( GTK_TABLE( table ), entryID, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
#endif

    entryEnabled = gtk_check_button_new_with_mnemonic(_("_Enable this account"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entryEnabled),
            g_strcasecmp(curAccountEnabled,"TRUE") == 0 ? TRUE: FALSE);
    gtk_table_attach ( GTK_TABLE( table ), entryEnabled, 0, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_widget_set_sensitive( GTK_WIDGET( entryEnabled ) , TRUE );
    
    label = gtk_label_new_with_mnemonic (_("_Alias"));
    gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
    entryAlias = gtk_entry_new();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryAlias);
    gtk_entry_set_text(GTK_ENTRY(entryAlias), curAlias);
    gtk_table_attach ( GTK_TABLE( table ), entryAlias, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    label = gtk_label_new_with_mnemonic (_("_Protocol"));
    gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
    entryProtocol = gtk_combo_box_new_text();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryProtocol);
    gtk_combo_box_append_text(GTK_COMBO_BOX(entryProtocol), "SIP");
    if( is_iax_enabled() ) gtk_combo_box_append_text(GTK_COMBO_BOX(entryProtocol), "IAX");
    if(strcmp(curAccountType, "SIP") == 0)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(entryProtocol),0);
    }
    else if(strcmp(curAccountType, "IAX") == 0)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(entryProtocol),1);
    }
    else
    {
        /* Should never come here, add debug message. */
        gtk_combo_box_append_text(GTK_COMBO_BOX(entryProtocol), _("Unknown"));
        gtk_combo_box_set_active(GTK_COMBO_BOX(entryProtocol),2);
    }
    gtk_table_attach ( GTK_TABLE( table ), entryProtocol, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    /* Link signal 'changed' */
    g_signal_connect (G_OBJECT (GTK_COMBO_BOX(entryProtocol)), "changed",
            G_CALLBACK (change_protocol),
            currentAccount);

    label = gtk_label_new_with_mnemonic (_("_Host name"));
    gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
    entryHostname = gtk_entry_new();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryHostname);
    gtk_entry_set_text(GTK_ENTRY(entryHostname), curHostname);
    gtk_table_attach ( GTK_TABLE( table ), entryHostname, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    label = gtk_label_new_with_mnemonic (_("_User name"));
    gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 6, 7, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
#if GTK_CHECK_VERSION(2,16,0)
	entryUsername = gtk_entry_new();
    gtk_entry_set_icon_from_pixbuf (GTK_ENTRY (entryUsername), GTK_ENTRY_ICON_PRIMARY, gdk_pixbuf_new_from_file(ICONS_DIR "/stock_person.svg", NULL));
#else
    entryUsername = sexy_icon_entry_new();
    image = gtk_image_new_from_file( ICONS_DIR "/stock_person.svg" );
    sexy_icon_entry_set_icon( SEXY_ICON_ENTRY(entryUsername), SEXY_ICON_ENTRY_PRIMARY , GTK_IMAGE(image) );
#endif
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryUsername);
    gtk_entry_set_text(GTK_ENTRY(entryUsername), curUsername);
    gtk_table_attach ( GTK_TABLE( table ), entryUsername, 1, 2, 6, 7, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    label = gtk_label_new_with_mnemonic (_("_Password"));
    gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 7, 8, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
#if GTK_CHECK_VERSION(2,16,0)
	entryPassword = gtk_entry_new();
    gtk_entry_set_icon_from_stock (GTK_ENTRY (entryPassword), GTK_ENTRY_ICON_PRIMARY, GTK_STOCK_DIALOG_AUTHENTICATION);
#else
    entryPassword = sexy_icon_entry_new();
    image = gtk_image_new_from_stock( GTK_STOCK_DIALOG_AUTHENTICATION , GTK_ICON_SIZE_SMALL_TOOLBAR );
    sexy_icon_entry_set_icon( SEXY_ICON_ENTRY(entryPassword), SEXY_ICON_ENTRY_PRIMARY , GTK_IMAGE(image) );
#endif
    gtk_entry_set_visibility(GTK_ENTRY(entryPassword), FALSE);
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryPassword);
    gtk_entry_set_text(GTK_ENTRY(entryPassword), curPassword);
    gtk_table_attach ( GTK_TABLE( table ), entryPassword, 1, 2, 7, 8, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    label = gtk_label_new_with_mnemonic (_("_Voicemail number"));
    gtk_table_attach ( GTK_TABLE( table ), label, 0, 1, 8, 9, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
    entryMailbox = gtk_entry_new();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), entryMailbox);
    gtk_entry_set_text(GTK_ENTRY(entryMailbox), curMailbox);
    gtk_table_attach ( GTK_TABLE( table ), entryMailbox, 1, 2, 8, 9, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    /* SRTP Options */
    label = g_strdup_printf ("<b>%s</b>", _("_SRTP Options"));
    expander = gtk_expander_new_with_mnemonic (label);
    gtk_expander_set_use_markup (GTK_EXPANDER (expander), TRUE);
    gtk_expander_set_spacing (expander, 10);
    gtk_table_attach ( GTK_TABLE( table ), expander, 0, 2, 9, 10, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_widget_show (expander);
    
    tableSRTP = gtk_table_new ( 6, 2  ,  FALSE/* homogeneous */);
    gtk_table_set_row_spacings( GTK_TABLE(tableSRTP), 10);
    gtk_table_set_col_spacings( GTK_TABLE(tableSRTP), 10);
    gtk_widget_show(tableSRTP);
    gtk_container_add( GTK_EXPANDER(expander) , tableSRTP );

    enableSRTP = gtk_check_button_new_with_mnemonic(_("Use _SRTP"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(enableSRTP),
            g_strcasecmp(curSRTPEnabled,"TRUE") == 0 ? TRUE: FALSE);
    gtk_expander_set_expanded(GTK_EXPANDER(expander),g_strcasecmp(curSRTPEnabled,"TRUE") == 0 ? TRUE: FALSE );
    
    gtk_table_attach ( GTK_TABLE(tableSRTP), enableSRTP, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_widget_set_sensitive( GTK_WIDGET( enableSRTP ) , TRUE );
                 
    label = gtk_label_new_with_mnemonic (_("_Key Exchange"));
    gtk_table_attach ( GTK_TABLE( tableSRTP ), label, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_misc_set_alignment(GTK_MISC (label), 0, 0.5);
    keyExchangeCombo = gtk_combo_box_new_text();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), keyExchangeCombo);
    gtk_combo_box_append_text(GTK_COMBO_BOX(keyExchangeCombo), "ZRTP");
    //gtk_combo_box_append_text(GTK_COMBO_BOX(keyExchangeCombo), "SDES");
        
    if(strcmp(curKeyExchange, ZRTP) == 0)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(keyExchangeCombo),0);
    }
    else if(strcmp(curKeyExchange, SDES_TLS) == 0)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(keyExchangeCombo),1);
    }
    else
    {
        gtk_combo_box_append_text(GTK_COMBO_BOX(keyExchangeCombo), _("Unknown"));
        gtk_combo_box_set_active(GTK_COMBO_BOX(keyExchangeCombo),2);
    }
    //gtk_widget_set_size_request(GTK_WIDGET(keyExchangeCombo), 225, -1);
    gtk_table_attach ( GTK_TABLE( tableSRTP ), keyExchangeCombo, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);    
        
    enableHelloHash = gtk_check_button_new_with_mnemonic(_("Send Hello Hash in S_DP"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(enableHelloHash),
            g_strcasecmp(curHelloEnabled,"TRUE") == 0 ? TRUE: FALSE);
    gtk_table_attach ( GTK_TABLE(tableSRTP), enableHelloHash, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_widget_set_sensitive( GTK_WIDGET( enableHelloHash ) , TRUE );
        
    enableSASConfirm = gtk_check_button_new_with_mnemonic(_("Ask User to Confirm SAS"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(enableSASConfirm),
            g_strcasecmp(curSasConfirm,"TRUE") == 0 ? TRUE: FALSE);
    gtk_table_attach ( GTK_TABLE(tableSRTP), enableSASConfirm, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_widget_set_sensitive( GTK_WIDGET( enableSASConfirm ) , TRUE ); 
  
    enableZrtpNotSuppOther = gtk_check_button_new_with_mnemonic(_("_Warn if ZRTP not supported"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(enableZrtpNotSuppOther),
            g_strcasecmp(curZrtpNotSuppOther,"TRUE") == 0 ? TRUE: FALSE);
    gtk_table_attach ( GTK_TABLE(tableSRTP), enableZrtpNotSuppOther, 0, 1, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_widget_set_sensitive( GTK_WIDGET( enableZrtpNotSuppOther ) , TRUE );
  
    displaySasOnce = gtk_check_button_new_with_mnemonic(_("Display SAS once for hold events"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(displaySasOnce),
            g_strcasecmp(curDisplaySasOnce,"TRUE") == 0 ? TRUE: FALSE);
    gtk_table_attach ( GTK_TABLE(tableSRTP), displaySasOnce, 0, 1, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_widget_set_sensitive( GTK_WIDGET( displaySasOnce ) , TRUE );
          
    /* Link signal 'changed' */
    g_signal_connect (G_OBJECT (GTK_COMBO_BOX(keyExchangeCombo)), "changed",
            G_CALLBACK (key_exchange_changed),
            currentAccount);
            
    /* Display main table */
    gtk_widget_show_all( table );
    gtk_container_set_border_width (GTK_CONTAINER(table), 10);

    response = gtk_dialog_run (GTK_DIALOG (dialog));
    if(response == GTK_RESPONSE_ACCEPT)
    {
        gchar* proto = (gchar *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(entryProtocol));

        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_ENABLED),
                g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(entryEnabled)) ? "TRUE": "FALSE"));
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_ALIAS),
                g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryAlias))));
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_TYPE),
                g_strdup(proto));
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_HOSTNAME),
                g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryHostname))));
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_USERNAME),
                g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryUsername))));
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_PASSWORD),
                g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryPassword))));
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_MAILBOX),
                g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entryMailbox))));
                
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_SRTP_ENABLED),
                g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(enableSRTP)) ? "TRUE": "FALSE"));
                
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_ZRTP_DISPLAY_SAS),
                g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(enableSASConfirm)) ? "TRUE": "FALSE"));   
                
         g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_DISPLAY_SAS_ONCE),
                g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(displaySasOnce)) ? "TRUE": "FALSE")); 
                
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_ZRTP_HELLO_HASH),
                g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(enableHelloHash)) ? "TRUE": "FALSE"));
                
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_ZRTP_NOT_SUPP_WARNING),
                g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(enableZrtpNotSuppOther)) ? "TRUE": "FALSE"));                
                
        g_hash_table_replace(currentAccount->properties,
                g_strdup(ACCOUNT_KEY_EXCHANGE),
                (gchar *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(keyExchangeCombo)));
                
        if (strcmp(proto, "SIP") == 0) {
            guint i, size;
            account_t * account;
            gchar * stun_srv;
            gchar * stun_enable;
            gboolean flag = FALSE;

            size = account_list_get_size();
            // If a SIP account already exists, fetch its information about STUN
            for(i=0; i<size; i++){
                account = account_list_get_nth(i);
                if( strcmp(g_hash_table_lookup(account->properties, ACCOUNT_TYPE), "SIP" ) == 0 )
                {
                    stun_srv = g_hash_table_lookup(account->properties, ACCOUNT_SIP_STUN_SERVER);
                    stun_enable = g_hash_table_lookup(account->properties, ACCOUNT_SIP_STUN_ENABLED);
                    g_hash_table_replace(currentAccount->properties, g_strdup(ACCOUNT_SIP_STUN_SERVER),
                                            g_strdup(stun_srv));
                    g_hash_table_replace(currentAccount->properties, g_strdup(ACCOUNT_SIP_STUN_ENABLED),
                                            g_strdup(stun_enable));
                    flag = TRUE;
                    break;
                }
            }

            // Otherelse set a default value
            if(!flag)
            {
                g_hash_table_replace(currentAccount->properties, g_strdup(ACCOUNT_SIP_STUN_SERVER), (gchar*)"");
                g_hash_table_replace(currentAccount->properties, g_strdup(ACCOUNT_SIP_STUN_ENABLED),
                                            "FALSE");
            }

            config_window_set_stun_visible();
        }

        /** @todo Verify if it's the best condition to check */
        if (currentAccount->accountID == NULL) {
            dbus_add_account(currentAccount);
        }
        else {
            dbus_set_account_details(currentAccount);
        }
    }
    gtk_widget_destroy (GTK_WIDGET(dialog));
}
