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
#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include <string>
#include "../account.h"
#include "../user_cfg.h"
#include "../global.h"

struct contact_info {					// Struct for user data in XML PIDF presence parsing
	std::string lastElement;			// Used to remember last opened tag
	std::vector<Contact*>* contactList;	// Contact list in which contacts are loaded
	Contact* contact;					// Contact being prepared for insertion containing entries
	ContactEntry* entry;				// Contact entry used for building
};

/**
 * TOCOMMENT
 */
class ContactManager
{
private:
	static ContactManager* instance;
	
protected:
	ContactManager();
public:
	~ContactManager();

	static ContactManager* getInstance();
	
	void readContacts(std::string accountID, std::vector<Contact*>& contactList);
	void saveContacts(std::string accountID, std::vector<Contact*>& contactList);
};

#endif
