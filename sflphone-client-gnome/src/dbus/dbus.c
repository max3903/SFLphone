/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */
#include <config.h>

#include <callmanager-glue.h>
#include <configurationmanager-glue.h>
#include <videomanager-glue.h>
#include <instance-glue.h>
#include <marshaller.h>

#include <calltab.h>
#include <preferencesdialog.h>
#include <accountlistconfigdialog.h>
#include <mainwindow.h>
#include <sliders.h>
#include <statusicon.h>
#include <assistant.h>

#include <dbus.h>
#include <actions.h>
#include <string.h>

#include <widget/imwidget.h>

#include <eel-gconf-extensions.h>

#define DEFAULT_DBUS_TIMEOUT 30000

#define DBUS_STRUCT_INT_INT (dbus_g_type_get_struct ("GValueArray", G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID))
#define DBUS_AUDIO_CODEC_TYPE (dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UCHAR, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_STRING, G_TYPE_INVALID))
#define DBUS_VIDEO_CODEC_TYPE (dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UCHAR, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_STRING, G_TYPE_INVALID))
#define DBUS_VIDEO_SETTINGS_TYPE (dbus_g_type_get_struct ("GValueArray", dbus_g_type_get_struct ("GValueArray", G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID), dbus_g_type_get_struct ("GValueArray", G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID), G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_INVALID))
#define DBUS_VIDEO_RESOLUTION_TYPE (dbus_g_type_get_struct ("GValueArray", G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID))
#define DBUS_VIDEO_FRAMERATE_TYPE (dbus_g_type_get_struct ("GValueArray", G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID))
#define DBUS_VIDEO_SHM_INFO (dbus_g_type_get_struct ("GValueArray", G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_INVALID))

static DBusGConnection * connection;
static DBusGProxy * callManagerProxy;
static DBusGProxy * configurationManagerProxy;
static DBusGProxy * videoManagerProxy;
static DBusGProxy * instanceProxy;

static void
incoming_call_cb (DBusGProxy *proxy UNUSED, const gchar* accountID,
                  const gchar* callID, const gchar* from, void * foo  UNUSED)
{
    DEBUG ("Incoming call (%s) from %s", callID, from);

    callable_obj_t * c;
    gchar *peer_name, *peer_number;
    // We receive the from field under a formatted way. We want to extract the number and the name of the caller
    peer_name = call_get_peer_name (from);
    peer_number = call_get_peer_number (from);

    DEBUG ("    peer name: %s", peer_name);
    DEBUG ("    peer number: %s", peer_number);

    create_new_call (CALL, CALL_STATE_INCOMING, g_strdup (callID), g_strdup (
                         accountID), peer_name, peer_number, &c);
#if GTK_CHECK_VERSION(2,10,0)
    status_tray_icon_blink (TRUE);
    popup_main_window();
#endif

    set_timestamp (&c->_time_start);
    notify_incoming_call (c);
    sflphone_incoming_call (c);
}

static void
zrtp_negotiation_failed_cb (DBusGProxy *proxy UNUSED, const gchar* callID,
                            const gchar* reason, const gchar* severity, void * foo  UNUSED)
{
    DEBUG ("Zrtp negotiation failed.");
    main_window_zrtp_negotiation_failed (callID, reason, severity);
    callable_obj_t * c = NULL;
    c = calllist_get (current_calls, callID);

    if (c) {
        notify_zrtp_negotiation_failed (c);
    }
}

static void
curent_selected_codec (DBusGProxy *proxy UNUSED, const gchar* callID UNUSED,
                       const gchar* codecName UNUSED, void * foo  UNUSED)
{
}

static void
volume_changed_cb (DBusGProxy *proxy UNUSED, const gchar* device, const gdouble value,
                   void * foo  UNUSED)
{
    DEBUG ("Volume of %s changed to %f.",device, value);
    set_slider (device, value);
}

static void
voice_mail_cb (DBusGProxy *proxy UNUSED, const gchar* accountID, const guint nb,
               void * foo  UNUSED)
{
    DEBUG ("%d Voice mail waiting!",nb);
    sflphone_notify_voice_mail (accountID, nb);
}

static void
incoming_message_cb (DBusGProxy *proxy UNUSED, const gchar* callID UNUSED, const gchar *from, const gchar* msg, void * foo  UNUSED)
{
    DEBUG ("Message \"%s\" from %s!", msg, from);

    callable_obj_t *call = NULL;
    conference_obj_t *conf = NULL;

    // do not display message if instant messaging is disabled
    gboolean instant_messaging_enabled = TRUE;

    if (eel_gconf_key_exists (INSTANT_MESSAGING_ENABLED))
        instant_messaging_enabled = eel_gconf_get_integer (INSTANT_MESSAGING_ENABLED);

    if (!instant_messaging_enabled)
        return;

    // Get the call information. (if this call exist)
    call = calllist_get (current_calls, callID);

    // Get the conference information (if this conference exist)
    conf = conferencelist_get (callID);

    /* First check if the call is valid */
    if (call) {

        /* Make the instant messaging main window pops, add messages only if the main window exist.
           Elsewhere the message is displayed asynchronously*/
        if (im_widget_display ( (IMWidget **) (&call->_im_widget), msg, call->_callID, from))
            im_widget_add_message (IM_WIDGET (call->_im_widget), from, msg, 0);
    } else if (conf) {
        /* Make the instant messaging main window pops, add messages only if the main window exist.
           Elsewhere the message is displayed asynchronously*/
        if (im_widget_display ( (IMWidget **) (&conf->_im_widget), msg, conf->_confID, from))
            im_widget_add_message (IM_WIDGET (conf->_im_widget), from, msg, 0);
    } else {
        ERROR ("Message received, but no recipient found");
    }
}

static void
call_state_cb (DBusGProxy *proxy UNUSED, const gchar* callID, const gchar* state,
               void * foo  UNUSED)
{
    DEBUG ("Call %s state %s",callID, state);
    callable_obj_t * c = calllist_get (current_calls, callID);

    if (c) {
        if (strcmp (state, "HUNGUP") == 0) {
            if (c->_state == CALL_STATE_CURRENT) {
                // peer hung up, the conversation was established, so _stop has been initialized with the current time value
                DEBUG ("call state current");
                set_timestamp (&c->_time_stop);
                calltree_update_call (history, c, NULL);
            }

            stop_notification();
            calltree_update_call (history, c, NULL);
            status_bar_display_account();
            sflphone_hung_up (c);
        } else if (strcmp (state, "UNHOLD_CURRENT") == 0) {
            sflphone_current (c);
        } else if (strcmp (state, "UNHOLD_RECORD") == 0) {
            sflphone_record (c);
        } else if (strcmp (state, "HOLD") == 0) {
            sflphone_hold (c);
        } else if (strcmp (state, "RINGING") == 0) {
            sflphone_ringing (c);
        } else if (strcmp (state, "CURRENT") == 0) {
            sflphone_current (c);
        } else if (strcmp (state, "FAILURE") == 0) {
            sflphone_fail (c);
        } else if (strcmp (state, "BUSY") == 0) {
            sflphone_busy (c);
        }
    } else {
        // The callID is unknow, threat it like a new call
        // If it were an incoming call, we won't be here
        // It means that a new call has been initiated with an other client (cli for instance)
        if (strcmp (state, "RINGING") == 0 || strcmp (state, "CURRENT") == 0) {
            callable_obj_t *new_call;
            GHashTable *call_details;
            gchar *type;

            DEBUG ("New ringing call! accountID: %s", callID);

            // We fetch the details associated to the specified call
            call_details = dbus_get_call_details (callID);
            create_new_call_from_details (callID, call_details, &new_call);

            // Restore the callID to be synchronous with the daemon
            new_call->_callID = g_strdup (callID);
            type = g_hash_table_lookup (call_details, "CALL_TYPE");

            if (g_strcasecmp (type, "0") == 0) {
                // DEBUG("incoming\n");
                new_call->_history_state = INCOMING;
            } else {
                // DEBUG("outgoing\n");
                new_call->_history_state = OUTGOING;
            }

            calllist_add (current_calls, new_call);
            calllist_add (history, new_call);
            calltree_add_call (current_calls, new_call, NULL);
            update_actions();
            calltree_display (current_calls);

            //sflphone_incoming_call (new_call);
        }
    }
}

