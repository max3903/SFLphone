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

/* Definition of all presence status used by the deamon and the GUI
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

// Same active presence status as defined in Asterisk
#define PRESENCE_UNKNOWN			"UNKNOWN"				// Aka UNSUBSCRIBED
#define PRESENCE_READY				"READY"					// Aka NOT_IN_USE
#define PRESENCE_IS_BUSY			"IS_BUSY"	
#define PRESENCE_INVALID			"INVALID"
#define PRESENCE_UNAVAILABLE		"UNAVAILABLE"
#define PRESENCE_ON_THE_PHONE		"ON_THE_PHONE"			// Aka INUSE
#define PRESENCE_RINGING			"RINGING"
#define PRESENCE_RING_IN_USE		"RING_IN_USE"
#define PRESENCE_HOLD_IN_USE		"HOLD_IN_USE"
#define PRESENCE_ON_HOLD			"ON_HOLD"
// Passive presence status defined on some hardware phones
#define PRESENCE_ONLINE				"ONLINE"
#define PRESENCE_BUSY				"BUSY"
#define PRESENCE_BE_RIGHT_BACK		"BE_RIGHT_BACK"
#define PRESENCE_AWAY				"AWAY"
#define PRESENCE_OUT_TO_LUNCH		"OUT_TO_LUNCH"
#define PRESENCE_OFFLINE			"OFFLINE"
#define PRESENCE_DO_NOT_DISTURB		"DO_NOT_DISTURB"
#define PRESENCE_IN_REUNION			"IN_REUNION"			// UNUSED
// Other active presence status defined
#define PRESENCE_IN_CONFERENCE_CALL	"IN_CONFERENCE_CALL"	// UNUSED
#define PRESENCE_IN_VIDEO_CALL		"IN_VIDEO_CALL"			// UNUSED
#define PRESENCE_IN_VIDEO_CONF_CALL	"IN_VIDEO_CONF_CALL"	// UNUSED

#endif
