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
	std::string p =  Manager::instance().getConfigString( _accountID , CONFIG_ACCOUNT_TYPE );
#ifdef USE_IAX	
	  _enabled = Manager::instance().getConfigInt(_accountID, CONFIG_ACCOUNT_ENABLE) ? true : false;
#else
	if( p.c_str() == "IAX" )
	  _enabled = false;
	else
	  _enabled = Manager::instance().getConfigInt(_accountID, CONFIG_ACCOUNT_ENABLE) ? true : false;
#endif
}

void
Account::loadContacts()
{
	// Load contact file containing all contacts and entries for account
	ContactManager::getInstance()->readContacts(_accountID, _contacts);
}

void
Account::saveContacts()
{
	// Save contacts in contact file
	ContactManager::getInstance()->saveContacts(_accountID, _contacts);	
}

const std::vector<Contact*>&
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
		std::vector<Contact*>::const_iterator contactIter;		
		for(contactIter = _contacts.begin(); contactIter != _contacts.end(); contactIter++)
		{
			Contact* contact = (Contact*)*contactIter;
			std::vector<ContactEntry*> entries = contact->getEntries();
			std::vector<ContactEntry*>::const_iterator contactEntryIter;
			for(contactEntryIter = entries.begin(); contactEntryIter != entries.end(); contactEntryIter++)
			{
				ContactEntry* entry = (ContactEntry*)*contactEntryIter;
				if(entry->getSubscribedToPresence())
					_link->subscribePresenceForContact(entry);
			}
		}
	}
}

void
Account::publishPresence(std::string presenceStatus)
{
	if(_link->isContactPresenceSupported())
		_link->publishPresenceStatus(presenceStatus);
}