static void
conference_changed_cb (DBusGProxy *proxy UNUSED, const gchar* confID,
                       const gchar* state, void * foo  UNUSED)
{

    // gchar** part;
    callable_obj_t *call;
    gchar* call_id;

    // sflphone_display_transfer_status("Transfer successfull");
    conference_obj_t* changed_conf = conferencelist_get (confID);
    GSList * part;

    DEBUG ("conference new state %s\n", state);

    if (changed_conf) {
        // remove old conference from calltree
        calltree_remove_conference (current_calls, changed_conf, NULL);

        // update conference state
        if (strcmp (state, "ACTIVE_ATACHED") == 0) {
            changed_conf->_state = CONFERENCE_STATE_ACTIVE_ATACHED;
        } else if (strcmp (state, "ACTIVE_DETACHED") == 0) {
            changed_conf->_state = CONFERENCE_STATE_ACTIVE_DETACHED;
        } else if (strcmp (state, "HOLD") == 0) {
            changed_conf->_state = CONFERENCE_STATE_HOLD;
        } else {
            DEBUG ("Error: conference state not recognized");
        }

        // reactivate instant messaging window for these calls
        part = changed_conf->participant_list;

        while (part) {
            call_id = (gchar*) (part->data);
            call = calllist_get (current_calls, call_id);

            if (call && call->_im_widget)
                im_widget_update_state (IM_WIDGET (call->_im_widget), TRUE);

            part = g_slist_next (part);
        }

//        new_participants = (gchar **) dbus_get_participant_list (changed_conf->_confID);

        // update conferece participants
        conference_participant_list_update (dbus_get_participant_list (changed_conf->_confID), changed_conf);

        // deactivate instant messaging window for new participants
        part = changed_conf->participant_list;

        while (part) {
            call_id = (gchar*) (part->data);
            call = calllist_get (current_calls, call_id);

            if (call && call->_im_widget)
                im_widget_update_state (IM_WIDGET (call->_im_widget), FALSE);

            part = g_slist_next (part);
        }

        // add new conference to calltree
        calltree_add_conference (current_calls, changed_conf);
    }
}

static void
conference_created_cb (DBusGProxy *proxy UNUSED, const gchar* confID, void * foo  UNUSED)
{
    DEBUG ("DBUS: Conference %s added", confID);

    conference_obj_t* new_conf;
    callable_obj_t* call;
    gchar* call_id;
    gchar** participants;
    gchar** part;

    create_new_conference (CONFERENCE_STATE_ACTIVE_ATACHED, confID, &new_conf);

    participants = (gchar**) dbus_get_participant_list (new_conf->_confID);

    // Update conference list
    conference_participant_list_update (participants, new_conf);

    // Add conference ID in in each calls
    for (part = participants; *part; part++) {
        call_id = (gchar*) (*part);
        call = calllist_get (current_calls, call_id);

        // if a text widget is already created, disable it, use conference widget instead
        if (call->_im_widget)
            im_widget_update_state (IM_WIDGET (call->_im_widget), FALSE);

        call->_confID = g_strdup (confID);
    }

    conferencelist_add (new_conf);
    calltree_add_conference (current_calls, new_conf);
}

static void
conference_removed_cb (DBusGProxy *proxy UNUSED, const gchar* confID, void * foo  UNUSED)
{
    DEBUG ("DBUS: Conference removed %s", confID);

    conference_obj_t * c = conferencelist_get (confID);
    calltree_remove_conference (current_calls, c, NULL);

    GSList *participant = c->participant_list;
    callable_obj_t *call;

    // deactivate instant messaging window for this conference
    if (c->_im_widget)
        im_widget_update_state (IM_WIDGET (c->_im_widget), FALSE);

    // remove all participant for this conference
    while (participant) {

        call = calllist_get (current_calls, (const gchar *) (participant->data));

        if (call) {
            DEBUG ("DBUS: Remove participant %s", call->_callID);

            if (call->_confID) {
                g_free (call->_confID);
                call->_confID = NULL;
            }

            // if an instant messaging was previously disabled, enabled it
            if (call->_im_widget)
                im_widget_update_state (IM_WIDGET (call->_im_widget), TRUE);
        }

        participant = conference_next_participant (participant);
    }

    conferencelist_remove (c->_confID);
}

static void
accounts_changed_cb (DBusGProxy *proxy UNUSED, void * foo  UNUSED)
{
    DEBUG ("Dbus: Accounts changed");

    sflphone_fill_account_list();
    // sflphone_fill_ip2ip_profile();
    account_list_config_dialog_fill();

    // Update the status bar in case something happened
    // Should fix ticket #1215
    status_bar_display_account();

    // Update the tooltip on the status icon
    statusicon_set_tooltip ();
}

static void
transfer_succeded_cb (DBusGProxy *proxy UNUSED, void * foo  UNUSED)
{
    DEBUG ("Dbus: Transfer succeded");
    sflphone_display_transfer_status ("Transfer successfull");
}

static void
transfer_failed_cb (DBusGProxy *proxy UNUSED, void * foo  UNUSED)
{
    DEBUG ("Transfer failed\n");
    sflphone_display_transfer_status ("Transfer failed");
}

static void
secure_sdes_on_cb (DBusGProxy *proxy UNUSED, const gchar *callID, void *foo UNUSED)
{
    DEBUG ("SRTP using SDES is on");
    callable_obj_t *c = calllist_get (current_calls, callID);

    if (c) {
        sflphone_srtp_sdes_on (c);
        notify_secure_on (c);
    }

}

static void
secure_sdes_off_cb (DBusGProxy *proxy UNUSED, const gchar *callID, void *foo UNUSED)
{
    DEBUG ("SRTP using SDES is off");
    callable_obj_t *c = calllist_get (current_calls, callID);

    if (c) {
        sflphone_srtp_sdes_off (c);
        notify_secure_off (c);
    }
}

static void
secure_zrtp_on_cb (DBusGProxy *proxy UNUSED, const gchar* callID, const gchar* cipher,
                   void * foo  UNUSED)
{
    DEBUG ("SRTP using ZRTP is ON secure_on_cb");
    callable_obj_t * c = calllist_get (current_calls, callID);

    if (c) {
        c->_srtp_cipher = g_strdup (cipher);

        sflphone_srtp_zrtp_on (c);
        notify_secure_on (c);
    }
}

static void
secure_zrtp_off_cb (DBusGProxy *proxy UNUSED, const gchar* callID, void * foo  UNUSED)
{
    DEBUG ("SRTP using ZRTP is OFF");
    callable_obj_t * c = calllist_get (current_calls, callID);

    if (c) {
        sflphone_srtp_zrtp_off (c);
        notify_secure_off (c);
    }
}

static void
show_zrtp_sas_cb (DBusGProxy *proxy UNUSED, const gchar* callID, const gchar* sas,
                  const gboolean verified, void * foo  UNUSED)
{
    DEBUG ("Showing SAS");
    callable_obj_t * c = calllist_get (current_calls, callID);

    if (c) {
        sflphone_srtp_zrtp_show_sas (c, sas, verified);
    }
}

static void
confirm_go_clear_cb (DBusGProxy *proxy UNUSED, const gchar* callID, void * foo  UNUSED)
{
    DEBUG ("Confirm Go Clear request");
    callable_obj_t * c = calllist_get (current_calls, callID);

    if (c) {
        sflphone_confirm_go_clear (c);
    }
}

static void
zrtp_not_supported_cb (DBusGProxy *proxy UNUSED, const gchar* callID, void * foo  UNUSED)
{
    DEBUG ("ZRTP not supported on the other end");
    callable_obj_t * c = calllist_get (current_calls, callID);

    if (c) {
        sflphone_srtp_zrtp_not_supported (c);
        notify_zrtp_not_supported (c);
    }
}

static void
sip_call_state_cb (DBusGProxy *proxy UNUSED, const gchar* callID,
                   const gchar* description, const guint code, void * foo  UNUSED)
{
    callable_obj_t * c = NULL;
    c = calllist_get (current_calls, callID);

    if (c != NULL) {
        DEBUG ("sip_call_state_cb received code %d", code);
        sflphone_call_state_changed (c, description, code);
    }
}

static void
error_alert (DBusGProxy *proxy UNUSED, int errCode, void * foo  UNUSED)
{
    ERROR ("Error notifying : (%i)", errCode);
    sflphone_throw_exception (errCode);
}

/**
 * Connect to the configuration manager.
 * @param connection The Dbus connection.
 */
static DBusGProxy *
connect_to_configuration_manager (DBusGConnection * connection)
{
    DBusGProxy * configurationProxy = dbus_g_proxy_new_for_name (connection,
                                      "org.sflphone.SFLphone", "/org/sflphone/SFLphone/ConfigurationManager",
                                      "org.sflphone.SFLphone.ConfigurationManager");

    if (!configurationProxy) {
        ERROR ("Failed to get proxy to ConfigurationManager");
        return NULL;
    }

    DEBUG ("DBus connected to ConfigurationManager");

    dbus_g_proxy_add_signal (configurationProxy, "accountsChanged",
                             G_TYPE_INVALID);

    dbus_g_proxy_connect_signal (configurationProxy, "accountsChanged",
                                 G_CALLBACK (accounts_changed_cb), NULL, NULL);

    dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__INT,
                                       G_TYPE_NONE, G_TYPE_INT, G_TYPE_INVALID);

    dbus_g_proxy_add_signal (configurationProxy, "errorAlert", G_TYPE_INT,
                             G_TYPE_INVALID);

    dbus_g_proxy_connect_signal (configurationProxy, "errorAlert",
                                 G_CALLBACK (error_alert), NULL, NULL);

    return configurationProxy;
}


