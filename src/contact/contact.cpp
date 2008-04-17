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

#include "contact.h"

Contact::Contact(const std::string contactID)
{
	_contactID = contactID;
}

Contact::Contact(const std::string contactID, const std::string firstName, const std::string lastName, const std::string email)
{
	_contactID = contactID;
	_firstName = firstName;
	_lastName = lastName;
	_email = email;
}

Contact::Contact(const std::string contactID, const std::string firstName, const std::string lastName,
		const std::string email, const std::string group, const std::string subGroup)
{
	_contactID = contactID;
	_firstName = firstName;
	_lastName = lastName;
	_email = email;
}

Contact::~Contact()
{
	// Delete contact entries
	std::vector<ContactEntry*>::iterator iter;
	
	iter = _entries.begin();
	while (iter != _entries.end())
	{
		delete *iter;
		*iter = NULL;
		iter++;
	}
}

const std::vector<ContactEntry*>&
Contact::getEntries()
{
	return _entries;
}

void
Contact::addEntry(ContactEntry* entry)
{
	_entries.push_back(entry);
}
