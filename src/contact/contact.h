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

/// Contact information
/**
 * A contact is used to store information as first name, last name and email
 * A synonym for this class would be a buddy as it is usually a person, but can
 * also be for example an organisation, a group of people or else
 * It can possibly have many entries, like telephone numbers related to this contact
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
	
	void setFirstName(std::string firstName) { _firstName = firstName; }
	void setLastName(std::string lastName) { _lastName = lastName; }
	void setEmail(std::string email) { _email = email; }

	const std::vector<ContactEntry*>& getEntries();
	void addEntry(ContactEntry* entry);
		
private:
	// Attributes only related to contact list
	std::string _contactID;			/// Unique identifier for each account generated randomly
	std::string _firstName;			/// First name
	std::string _lastName;			/// Last name (optional)
	std::string _email;				/// Email (optional)

	std::vector<ContactEntry*> _entries;	/// Each contact can have multiple ways to contact him (at work, home phone, mobile...)
	
	// SEE Contacts could be grouped in a tree structure to allow a more convenient managment and display in contact window and call console
	// It could for example be a string separated by a character to express a group path (ex: company/support, friends, school/students/biology)
	// which would allow for easily folding and unfolding groups that we want in the call console or contact window
	// Using only one string would allow to be easily transmittable by D-Bus and an unlimited depth
};

#endif