/**
 * Connect to the call manager.
 * @param connection The Dbus connection.
 */
static DBusGProxy *
connect_to_call_manager (DBusGConnection * connection)
{
    DBusGProxy * callProxy = dbus_g_proxy_new_for_name (connection,
                             "org.sflphone.SFLphone", "/org/sflphone/SFLphone/CallManager",
                             "org.sflphone.SFLphone.CallManager");

    if (callProxy == NULL) {
        ERROR ("Failed to get proxy to CallManager");
        return NULL;
    }

    DEBUG ("DBus connected to CallManager");

    /* Incoming call */
    dbus_g_object_register_marshaller (
        g_cclosure_user_marshal_VOID__STRING_STRING_STRING, G_TYPE_NONE,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "incomingCall", G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "incomingCall",
                                 G_CALLBACK (incoming_call_cb), NULL, NULL);

    dbus_g_proxy_add_signal (callProxy, "zrtpNegotiationFailed", G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "zrtpNegotiationFailed",
                                 G_CALLBACK (zrtp_negotiation_failed_cb), NULL, NULL);

    /* Current codec */
    dbus_g_object_register_marshaller (
        g_cclosure_user_marshal_VOID__STRING_STRING_STRING, G_TYPE_NONE,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "currentSelectedCodec", G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "currentSelectedCodec",
                                 G_CALLBACK (curent_selected_codec), NULL, NULL);

    /* Register a marshaller for STRING,STRING */
    dbus_g_object_register_marshaller (
        g_cclosure_user_marshal_VOID__STRING_STRING, G_TYPE_NONE, G_TYPE_STRING,
        G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "callStateChanged", G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "callStateChanged",
                                 G_CALLBACK (call_state_cb), NULL, NULL);

    dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__STRING_INT,
                                       G_TYPE_NONE, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "voiceMailNotify", G_TYPE_STRING,
                             G_TYPE_INT, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "voiceMailNotify",
                                 G_CALLBACK (voice_mail_cb), NULL, NULL);

    dbus_g_proxy_add_signal (callProxy, "incomingMessage", G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "incomingMessage",
                                 G_CALLBACK (incoming_message_cb), NULL, NULL);

    dbus_g_object_register_marshaller (
        g_cclosure_user_marshal_VOID__STRING_DOUBLE, G_TYPE_NONE, G_TYPE_STRING,
        G_TYPE_DOUBLE, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "volumeChanged", G_TYPE_STRING,
                             G_TYPE_DOUBLE, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "volumeChanged",
                                 G_CALLBACK (volume_changed_cb), NULL, NULL);

    dbus_g_proxy_add_signal (callProxy, "transferSucceded", G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "transferSucceded",
                                 G_CALLBACK (transfer_succeded_cb), NULL, NULL);

    dbus_g_proxy_add_signal (callProxy, "transferFailed", G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "transferFailed",
                                 G_CALLBACK (transfer_failed_cb), NULL, NULL);

    /* Conference related callback */

    dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__STRING,
                                       G_TYPE_NONE, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "conferenceChanged", G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "conferenceChanged",
                                 G_CALLBACK (conference_changed_cb), NULL, NULL);

    dbus_g_proxy_add_signal (callProxy, "conferenceCreated", G_TYPE_STRING,
                             G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "conferenceCreated",
                                 G_CALLBACK (conference_created_cb), NULL, NULL);

    dbus_g_proxy_add_signal (callProxy, "conferenceRemoved", G_TYPE_STRING,
                             G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "conferenceRemoved",
                                 G_CALLBACK (conference_removed_cb), NULL, NULL);

    /* Security related callbacks */

    dbus_g_proxy_add_signal (callProxy, "secureSdesOn", G_TYPE_STRING,
                             G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "secureSdesOn",
                                 G_CALLBACK (secure_sdes_on_cb), NULL, NULL);
    dbus_g_proxy_add_signal (callProxy, "secureSdesOff", G_TYPE_STRING,
                             G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "secureSdesOff",
                                 G_CALLBACK (secure_sdes_off_cb), NULL, NULL);

    /* Register a marshaller for STRING,STRING,BOOL */
    dbus_g_object_register_marshaller (
        g_cclosure_user_marshal_VOID__STRING_STRING_BOOL, G_TYPE_NONE,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "showSAS", G_TYPE_STRING, G_TYPE_STRING,
                             G_TYPE_BOOLEAN, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "showSAS",
                                 G_CALLBACK (show_zrtp_sas_cb), NULL, NULL);

    dbus_g_proxy_add_signal (callProxy, "secureZrtpOn", G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "secureZrtpOn",
                                 G_CALLBACK (secure_zrtp_on_cb), NULL, NULL);

    /* Register a marshaller for STRING*/
    dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__STRING,
                                       G_TYPE_NONE, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "secureZrtpOff", G_TYPE_STRING,
                             G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "secureZrtpOff",
                                 G_CALLBACK (secure_zrtp_off_cb), NULL, NULL);
    dbus_g_proxy_add_signal (callProxy, "zrtpNotSuppOther", G_TYPE_STRING,
                             G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "zrtpNotSuppOther",
                                 G_CALLBACK (zrtp_not_supported_cb), NULL, NULL);
    dbus_g_proxy_add_signal (callProxy, "confirmGoClear", G_TYPE_STRING,
                             G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "confirmGoClear",
                                 G_CALLBACK (confirm_go_clear_cb), NULL, NULL);

    /* VOID STRING STRING INT */
    dbus_g_object_register_marshaller (
        g_cclosure_user_marshal_VOID__STRING_STRING_INT, G_TYPE_NONE,
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INVALID);
    dbus_g_proxy_add_signal (callProxy, "sipCallStateChanged", G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_INT, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal (callProxy, "sipCallStateChanged",
                                 G_CALLBACK (sip_call_state_cb), NULL, NULL);

    return callProxy;
}

/**
 * Connect to the instance manager.
 * @param connection The Dbus connection.
 */
static DBusGProxy *
connect_to_instance_proxy (DBusGConnection * connection)
{
    DBusGProxy* instance = dbus_g_proxy_new_for_name (connection,
                           "org.sflphone.SFLphone", "/org/sflphone/SFLphone/Instance",
                           "org.sflphone.SFLphone.Instance");

    if (instance == NULL) {
        ERROR ("Failed to get proxy to Instance");
        return NULL;
    }

    DEBUG ("DBus connected to Instance");

    return instance;
}

/**
 * Connect to the video manager.
 * @param connection The Dbus connection.
 */
static DBusGProxy *
connect_to_video_manager (DBusGConnection * connection)
{
    DBusGProxy * videoProxy = dbus_g_proxy_new_for_name (connection,
                              "org.sflphone.SFLphone", "/org/sflphone/SFLphone/VideoManager",
                              "org.sflphone.SFLphone.VideoManager");

    if (!videoProxy) {
        ERROR ("Failed to get proxy to VideoManager");
        return NULL;
    }

    DEBUG ("DBus connected to VideoManager");

    // onNewRemoteVideoStream signal
    dbus_g_object_register_marshaller (g_cclosure_user_marshal_VOID__STRING_STRING,
                                       G_TYPE_NONE, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);

    dbus_g_proxy_add_signal (videoProxy, "onNewRemoteVideoStream", G_TYPE_STRING, G_TYPE_STRING,
                             G_TYPE_INVALID);

    dbus_g_proxy_add_signal (videoProxy, "onRemoteVideoStreamStopped", G_TYPE_STRING, G_TYPE_STRING,
                             G_TYPE_INVALID);

    return videoProxy;
}

DBusGProxy *
dbus_get_video_proxy()
{
    return videoManagerProxy;
}

gboolean
dbus_connect ()
{
    GError *error = NULL;
    connection = NULL;
    instanceProxy = NULL;

    g_type_init ();

    connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

    if (error) {
        ERROR ("Failed to open connection to bus: %s", error->message);
        g_error_free (error);
        return FALSE;
    }

    /* Create a proxy object for the "bus driver" (name "org.freedesktop.DBus") */
    instanceProxy = connect_to_instance_proxy (connection);

    if (instanceProxy == NULL) {
        return FALSE;
    }

    /* Connect to the call manager */
    callManagerProxy = connect_to_call_manager (connection);

    if (callManagerProxy == NULL) {
        return FALSE;
    }

    /* Connect to the configuration manager */
    configurationManagerProxy = connect_to_configuration_manager (connection);

    if (configurationManagerProxy == NULL) {
        return FALSE;
    }

    /* Connect to the video manager */
    videoManagerProxy = connect_to_video_manager (connection);

    if (videoManagerProxy == NULL) {
        return FALSE;
    }

    /* Defines a default timeout for the proxies */
#if HAVE_DBUS_G_PROXY_SET_DEFAULT_TIMEOUT
    dbus_g_proxy_set_default_timeout (callManagerProxy, DEFAULT_DBUS_TIMEOUT);
    dbus_g_proxy_set_default_timeout (instanceProxy, DEFAULT_DBUS_TIMEOUT);
    dbus_g_proxy_set_default_timeout (configurationManagerProxy, DEFAULT_DBUS_TIMEOUT);
    dbus_g_proxy_set_default_timeout (videoManagerProxy, DEFAULT_DBUS_TIMEOUT);
#endif

    return TRUE;
}

