/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
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
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */
#include <instance.h>
#include "manager.h"
#include "global.h"

const char* Instance::SERVER_PATH = "/org/sflphone/SFLphone/Instance";
const char* Instance::DBUS_SERVER_NAME = "org.freedesktop.DBus";
const char* Instance::DBUS_SERVER_PATH = "/org/freedesktop/DBus";

Instance::Instance (DBus::Connection& connection)
    : DBus::ObjectAdaptor (connection, SERVER_PATH),
      ::DBus::ObjectProxy (connection, DBUS_SERVER_PATH, DBUS_SERVER_NAME)
{}


void
Instance::NameOwnerChanged (const std::string& name,
                            const std::string& old_owner, const std::string& new_owner)
{
    // Check if a name was released
    ClientsIterator it = std::find (clients.begin(), clients.end(), old_owner);
    if (new_owner == "" && it != clients.end()) {
        TerminationThread* termination = new TerminationThread (this, old_owner);
        termination->start();
    }
}

void Instance::NameLost (const std::string& name)
{
    _warn ("Name lost %s", name.c_str());
}

void Instance::NameAcquired (const std::string& name)
{
    _warn ("Name acquired %s", name.c_str());
}

void
Instance::removeClient (const std::string& uniqueName)
{
    ClientsIterator it = std::find (clients.begin(), clients.end(), uniqueName);
    clients.erase (it);

    if (clients.empty()) {
        Manager::instance().terminate();
        DBusManager::instance().exit();
    }
}

void
Instance::Register (const int32_t& pid UNUSED,
                    const std::string& name UNUSED, const std::string& sender)
{
    _debug ("Instance::register received from %s, adding to table", sender.c_str());
    clients.push_back (sender);
}


void
Instance::Unregister (const int32_t& pid UNUSED)
{
    _debug ("Instance::unregister received");
}

int32_t
Instance::getRegistrationCount (void)
{
    return clients.size();
}

