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

struct contact_info {					/// Struct for user data in XML PIDF presence parsing
	std::string lastElement;			/// Used to remember last opened tag
	std::vector<Contact*>* contactList;	/// Contact list in which contacts are loaded
	Contact* contact;					/// Contact being prepared for insertion containing entries
	ContactEntry* entry;				/// Contact entry used for building
};

/**
 * The contact manager singleton uses the expat library to read contacts by parsing tags
 * and to save contacts by setting them back in xml tags in the appropriate file
 * The contacts are stored by account in a xml file named according to the account id in the sflphone configuration folder
 * TODO
 * The read function should be called when an account is loaded at daemon startup or when a new one is created
 * The save function should be called when the accounts are closed at the daemon end or when an account is deleted
 * The remove account function should ask if the contacts are to be removed when an account is deleted
 */
class ContactManager
{
private:
	static ContactManager* instance;		/// Static singleton instance of the class
	
protected:
	ContactManager();
public:
	~ContactManager();

	/** Return singleton instance of the class */
	static ContactManager* getInstance();
	
	/**
	 * Read contacts for an account by parsing XML related file in sflphone configuration folder
	 * @param accounID Complete ID of the account to load contacts from
	 * @param contactList Vector list to return
	 */
	void readContacts(std::string accountID, std::vector<Contact*>& contactList);
	/**
	 * Save contacts for an account in XML format in related file in the sflphone configuration folder
	 * @param accounID Complete ID of the account to save contacts from
	 * @param contactList Vector containing contacts and entries to save
	 */
	void saveContacts(std::string accountID, std::vector<Contact*>& contactList);
};

#endif
