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

#include <contactmanager.h>
#include "../manager.h"

#include <vector>
#include <string>

const char* ContactManager::SERVER_PATH = "/org/sflphone/SFLphone/ContactManager";

ContactManager::ContactManager(DBus::Connection& connection)
: DBus::ObjectAdaptor(connection, SERVER_PATH)
{
}

std::vector< ::DBus::String >
ContactManager::getContacts( const ::DBus::String& accountID )
{
	return Manager::instance().getContacts(accountID);
}

std::vector< ::DBus::String >
ContactManager::getContactDetails( const ::DBus::String& accountID, const ::DBus::String& contactID )
{
	return Manager::instance().getContactDetails(accountID, contactID);
}

std::vector< ::DBus::String >
ContactManager::getContactEntries( const ::DBus::String& accountID, const ::DBus::String& contactID )
{
	return Manager::instance().getContactEntries(accountID, contactID);
}

std::vector< ::DBus::String >
ContactManager::getContactEntryDetails( const ::DBus::String& accountID, const ::DBus::String& contactID, const ::DBus::String& contactEntryID )
{
	return Manager::instance().getContactEntryDetails(accountID, contactID, contactEntryID);
}

void
ContactManager::setContacts( const ::DBus::String& accountID, const std::vector< ::DBus::String >& details )
{
	// TODO
}

void
ContactManager::setContactEntries( const ::DBus::String& contactID, const std::vector< ::DBus::String >& details )
{
	// TODO
}

void
ContactManager::setPresence( const ::DBus::String& accountID, const ::DBus::String& presence, const ::DBus::String& additionalInfo )
{
	// TODO
}
