/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
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
#include "account.h"
#include "voiplink.h"
#include "manager.h"

#include <string>

Account::Account(const AccountID& accountID) : _accountID(accountID)
{
	_link = NULL;
	_enabled = false;
}

Account::~Account()
{
	// Delete contacts
	std::vector<Contact*>::iterator iter;
	
	iter = _contacts.begin();
	while (iter != _contacts.end())
	{
		delete *iter;
		*iter = NULL;
		iter++;
	}
}

void
Account::loadConfig()
{
	_enabled = Manager::instance().getConfigInt(_accountID, CONFIG_ACCOUNT_ENABLE) ? true : false;
}

// NOW
void
Account::loadContacts()
{
	// TMP
	// Exemple de chargement de contacts pour account 203
	Contact* contact1 = new Contact("GuillaumeID", "Guillaume", "Carmel-Archambault", "guillaume.carmel-archambault@savoirfairelinux.com");
	ContactEntry* entry1 = new ContactEntry("201", "work", true, true);
	contact1->addEntry(entry1);
	ContactEntry* entry2 = new ContactEntry("514-123-1234", "home", false, false);
	contact1->addEntry(entry2);
	_contacts.push_back(contact1);
	
	Contact* contact2 = new Contact("JeromeID", "Jerome", "Oufella", "jerome.oufella@savoirfairelinux.com");
	ContactEntry* entry3 = new ContactEntry("204", "work", true, true);
	contact2->addEntry(entry3);
	ContactEntry* entry4 = new ContactEntry("514-987-9876", "home", true, false);
	contact2->addEntry(entry4);
	_contacts.push_back(contact2);

	// TODO Load contact file containing list of contacts
}

const std::vector<Contact*>
Account::getContacts()
{
	return _contacts;
}

void
Account::subscribeContactsPresence()
{
	if(_link->isContactPresenceSupported())
	{
		// Subscribe to presence for each contact entry that presence is enabled
		std::vector<Contact*>::const_iterator iter;
		
		for(iter = _contacts.begin(); iter != _contacts.end(); iter++)
		{
			_link->subscribePresenceForContact(*iter);
		}
	}
}

void
Account::publishPresence(std::string presenceStatus)
{
	if(_link->isContactPresenceSupported())
		_link->publishPresenceStatus(presenceStatus);
}