void
dbus_clean ()
{
    g_object_unref (callManagerProxy);
    g_object_unref (configurationManagerProxy);
    g_object_unref (instanceProxy);
}

void
dbus_hold (const callable_obj_t * c)
{
    DEBUG ("dbus_hold %s\n", c->_callID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_hold (callManagerProxy, c->_callID, &error);

    if (error) {
        ERROR ("Failed to call hold() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_unhold (const callable_obj_t * c)
{
    DEBUG ("dbus_unhold %s\n", c->_callID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_unhold (callManagerProxy, c->_callID, &error);

    if (error) {
        ERROR ("Failed to call unhold() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_hold_conference (const conference_obj_t * c)
{
    DEBUG ("dbus_hold_conference %s\n", c->_confID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_hold_conference (callManagerProxy,
            c->_confID, &error);

    if (error) {
        ERROR ("Failed to call hold() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_unhold_conference (const conference_obj_t * c)
{
    DEBUG ("dbus_unhold_conference %s\n", c->_confID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_unhold_conference (callManagerProxy,
            c->_confID, &error);

    if (error) {
        ERROR ("Failed to call unhold() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_hang_up (const callable_obj_t * c)
{
    DEBUG ("dbus_hang_up %s\n", c->_callID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_hang_up (callManagerProxy, c->_callID,
            &error);

    if (error) {
        ERROR ("Failed to call hang_up() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_hang_up_conference (const conference_obj_t * c)
{
    DEBUG ("dbus_hang_up_conference %s\n", c->_confID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_hang_up_conference (callManagerProxy,
            c->_confID, &error);

    if (error) {
        ERROR ("Failed to call hang_up() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_transfert (const callable_obj_t * c)
{
    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_transfert (callManagerProxy, c->_callID,
            c->_trsft_to, &error);

    if (error) {
        ERROR ("Failed to call transfert() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_accept (const callable_obj_t * c)
{
#if GTK_CHECK_VERSION(2,10,0)
    status_tray_icon_blink (FALSE);
#endif

    DEBUG ("dbus_accept %s\n", c->_callID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_accept (callManagerProxy, c->_callID, &error);

    if (error) {
        ERROR ("Failed to call accept(%s) on CallManager: %s", c->_callID,
               (error->message == NULL ? g_quark_to_string (error->domain) : error->message));
        g_error_free (error);
    }
}

void
dbus_refuse (const callable_obj_t * c)
{
#if GTK_CHECK_VERSION(2,10,0)
    status_tray_icon_blink (FALSE);
#endif

    DEBUG ("dbus_refuse %s\n", c->_callID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_refuse (callManagerProxy, c->_callID, &error);

    if (error) {
        ERROR ("Failed to call refuse() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_place_call (const callable_obj_t * c)
{
    DEBUG ("dbus_place_call %s\n", c->_callID);

    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_place_call (callManagerProxy, c->_accountID,
            c->_callID, c->_peer_number, &error);

    if (error) {
        ERROR ("Failed to call placeCall() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

gchar**
dbus_account_list ()
{
    GError *error = NULL;
    char ** array;

    if (!org_sflphone_SFLphone_ConfigurationManager_get_account_list (
                configurationManagerProxy, &array, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_account_list) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_account_list: %s", error->message);
        }

        g_error_free (error);
        return NULL;
    } else {
        DEBUG ("DBus: Get account list");
        return array;
    }
}

GHashTable*
dbus_account_details (gchar * accountID)
{
    GError *error = NULL;
    GHashTable * details;

    DEBUG ("Dbus: Get account detail accountid %s", accountID);

    if (!org_sflphone_SFLphone_ConfigurationManager_get_account_details (
                configurationManagerProxy, accountID, &details, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_account_details) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_account_details: %s", error->message);
        }

        g_error_free (error);
        return NULL;
    } else {
        return details;
    }
}

void
dbus_set_credential (account_t *a, int index)
{
    DEBUG ("Sending credential %d to server", index);
    GError *error = NULL;
    GHashTable * credential = g_ptr_array_index (a->credential_information, index);

    if (credential == NULL) {
        DEBUG ("Credential %d was deleted", index);
    } else {
        org_sflphone_SFLphone_ConfigurationManager_set_credential (
            configurationManagerProxy, a->accountID, index, credential, &error);
    }

    if (error) {
        ERROR ("Failed to call set_credential() on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}
void
dbus_delete_all_credential (account_t *a)
{
    DEBUG ("Deleting all credentials\n");
    GError *error = NULL;

    org_sflphone_SFLphone_ConfigurationManager_delete_all_credential (
        configurationManagerProxy, a->accountID, &error);

    if (error) {
        ERROR ("Failed to call deleteAllCredential on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}

int
dbus_get_number_of_credential (gchar * accountID)
{
    GError *error = NULL;
    int number = 0;

    DEBUG ("Getting number of credential for account %s", accountID);

    if (!org_sflphone_SFLphone_ConfigurationManager_get_number_of_credential (
                configurationManagerProxy, accountID, &number, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_account_details) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_account_details: %s", error->message);
        }

        g_error_free (error);
        return 0;
    } else {
        DEBUG ("%d credential(s) found for account %s", number, accountID);
        return number;
    }
}

GHashTable*
dbus_get_credential (gchar * accountID, int index)
{
    GError *error = NULL;
    GHashTable * details;

    if (!org_sflphone_SFLphone_ConfigurationManager_get_credential (
                configurationManagerProxy, accountID, index, &details, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_account_details) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_account_details: %s", error->message);
        }

        g_error_free (error);
        return NULL;
    } else {
        return details;
    }
}

GHashTable*
dbus_get_ip2_ip_details (void)
{
    GError *error = NULL;
    GHashTable * details;

    DEBUG("Dbus: Get IP2IP details");

    if (!org_sflphone_SFLphone_ConfigurationManager_get_ip2_ip_details (
                configurationManagerProxy, &details, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_ip2_ip_details) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_ip2_ip_details: %s", error->message);
        }

        g_error_free (error);
        return NULL;
    } else {
        return details;
    }
}

void
dbus_set_ip2ip_details (GHashTable * properties)
{
    GError *error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_ip2_ip_details (
        configurationManagerProxy, properties, &error);

    if (error) {
        ERROR ("Failed to call set_ip_2ip_details() on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_send_register (gchar* accountID, const guint enable)
{
    GError *error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_send_register (
        configurationManagerProxy, accountID, enable, &error);

    if (error) {
        ERROR ("Failed to call send_register() on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_remove_account (gchar * accountID)
{
    GError *error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_remove_account (
        configurationManagerProxy, accountID, &error);

    if (error) {
        ERROR ("Failed to call remove_account() on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_set_account_details (account_t *a)
{
    DEBUG("Dbus: Set account details");

    GError *error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_account_details (
        configurationManagerProxy, a->accountID, a->properties, &error);

    if (error) {
        ERROR ("Failed to call set_account_details() on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}
gchar*
dbus_add_account (account_t *a)
{
    DEBUG("Dbus: Add account");

    gchar* accountId;
    GError *error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_add_account (
        configurationManagerProxy, a->properties, &accountId, &error);

    if (error) {
        ERROR ("Failed to call add_account() on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }

    return accountId;
}

void
dbus_set_volume (const gchar * device, gdouble value)
{
    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_set_volume (callManagerProxy, device, value,
            &error);

    if (error) {
        ERROR ("Failed to call set_volume() on callManagerProxy: %s",
               error->message);
        g_error_free (error);
    }
}

gdouble
dbus_get_volume (const gchar * device)
{
    gdouble value;
    GError *error = NULL;

    org_sflphone_SFLphone_CallManager_get_volume (callManagerProxy, device,
            &value, &error);

    if (error) {
        ERROR ("Failed to call get_volume() on callManagerProxy: %s",
               error->message);
        g_error_free (error);
    }

    return value;
}

void
dbus_play_dtmf (const gchar * key)
{
    GError *error = NULL;

    org_sflphone_SFLphone_CallManager_play_dt_mf (callManagerProxy, key, &error);

    if (error) {
        ERROR ("Failed to call playDTMF() on callManagerProxy: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_start_tone (const int start, const guint type)
{
    GError *error = NULL;

    org_sflphone_SFLphone_CallManager_start_tone (callManagerProxy, start, type,
            &error);

    if (error) {
        ERROR ("Failed to call startTone() on callManagerProxy: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_register (int pid, gchar * name)
{
    GError *error = NULL;

    org_sflphone_SFLphone_Instance_register (instanceProxy, pid, name, &error);

    if (error) {
        ERROR ("Failed to call register() on instanceProxy: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_unregister (int pid)
{
    GError *error = NULL;

    org_sflphone_SFLphone_Instance_unregister (instanceProxy, pid, &error);

    if (error) {
        ERROR ("Failed to call unregister() on instanceProxy: %s",
               error->message);
        g_error_free (error);
    }
}

GList*
dbus_get_all_audio_codecs ()
{
    GError* error = NULL;
    GPtrArray* audio_codecs = NULL;
    GList* ret = NULL;

    org_sflphone_SFLphone_ConfigurationManager_get_all_audio_codecs (
        configurationManagerProxy, &audio_codecs, &error);

    if (error != NULL) {
        ERROR ("Failed to get all of the audio codecs over dbus");
        g_error_free (error);
        return NULL;
    }

    int i;

    for (i = 0; i < audio_codecs->len; i++) {
        GValue elem = { 0 };
        g_value_init (&elem, DBUS_AUDIO_CODEC_TYPE);

        g_value_set_static_boxed (&elem, g_ptr_array_index (audio_codecs, i));

        audio_codec_t* codec = g_new (audio_codec_t, 1);

        dbus_g_type_struct_get (&elem, 0, &codec->identifier, 1,
                                &codec->clock_rate, 2, &codec->payload, 3, &codec->mime_type, 4,
                                &codec->mime_subtype, 5, &codec->bitrate, 6, &codec->bandwidth, 7,
                                &codec->description, G_MAXUINT);

        codec->is_active = FALSE;

        ret = g_list_append (ret, codec);
    }

    return ret;
}

GList*
dbus_get_all_video_codecs ()
{
    GError* error = NULL;
    GPtrArray* video_codecs = NULL;
    GList* ret = NULL;

    org_sflphone_SFLphone_ConfigurationManager_get_all_video_codecs (
        configurationManagerProxy, &video_codecs, &error);

    if (error != NULL) {
        ERROR ("Failed to get all of the video codecs over dbus");
        g_error_free (error);
        return NULL;
    }

    int i;

    for (i = 0; i < video_codecs->len; i++) {
        GValue elem = { 0 };
        g_value_init (&elem, DBUS_VIDEO_CODEC_TYPE);

        g_value_set_static_boxed (&elem, g_ptr_array_index (video_codecs, i));

        video_codec_t* codec = g_new (video_codec_t, 1);

        dbus_g_type_struct_get (&elem, 0, &codec->identifier, 1,
                                &codec->clock_rate, 2, &codec->payload, 3, &codec->mime_type, 4,
                                &codec->mime_subtype, 5, &codec->bitrate, 6, &codec->bandwidth, 7,
                                &codec->description, G_MAXUINT);

        codec->is_active = FALSE;

        ret = g_list_append (ret, codec);
    }

    return ret;
}

gchar*
dbus_get_current_codec_name (const callable_obj_t * c)
{
    gchar* codecName = "";
    GError* error = NULL;

    org_sflphone_SFLphone_CallManager_get_current_codec_name (callManagerProxy,
            c->_callID, &codecName, &error);

    if (error) {
        g_error_free (error);
    }

    DEBUG ("Current audio codec name : \"%s\"", codecName);

    return codecName;
}

GList*
dbus_get_active_audio_codecs (gchar* accountID)
{
    GError* error = NULL;
    GPtrArray* audio_codecs = NULL;
    GList* ret = NULL;

    DEBUG ("Dbus: Fetching active audio codecs for account \"%s\"", accountID);

    org_sflphone_SFLphone_ConfigurationManager_get_all_active_audio_codecs (
        configurationManagerProxy, accountID, &audio_codecs, &error);

    if (error != NULL) {
        ERROR ("Dbus: Failed to retrieve active audio codecs for account \"%s\" over Dbus (%s:%d)",
                accountID, __FILE__, __LINE__);
        g_error_free (error);
        return NULL;
    }

    DEBUG ("Dbus: Server returned %d audio codecs.", audio_codecs->len);

    int i;

    for (i = 0; i < audio_codecs->len; i++) {
        GValue elem = { 0 };
        g_value_init (&elem, DBUS_AUDIO_CODEC_TYPE);
        g_value_set_static_boxed (&elem, g_ptr_array_index (audio_codecs, i));

        audio_codec_t* codec = g_new (audio_codec_t, 1);
        dbus_g_type_struct_get (&elem, 0, &codec->identifier, 1,
                                &codec->clock_rate, 2, &codec->payload, 3, &codec->mime_type, 4,
                                &codec->mime_subtype, 5, &codec->bitrate, 6, &codec->bandwidth, 7,
                                &codec->description, G_MAXUINT);

        DEBUG ("Dbus: Audio codec %s/%s %d (payload number %d)\n"
               "               Description : \"%s\"\n"
               "               Bandwidth : %f\n"
               "               Bitrate : %f",
               codec->mime_type,
               codec->mime_subtype,
               codec->clock_rate,
               codec->payload,
               codec->description,
               codec->bandwidth,
               codec->bitrate);

        codec->is_active = TRUE;

        ret = g_list_append (ret, codec);
    }

    return ret;
}

void
dbus_set_active_audio_codecs (const gchar** list, const gchar *accountID)
{
    DEBUG ("DBus: Sending active audio codec list for account %s", accountID);
    GError *error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_active_audio_codecs (
        configurationManagerProxy, list, accountID, &error);

    if (error) {
        ERROR ("Failed to call setActiveAudioCodecs on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}

GList*
dbus_get_active_video_codecs (gchar* accountID)
{
    GError* error = NULL;
    GPtrArray* video_codecs = NULL;
    GList* ret = NULL;

    DEBUG ("Fetching active video codecs for account \"%s\" ...", accountID);

    org_sflphone_SFLphone_ConfigurationManager_get_all_active_video_codecs (
        configurationManagerProxy, accountID, &video_codecs, &error);

    if (error != NULL) {
        ERROR ("Failed to retrieve active video codecs for account \"%s\" over Dbus", accountID);
        g_error_free (error);
        return NULL;
    }

    DEBUG ("Server returned %d video codecs.", video_codecs->len);

    int i;

    for (i = 0; i < video_codecs->len; i++) {
        GValue elem = { 0 };
        g_value_init (&elem, DBUS_VIDEO_CODEC_TYPE);
        g_value_set_static_boxed (&elem, g_ptr_array_index (video_codecs, i));

        video_codec_t* codec = g_new (video_codec_t, 1);
        dbus_g_type_struct_get (&elem, 0, &codec->identifier, 1,
                                &codec->clock_rate, 2, &codec->payload, 3, &codec->mime_type, 4,
                                &codec->mime_subtype, 5, &codec->bitrate, 6, &codec->bandwidth, 7,
                                &codec->description, G_MAXUINT);

        DEBUG ("Video codec %s/%s %d (payload number %d)\nDescription : \"%s\"\nBandwidth : %f\nBitrate : %f",
               codec->mime_type,
               codec->mime_subtype,
               codec->clock_rate,
               codec->payload,
               codec->description,
               codec->bandwidth,
               codec->bitrate);

        codec->is_active = TRUE;

        ret = g_list_append (ret, codec);
    }

    return ret;
}

void
dbus_set_active_video_codecs (const gchar** list, const gchar *accountID)
{
    DEBUG ("Sending active video codec list for account %s ...", accountID);
    GError *error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_active_video_codecs (
        configurationManagerProxy, list, accountID, &error);

    if (error) {
        ERROR ("Failed to call setActiveVideoCodecs on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_set_video_settings (const gchar* accountID,
                         video_settings_t* video_settings)
{
    DEBUG ("Setting video for account id %s", accountID);

    GValue elem = { 0 };
    g_value_init (&elem, DBUS_VIDEO_SETTINGS_TYPE);
    g_value_take_boxed (&elem, dbus_g_type_specialized_construct (
                            DBUS_VIDEO_SETTINGS_TYPE));

    // Set resolution
    GValue resolution = { 0 };
    g_value_init (&resolution, DBUS_VIDEO_RESOLUTION_TYPE);
    g_value_take_boxed (&resolution, dbus_g_type_specialized_construct (
                            DBUS_VIDEO_RESOLUTION_TYPE));

    dbus_g_type_struct_set (&resolution,
                            0, video_settings_get_width (video_settings),
                            1, video_settings_get_height (video_settings),
                            G_MAXUINT);

    // Set framerate
    GValue framerate = { 0 };
    g_value_init (&framerate, DBUS_VIDEO_FRAMERATE_TYPE);
    g_value_take_boxed (&framerate, dbus_g_type_specialized_construct (
                            DBUS_VIDEO_FRAMERATE_TYPE));

    dbus_g_type_struct_set (&framerate,
                            0, video_settings_get_numerator (video_settings),
                            1, video_settings_get_denominator (video_settings),
                            G_MAXUINT);

    // Set other properties
    dbus_g_type_struct_set (&elem,
                            0, (GValueArray*) g_value_get_boxed (&resolution),
                            1, (GValueArray*) g_value_get_boxed (&framerate),
                            2, video_settings_get_device (video_settings),
                            3, video_settings_get_always_offer_video (video_settings),
                            G_MAXUINT);

    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_video_settings (
        configurationManagerProxy, accountID, (GValueArray*) g_value_get_boxed (
            &elem), &error);

    if (error) {
        ERROR ("Failed to call setVideoSettings on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }

    DEBUG ("Video settings sent.");
}

video_settings_t*
dbus_get_video_settings (const gchar* accountID)
{
    GError* error = NULL;
    GValueArray* settings;
    org_sflphone_SFLphone_ConfigurationManager_get_video_settings (
        configurationManagerProxy, accountID, &settings, &error);

    DEBUG("Dbus: Get video settings for account: %s", accountID);

    if (error) {
        ERROR ("Dbus: Failed to call getVideoSettings on ConfigurationManager: %s (%s:%d)", error->message, __FILE__, __LINE__);
        g_error_free (error);
    }

    GValueArray* resolution_struct = (GValueArray *) g_value_get_boxed (
                                         g_value_array_get_nth (settings, 0));

    if (resolution_struct->n_values != 2) {
        ERROR ("Dbus: Wrong data format while getting video settings (%s:%d)", __FILE__, __LINE__);
        return NULL;
    }

    GValue* width = g_value_array_get_nth (resolution_struct, 0);
    GValue* height = g_value_array_get_nth (resolution_struct, 1);

    GValueArray* framerate_struct = (GValueArray *) g_value_get_boxed (
                                        g_value_array_get_nth (settings, 1));

    if (framerate_struct->n_values != 2) {
        ERROR ("Dbus: Wrong data format while getting video settings (%s:%d)", __FILE__, __LINE__);
        return NULL;
    }

    GValue* numerator = g_value_array_get_nth (framerate_struct, 0);
    GValue* denominator = g_value_array_get_nth (framerate_struct, 1);

    GValue* device = g_value_array_get_nth (settings, 2);
    GValue* always_offer_video = g_value_array_get_nth (settings, 3);

    video_settings_t* video_settings = video_settings_new ();
    video_settings_set_width (video_settings, g_value_get_uint (width));
    video_settings_set_height (video_settings, g_value_get_uint (height));

    video_settings_set_numerator (video_settings, g_value_get_uint (numerator));
    video_settings_set_denominator (video_settings,
                                    g_value_get_uint (denominator));

    video_settings_set_device (video_settings, g_value_get_string (device));
    video_settings_set_always_offer_video (video_settings, g_value_get_boolean (
                                               always_offer_video));

    video_settings_print (video_settings);

    return video_settings;
}

/**
 * Get a list of output supported audio plugins
 */
gchar**
dbus_get_audio_plugin_list ()
{
    gchar** array;
    GError* error = NULL;

    if (!org_sflphone_SFLphone_ConfigurationManager_get_audio_plugin_list (
                configurationManagerProxy, &array, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_output_plugin_list) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_out_plugin_list: %s", error->message);
        }

        g_error_free (error);
        return NULL;
    } else {
        return array;
    }
}

void
dbus_set_input_audio_plugin (gchar* audioPlugin)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_input_audio_plugin (
        configurationManagerProxy, audioPlugin, &error);

    if (error) {
        ERROR ("Failed to call set_input_audio_plugin() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }
}

void
dbus_set_output_audio_plugin (gchar* audioPlugin)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_output_audio_plugin (
        configurationManagerProxy, audioPlugin, &error);

    if (error) {
        ERROR ("Failed to call set_output_audio_plugin() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }
}

/**
 * Get all output devices index supported by current audio manager
 */
gchar**
dbus_get_audio_output_device_list ()
{
    gchar** array;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_audio_output_device_list (
        configurationManagerProxy, &array, &error);

    if (error) {
        ERROR ("Failed to call get_audio_output_device_list() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }

    return array;
}

/**
 * Set audio output device from its index
 */
void
dbus_set_audio_output_device (const int index)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_audio_output_device (
        configurationManagerProxy, index, &error);

    if (error) {
        ERROR ("Failed to call set_audio_output_device() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }
}

/**
 * Set audio input device from its index
 */
void
dbus_set_audio_input_device (const int index)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_audio_input_device (
        configurationManagerProxy, index, &error);

    if (error) {
        ERROR ("Failed to call set_audio_input_device() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }
}

/**
 * Set adio ringtone device from its index
 */
void
dbus_set_audio_ringtone_device (const int index)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_audio_ringtone_device (
        configurationManagerProxy, index, &error);

    if (error) {
        ERROR ("Failed to call set_audio_ringtone_device() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }
}

/**
 * Get all input devices index supported by current audio manager
 */
gchar**
dbus_get_audio_input_device_list ()
{
    gchar** array;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_audio_input_device_list (
        configurationManagerProxy, &array, &error);

    if (error) {
        ERROR ("Failed to call get_audio_input_device_list() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }

    return array;
}

/**
 * Get output device index and input device index
 */
gchar**
dbus_get_current_audio_devices_index ()
{
    gchar** array;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_current_audio_devices_index (
        configurationManagerProxy, &array, &error);

    if (error) {
        ERROR ("Failed to call get_current_audio_devices_index() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }

    return array;
}

/**
 * Get index
 */
int
dbus_get_audio_device_index (const gchar *name)
{
    int index;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_audio_device_index (
        configurationManagerProxy, name, &index, &error);

    if (error) {
        ERROR ("Failed to call get_audio_device_index() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }

    return index;
}

/**
 * Get audio plugin
 */
gchar*
dbus_get_current_audio_output_plugin ()
{
    gchar* plugin = "";
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_current_audio_output_plugin (
        configurationManagerProxy, &plugin, &error);

    if (error) {
        ERROR ("Failed to call get_current_audio_output_plugin() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }

    return plugin;
}

/**
 * Get noise reduction state
 */
gchar*
dbus_get_noise_suppress_state ()
{
    gchar* state = "";
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_noise_suppress_state (configurationManagerProxy, &state, &error);

    if (error) {
        ERROR ("DBus: Failed to call get_noise_suppress_state() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }

    return state;
}

/**
 * Set noise reduction state
 */
void
dbus_set_noise_suppress_state (gchar* state)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_noise_suppress_state (
        configurationManagerProxy, state, &error);

    if (error) {
        ERROR ("Failed to call set_noise_suppress_state() on ConfigurationManager: %s", error->message);
        g_error_free (error);
    }
}

gchar*
dbus_get_ringtone_choice (const gchar *accountID)
{
    gchar* tone;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_ringtone_choice (
        configurationManagerProxy, accountID, &tone, &error);

    if (error) {
        g_error_free (error);
    }

    return tone;
}

void
dbus_set_ringtone_choice (const gchar *accountID, const gchar* tone)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_ringtone_choice (
        configurationManagerProxy, accountID, tone, &error);

    if (error) {
        g_error_free (error);
    }
}

int
dbus_is_ringtone_enabled (const gchar *accountID)
{
    int res;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_is_ringtone_enabled (
        configurationManagerProxy, accountID, &res, &error);

    if (error) {
        g_error_free (error);
    }

    return res;
}

void
dbus_ringtone_enabled (const gchar *accountID)
{
    DEBUG ("DBUS: Ringtone enabled %s", accountID);

    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_ringtone_enabled (
        configurationManagerProxy, accountID, &error);

    if (error) {
        g_error_free (error);
    }
}

gboolean
dbus_is_md5_credential_hashing ()
{
    int res;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_is_md5_credential_hashing (
        configurationManagerProxy, &res, &error);

    if (error) {
        g_error_free (error);
    }

    return res;
}

void
dbus_set_md5_credential_hashing (gboolean enabled)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_md5_credential_hashing (
        configurationManagerProxy, enabled, &error);

    if (error) {
        g_error_free (error);
    }
}

int
dbus_is_iax2_enabled ()
{
    int res;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_is_iax2_enabled (
        configurationManagerProxy, &res, &error);

    if (error) {
        g_error_free (error);
    }

    return res;
}

void
dbus_join_participant (const gchar* sel_callID, const gchar* drag_callID)
{

    DEBUG ("dbus_join_participant %s and %s\n", sel_callID, drag_callID);

    GError* error = NULL;

    org_sflphone_SFLphone_CallManager_join_participant (callManagerProxy,
            sel_callID, drag_callID, &error);

    if (error) {
        g_error_free (error);
    }

}

void
dbus_add_participant (const gchar* callID, const gchar* confID)
{

    DEBUG ("dbus_add_participant %s and %s\n", callID, confID);

    GError* error = NULL;

    org_sflphone_SFLphone_CallManager_add_participant (callManagerProxy, callID,
            confID, &error);

    if (error) {
        g_error_free (error);
    }

}

void
dbus_add_main_participant (const gchar* confID)
{
    DEBUG ("dbus_add_participant %s\n", confID);

    GError* error = NULL;

    org_sflphone_SFLphone_CallManager_add_main_participant (callManagerProxy,
            confID, &error);

    if (error) {
        g_error_free (error);
    }
}

void
dbus_detach_participant (const gchar* callID)
{

    DEBUG ("dbus_detach_participant %s\n", callID);

    GError* error = NULL;
    org_sflphone_SFLphone_CallManager_detach_participant (callManagerProxy,
            callID, &error);

    if (error) {
        g_error_free (error);
    }

}

void
dbus_join_conference (const gchar* sel_confID, const gchar* drag_confID)
{

    DEBUG ("dbus_join_conference %s and %s\n", sel_confID, drag_confID);

    GError* error = NULL;

    org_sflphone_SFLphone_CallManager_join_conference (callManagerProxy,
            sel_confID, drag_confID, &error);

    if (error) {
        g_error_free (error);
    }

}

void
dbus_set_record (const gchar* id)
{
    DEBUG ("Dbus: dbus_set_record %s", id);

    GError* error = NULL;
    org_sflphone_SFLphone_CallManager_set_recording (callManagerProxy, id, &error);

    if (error) {
        g_error_free (error);
    }
}

gboolean
dbus_get_is_recording (const callable_obj_t * c)
{
    DEBUG ("Dbus: dbus_get_is_recording %s", c->_callID);
    GError* error = NULL;
    gboolean isRecording;
    org_sflphone_SFLphone_CallManager_get_is_recording (callManagerProxy,
            c->_callID, &isRecording, &error);

    if (error) {
        g_error_free (error);
    }

    //DEBUG("RECORDING: %i",isRecording);
    return isRecording;
}

void
dbus_set_record_path (const gchar* path)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_record_path (
        configurationManagerProxy, path, &error);

    if (error) {
        g_error_free (error);
    }
}

gchar*
dbus_get_record_path (void)
{
    GError* error = NULL;
    gchar *path;
    org_sflphone_SFLphone_ConfigurationManager_get_record_path (
        configurationManagerProxy, &path, &error);

    if (error) {
        g_error_free (error);
    }

    return path;
}

void
dbus_set_history_limit (const guint days)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_history_limit (
        configurationManagerProxy, days, &error);

    if (error) {
        g_error_free (error);
    }
}

guint
dbus_get_history_limit (void)
{
    GError* error = NULL;
    gint days = 30;
    org_sflphone_SFLphone_ConfigurationManager_get_history_limit (
        configurationManagerProxy, &days, &error);

    if (error) {
        g_error_free (error);
    }

    return (guint) days;
}

void
dbus_set_audio_manager (int api)
{
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_audio_manager (
        configurationManagerProxy, api, &error);

    if (error) {
        g_error_free (error);
    }
}

int
dbus_get_audio_manager (void)
{
    int api;
    GError* error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_audio_manager (
        configurationManagerProxy, &api, &error);

    if (error) {
        ERROR ("Error calling dbus_get_audio_manager");
        g_error_free (error);
    }

    return api;
}

/*
   void
   dbus_set_sip_address( const gchar* address )
   {
   GError* error = NULL;
   org_sflphone_SFLphone_ConfigurationManager_set_sip_address(
   configurationManagerProxy,
   address,
   &error);
   if(error)
   {
   g_error_free(error);
   }
   }
 */

/*

   gint
   dbus_get_sip_address( void )
   {
   GError* error = NULL;
   gint address;
   org_sflphone_SFLphone_ConfigurationManager_get_sip_address(
   configurationManagerProxy,
   &address,
   &error);
   if(error)
   {
   g_error_free(error);
   }
   return address;
   }
 */

GHashTable*
dbus_get_addressbook_settings (void)
{

    GError *error = NULL;
    GHashTable *results = NULL;

    //DEBUG ("Calling org_sflphone_SFLphone_ConfigurationManager_get_addressbook_settings");

    org_sflphone_SFLphone_ConfigurationManager_get_addressbook_settings (
        configurationManagerProxy, &results, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_get_addressbook_settings");
        g_error_free (error);
    }

    return results;
}

void
dbus_set_addressbook_settings (GHashTable * settings)
{

    GError *error = NULL;

    DEBUG ("Calling org_sflphone_SFLphone_ConfigurationManager_set_addressbook_settings");

    org_sflphone_SFLphone_ConfigurationManager_set_addressbook_settings (
        configurationManagerProxy, settings, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_set_addressbook_settings");
        g_error_free (error);
    }
}

gchar**
dbus_get_addressbook_list (void)
{

    GError *error = NULL;
    gchar** array;

    org_sflphone_SFLphone_ConfigurationManager_get_addressbook_list (
        configurationManagerProxy, &array, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_get_addressbook_list");
        g_error_free (error);
    }

    return array;
}

void
dbus_set_addressbook_list (const gchar** list)
{

    GError *error = NULL;

    org_sflphone_SFLphone_ConfigurationManager_set_addressbook_list (
        configurationManagerProxy, list, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_set_addressbook_list");
        g_error_free (error);
    }
}

GHashTable*
dbus_get_hook_settings (void)
{

    GError *error = NULL;
    GHashTable *results = NULL;

    //DEBUG ("Calling org_sflphone_SFLphone_ConfigurationManager_get_addressbook_settings");

    org_sflphone_SFLphone_ConfigurationManager_get_hook_settings (
        configurationManagerProxy, &results, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_get_hook_settings");
        g_error_free (error);
    }

    return results;
}

void
dbus_set_hook_settings (GHashTable * settings)
{

    GError *error = NULL;

    org_sflphone_SFLphone_ConfigurationManager_set_hook_settings (
        configurationManagerProxy, settings, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_set_hook_settings");
        g_error_free (error);
    }
}

GHashTable*
dbus_get_call_details (const gchar *callID)
{
    GError *error = NULL;
    GHashTable *details = NULL;

    org_sflphone_SFLphone_CallManager_get_call_details (callManagerProxy, callID,
            &details, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_CallManager_get_call_details");
        g_error_free (error);
    }

    return details;
}

gchar**
dbus_get_call_list (void)
{
    GError *error = NULL;
    gchar **list = NULL;

    org_sflphone_SFLphone_CallManager_get_call_list (callManagerProxy, &list,
            &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_CallManager_get_call_list");
        g_error_free (error);
    }

    return list;
}

gchar**
dbus_get_conference_list (void)
{
    GError *error = NULL;
    gchar **list = NULL;

    org_sflphone_SFLphone_CallManager_get_conference_list (callManagerProxy,
            &list, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_CallManager_get_conference_list");
        g_error_free (error);
    }

    return list;
}

gchar**
dbus_get_participant_list (const gchar *confID)
{
    GError *error = NULL;
    char **list = NULL;

    DEBUG ("DBUS: Get conference %s participant list", confID);

    org_sflphone_SFLphone_CallManager_get_participant_list (callManagerProxy,
            confID, &list, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_CallManager_get_participant_list");
        g_error_free (error);
    }

    return list;
}

GHashTable*
dbus_get_conference_details (const gchar *confID)
{
    GError *error = NULL;
    GHashTable *details = NULL;

    org_sflphone_SFLphone_CallManager_get_conference_details (callManagerProxy,
            confID, &details, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_CallManager_get_conference_details");
        g_error_free (error);
    }

    return details;
}

void
dbus_set_accounts_order (const gchar* order)
{

    GError *error = NULL;

    org_sflphone_SFLphone_ConfigurationManager_set_accounts_order (
        configurationManagerProxy, order, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_set_accounts_order");
        g_error_free (error);
    }
}

GHashTable*
dbus_get_history (void)
{
    GError *error = NULL;
    GHashTable *entries = NULL;

    org_sflphone_SFLphone_ConfigurationManager_get_history (
        configurationManagerProxy, &entries, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_CallManager_get_history");
        g_error_free (error);
    }

    return entries;
}

void
dbus_set_history (GHashTable* entries)
{
    GError *error = NULL;

    org_sflphone_SFLphone_ConfigurationManager_set_history (
        configurationManagerProxy, entries, &error);

    if (error) {
        ERROR ("Error calling org_sflphone_SFLphone_CallManager_set_history");
        g_error_free (error);
    }
}

void
dbus_confirm_sas (const callable_obj_t * c)
{
    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_set_sa_sverified (callManagerProxy,
            c->_callID, &error);

    if (error) {
        ERROR ("Failed to call setSASVerified() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_reset_sas (const callable_obj_t * c)
{
    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_reset_sa_sverified (callManagerProxy,
            c->_callID, &error);

    if (error) {
        ERROR ("Failed to call resetSASVerified on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_set_confirm_go_clear (const callable_obj_t * c)
{
    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_set_confirm_go_clear (callManagerProxy,
            c->_callID, &error);

    if (error) {
        ERROR ("Failed to call set_confirm_go_clear on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

void
dbus_request_go_clear (const callable_obj_t * c)
{
    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_request_go_clear (callManagerProxy,
            c->_callID, &error);

    if (error) {
        ERROR ("Failed to call request_go_clear on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

gchar**
dbus_get_supported_tls_method ()
{
    GError *error = NULL;
    gchar** array = NULL;
    org_sflphone_SFLphone_ConfigurationManager_get_supported_tls_method (
        configurationManagerProxy, &array, &error);

    if (error != NULL) {
        ERROR ("Failed to call get_supported_tls_method() on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }

    return array;
}

GHashTable*
dbus_get_tls_settings_default (void)
{
    GError *error = NULL;
    GHashTable *results = NULL;

    org_sflphone_SFLphone_ConfigurationManager_get_tls_settings_default (
        configurationManagerProxy, &results, &error);

    if (error != NULL) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_get_tls_settings_default");
        g_error_free (error);
    }

    return results;
}

gchar *
dbus_get_address_from_interface_name (gchar* interface)
{
    GError *error = NULL;
    gchar * address;

    org_sflphone_SFLphone_ConfigurationManager_get_addr_from_interface_name (
        configurationManagerProxy, interface, &address, &error);

    if (error != NULL) {
        ERROR ("Error calling org_sflphone_SFLphone_ConfigurationManager_get_addr_from_interface_name\n");
        g_error_free (error);
    }

    return address;

}

gchar **
dbus_get_all_ip_interface (void)
{
    GError *error = NULL;
    gchar ** array;

    if (!org_sflphone_SFLphone_ConfigurationManager_get_all_ip_interface (
                configurationManagerProxy, &array, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_all_ip_interface) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_all_ip_interface: %s", error->message);
        }

        g_error_free (error);
        return NULL;
    } else {
        DEBUG ("DBus called get_all_ip_interface() on ConfigurationManager");
        return array;
    }
}

gchar **
dbus_get_all_ip_interface_by_name (void)
{
    GError *error = NULL;
    gchar ** array;

    if (!org_sflphone_SFLphone_ConfigurationManager_get_all_ip_interface_by_name (
                configurationManagerProxy, &array, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_all_ip_interface) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_all_ip_interface: %s", error->message);
        }

        g_error_free (error);
        return NULL;
    } else {
        DEBUG ("DBus called get_all_ip_interface() on ConfigurationManager");
        return array;
    }
}

GHashTable*
dbus_get_shortcuts (void)
{
    GError *error = NULL;
    GHashTable * shortcuts;

    if (!org_sflphone_SFLphone_ConfigurationManager_get_shortcuts (
                configurationManagerProxy, &shortcuts, &error)) {
        if (error->domain == DBUS_GERROR && error->code
                == DBUS_GERROR_REMOTE_EXCEPTION) {
            ERROR ("Caught remote method (get_shortcuts) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        } else {
            ERROR ("Error while calling get_shortcuts: %s", error->message);
        }

        g_error_free (error);
        return NULL;
    } else {
        return shortcuts;
    }
}

void
dbus_set_shortcuts (GHashTable * shortcuts)
{
    GError *error = NULL;
    org_sflphone_SFLphone_ConfigurationManager_set_shortcuts (
        configurationManagerProxy, shortcuts, &error);

    if (error) {
        ERROR ("Failed to call set_shortcuts() on ConfigurationManager: %s",
               error->message);
        g_error_free (error);
    }
}

gchar**
dbus_video_enumerate_devices (void)
{
    GError *error = NULL;
    char ** array = NULL;

    org_sflphone_SFLphone_VideoManager_enumerate_devices (videoManagerProxy,
            &array, &error);

    if (error != NULL) {
        ERROR ("Failed to enumerate devices over dbus.");
        g_error_free (error);
    }

    return array;
}

GList*
dbus_video_get_resolution_for_device (const gchar* device)
{
    GError* error = NULL;
    GPtrArray* resolutions = NULL;
    GList* ret = NULL;

    org_sflphone_SFLphone_VideoManager_get_resolution_for_device (
        videoManagerProxy, device, &resolutions, &error);

    if (error != NULL) {
        ERROR ("Failed to get resolution for device over dbus.");
        g_error_free (error);
    }

    int i;

    for (i = 0; i < resolutions->len; i++) {
        GValue elem = { 0 };
        gint width;
        gint height;

        g_value_init (&elem, DBUS_STRUCT_INT_INT);
        g_value_set_static_boxed (&elem, g_ptr_array_index (resolutions, i));

        dbus_g_type_struct_get (&elem, 0, &width, 1, &height, G_MAXUINT);

        resolution_t* resolution = malloc (sizeof (resolution_t));
        resolution->width = width;
        resolution->height = height;
        ret = g_list_prepend (ret, resolution);
    }

    ret = g_list_reverse (ret);
    return ret;
}

void
dbus_send_text_message (const gchar* callID, const gchar *message)
{
    GError *error = NULL;
    org_sflphone_SFLphone_CallManager_send_text_message (
        callManagerProxy, callID, message, &error);

    if (error) {
        ERROR ("Failed to call send_text_message() on CallManager: %s",
               error->message);
        g_error_free (error);
    }
}

gchar**
dbus_video_get_framerates (const gchar* device, const gint width,
                           const gint height)
{
    GError *error = NULL;
    char ** array = NULL;

    org_sflphone_SFLphone_VideoManager_get_frame_rates (videoManagerProxy,
            device, width, height, &array, &error);

    DEBUG ("Requesting supported framerates for device %s under %d x %d", device, width, height);

    if (error != NULL) {
        ERROR ("Failed to get resolution for device over dbus.");
        g_error_free (error);
    }

    return array;
}

video_shm_info*
dbus_get_video_shm_info (const gchar* shm)
{
    GError* error = NULL;
    GValueArray* shmInfo = NULL;

    org_sflphone_SFLphone_VideoManager_get_shm_info (videoManagerProxy, shm, &shmInfo, &error);

    if (error != NULL) {
        ERROR ("Caught remote method (getShmInfo) exception %s", error->message);
        g_error_free (error);
        return NULL;
    }

    video_shm_info* info = g_new (video_shm_info, 1);
    info->width = g_value_get_uint (g_value_array_get_nth (shmInfo, 0));
    info->height = g_value_get_uint (g_value_array_get_nth (shmInfo, 1));
    info->fourcc = g_value_get_uint (g_value_array_get_nth (shmInfo, 2));

    return info;
}

video_key_t*
dbus_video_start_local_capture (const gchar * device, gint width, gint height,
                                gchar* fps)
{
    GError* error = NULL;
    GValueArray* shmToken = NULL;

    org_sflphone_SFLphone_VideoManager_start_local_capture (videoManagerProxy,
            device, width, height, fps, &shmToken, &error);

    if (error != NULL) {
        ERROR ("Caught remote method (startLocalCapture) exception %s", error->message);
        g_error_free (error);
        return NULL;
    }

    video_key_t* key = (video_key_t*) malloc (sizeof (video_key_t));
    key->shm = g_value_dup_string (g_value_array_get_nth (shmToken, 0));
    key->token = g_value_dup_string (g_value_array_get_nth (shmToken, 1));

    return key;
}

gboolean
dbus_video_stop_local_capture (gchar* device, gchar* token)
{
    GError* error = NULL;
    org_sflphone_SFLphone_VideoManager_stop_local_capture (videoManagerProxy,
            device, token, &error);

    if (error != NULL) {
        ERROR ("Caught remote method exception");
        return FALSE;
    }

    return TRUE;
}

gchar*
dbus_video_get_fd_passer_namespace (gchar * shm)
{
    GError *error = NULL;
    gchar *fdpasser = NULL;

    org_sflphone_SFLphone_VideoManager_get_event_fd_passer_namespace (
        videoManagerProxy, shm, &fdpasser, &error);

    if (error != NULL) {
        ERROR ("Caught remote method (startLocalCapture) exception  %s: %s", dbus_g_error_get_name (error), error->message);
        g_error_free (error);
    }

    return fdpasser;
}
