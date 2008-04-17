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

#ifndef CONTACT_ENTRY_H
#define CONTACT_ENTRY_H

#include "presence.h"

#include <string>

/// Information to call a contact
/**
 * A contact entry belongs to a contact and usually represents a phone number or
 * extension, but can be any type of textual url to find the corresponding VoIP user agent.
 * The entry is immutable and should be a unique id per account. The text attribute represents
 * the way to show the entry, the different types are defined in the GTK such as (work, home, mobile, paget, other)
 * The boolean attributes will serve in the GUI to know if the entry should be displayed in the call console as a way
 * to easily interact and check an entry and also to know if the entry should be subscribed for presence information
 */
class ContactEntry {
public:
	ContactEntry(std::string entryID);
	ContactEntry(std::string entryID, std::string text, std::string type, bool showInCallConsole, bool subscribeToPresence);
	virtual ~ContactEntry();
	
	std::string getEntryID() { return _entryID; }
	std::string getText() { return _text; }
	std::string getType() { return _type; }
	bool getShownInCallConsole() { return _shownInCallConsole; }
	bool getSubscribedToPresence() { return _subscribedToPresence; }
	Presence* getPresence() { return _presence; }
	
	void setText(std::string text) { _text = text; }
	void setType(std::string type) { _type = type; }
	void setShownInCallConsole(bool shownInCallConsole) { _shownInCallConsole = shownInCallConsole; }
	void setSubscribedToPresence(bool subscribedToPresence) { _subscribedToPresence = subscribedToPresence; }
	void setPresence(Presence* presence) { delete _presence; _presence = presence; }
	void setPresence(std::string state, std::string additionalInfo) { delete _presence; _presence = new Presence(state, additionalInfo); }
	
private:
	// Attributes used for presence and call console
	std::string _entryID;			/// Unique extension, phone number, url...
	std::string _text;				/// Simplified textual representation of the ID
	std::string _type;				/// Work, mobile, home...
	bool _shownInCallConsole;		/// Contact entry will be shown in call console if true
	bool _subscribedToPresence;		/// Account link will try to subscribe to presence status if true
	
	/// Presence information, will be null if contact is not subscribed, not supported or not initialized (SEE presencestatus.h)
	Presence* _presence;
	
	// SEE Other possible attributes to add for general presence information on entry can be
	// different ringing type for each contact, auto reject, auto transfer, speed dial index
	// capabilities of the entry (audio, video, text..) and user agent type and description
};

#endif
