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

#ifndef PRESENCE_STATUS_H
#define PRESENCE_STATUS_H

/**
 * Definition of all presence status used by the deamon and the GUI
 * The deamon knows how to identify tags coming from presence servers
 * and cast them in a defined presence status presented here. 
 * The presence information is transmitted along DBus by those strings.
 * The GUI can format and translate these strings for presentation. 
 * 
 * If a presence status identified by a string cannot be identified
 * when sent from a presence server, we directly use the raw string
 * without any formating or translation process possible.
 * 
 * It is important to distinguish between active and passive presence
 * status. Online, offline, away, busy, do not disturb and such are
 * passive status set by the user or that can change automatically.
 * Ready, ringing, on hold, on the phone,
 * in conference call are status that change according to its actions. 
 * Thus a user always has two status, but for now only the most
 * significative would be represented graphically. 
 * For example a away user ready and a busy user on the phone would
 * present a away user and a user on the phone. So only when the user
 * is in the ready active state, we show its passive state.
 * 
 * Usually a user will publish its passive state manually or it can be
 * triggered automatically as for the away state after a period of
 * inactivity. The active state is usually triggered by the call manager
 * when calls are made or when the phone goes offline for example. 
 * The user listens for both active and passive presence status
 * from subscribed presentities that will usually transit through
 * the call manager. 
 */

// Special presence status for SFLphone
// On the daemon, the presence information will usually remain null when in a special presence state
#define PRESENCE_NOT_SUBSCRIBED		"NOT_SUBSCRIBED"		/// Special status: Entry is not subscribed to presence
#define PRESENCE_NOT_INITIALIZED	"NOT_INITIALIZED"		/// Special status: Entry is subscribed but no information has yet been received
#define PRESENCE_NOT_SUPPORTED		"NOT_SUPPORTED"			/// Special status: Account or entry does not support presence (ex: IAX, external phone number)
// Same active presence status as defined in Asterisk (rasterisk : core show function EXTENSION_STATE, DEVICE_STATE)
// It is possible in Asterisk to define functions that will trigger these status
// By default the READY, RINGING, ON_THE_PHONE, UNAVAILABLE are triggered from Asterisk
// See rasterisk core show functions, core show applications and extensions.conf in Asterisk for more details
#define PRESENCE_UNKNOWN			"UNKNOWN"				/// Active status:
#define PRESENCE_READY				"READY"					/// Active status: Can be joined, aka NOT_INUSE, ONLINE
#define PRESENCE_IS_BUSY			"IS_BUSY"				/// Active status:
#define PRESENCE_INVALID			"INVALID"				/// Active status:
#define PRESENCE_UNAVAILABLE		"UNAVAILABLE"			/// Active status: Reachable but unavailable, aka NOT_ONLINE, OFFLINE
#define PRESENCE_ON_THE_PHONE		"ON_THE_PHONE"			/// Active status: Communication established, aka INUSE
#define PRESENCE_RINGING			"RINGING"				/// Active status: Receiving call
#define PRESENCE_RING_IN_USE		"RING_IN_USE"			/// Active status:
#define PRESENCE_HOLD_IN_USE		"HOLD_IN_USE"			/// Active status:
#define PRESENCE_ON_HOLD			"ON_HOLD"				/// Active status:
// Passive presence status defined on some hardware phones
#define PRESENCE_ONLINE				"ONLINE"				/// Passive status: User tells that he is available
#define PRESENCE_BUSY				"BUSY"					/// Passive status: User tells that he is busy
#define PRESENCE_BE_RIGHT_BACK		"BE_RIGHT_BACK"			/// Passive status: User tells that he will be back soon
#define PRESENCE_AWAY				"AWAY"					/// Passive status: Usually triggered by a certain inactivity period or manually by the user to tell that he is not present at the moment
#define PRESENCE_OUT_TO_LUNCH		"OUT_TO_LUNCH"			/// Passive status: User tells that he is eating
#define PRESENCE_OFFLINE			"OFFLINE"				/// Passive status: User tells that he is not available, will be marked as UNAVAILABLE event if it is not the case 
#define PRESENCE_DO_NOT_DISTURB		"DO_NOT_DISTURB"		/// Passive status: User tells that he does not wish to be disturbed
//#define PRESENCE_IN_REUNION			"IN_REUNION"			// UNUSED
// Other active presence status defined
//#define PRESENCE_IN_CONFERENCE_CALL	"IN_CONFERENCE_CALL"	// UNUSED
//#define PRESENCE_IN_VIDEO_CALL		"IN_VIDEO_CALL"			// UNUSED
//#define PRESENCE_IN_VIDEO_CONF_CALL	"IN_VIDEO_CONF_CALL"	// UNUSED

#endif
