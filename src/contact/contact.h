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

#ifndef CONTACT_H
#define CONTACT_H

#include "contactentry.h"

#include <string>
#include <vector>

/**
 * TOCOMMENT
 * @author Guillaume Carmel-Archambault
 */
class Contact {
public:
	Contact(const std::string contactID);
	Contact(const std::string contactID, const std::string firstName, const std::string lastName, const std::string email);
	Contact(const std::string contactID, const std::string firstName, const std::string lastName,
			const std::string email, const std::string group, const std::string subGroup);
	virtual ~Contact();
	
	std::string getContactID() { return _contactID; }
	std::string getFirstName() { return _firstName; }
	std::string getLastName() { return _lastName; }
	std::string getEmail() { return _email; }
	std::string getGroup() { return _group; }
	std::string getSubGroup() { return _subGroup; }
	
	void setFirstName(std::string firstName) { _firstName = firstName; }
	void setLastName(std::string lastName) { _lastName = lastName; }
	void setEmail(std::string email) { _email = email; }
	void setGroup(std::string group) { _group = group; }
	void setSubGroup(std::string subGroup) { _subGroup = subGroup; }

	const std::vector<ContactEntry*>& getEntries();
	void addEntry(ContactEntry* entry);
		
private:
	// Attributes only related to contact list
	std::string _contactID;			// Unique identifier for each account generated randomly
	std::string _firstName;
	std::string _lastName;
	std::string _email;
	std::string _group;				// UNUSED but could serve to sort contacts in GUI lists
	std::string _subGroup;			// UNUSED

	std::vector<ContactEntry*> _entries;	// Each contact can have multiple ways to contact him (home, mobile, work...)
};

#endif
