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

#include "contactmanager-glue.h"
#include <dbus-c++/dbus.h>

class ContactManager : 
	public org::sflphone::SFLphone::ContactManager,
	public DBus::IntrospectableAdaptor,
	public DBus::ObjectAdaptor
{
public:
	ContactManager(DBus::Connection& connection);
	static const char* SERVER_PATH;

public:
	std::vector< ::DBus::String > getContacts( const ::DBus::String& accountID );
	std::vector< ::DBus::String > getContactDetails( const ::DBus::String& accountID, const ::DBus::String& contactID );
	std::vector< ::DBus::String > getContactEntries( const ::DBus::String& accountID, const ::DBus::String& contactID );
	std::vector< ::DBus::String > getContactEntryDetails( const ::DBus::String& accountID, const ::DBus::String& contactID, const ::DBus::String& contactEntryID );
	void setContact( const ::DBus::String& accountID, const ::DBus::String& contactID, const ::DBus::String& firstName, const ::DBus::String& lastName, const ::DBus::String& email );
	void removeContact( const ::DBus::String& accountID, const ::DBus::String& contactID );
	void setContactEntry( const ::DBus::String& accountID, const ::DBus::String& contactID, const ::DBus::String& entryID, const ::DBus::String& text, const ::DBus::String& type, const ::DBus::String& IsShown, const ::DBus::String& IsSubscribed );
	void removeContactEntry( const ::DBus::String& accountID, const ::DBus::String& contactID, const ::DBus::String& entryID );
	void setPresence( const ::DBus::String& accountID, const ::DBus::String& presence, const ::DBus::String& additionalInfo );
};

#endif
