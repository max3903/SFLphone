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

/**
 * TOCOMMENT
 * @author Guillaume Carmel-Archambault
 */
class ContactEntry {
public:
	ContactEntry();
	ContactEntry(std::string contact, std::string type, bool showInCallConsole, bool subscribeToPresence);
	virtual ~ContactEntry();
	
	std::string getContact() { return _contact; }
	std::string getType() { return _type; }
	bool getShownInCallConsole() { return _shownInCallConsole; }
	bool getSubscribedToPresence() { return _subscribedToPresence; }	
	
private:
	// Attributes used for presence and call console
	std::string _contact;			// Can be an extension, phone number, url...
	std::string _type;				// Work, mobile, home...
	bool _shownInCallConsole;		// Contact entry will be shown in call console if true
	bool _subscribedToPresence;		// Account link will try to subscribe to presence status if true
	
	// Presence information, will be null if contact is not subscribed
	Presence* _presence;
};

#endif
