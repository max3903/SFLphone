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

#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "manager.h"
#include "dbus/dbusmanager.h"

#include "instance-adaptor.h"
#include "ClientMonitor-glue.h"

#include <dbus-c++/dbus.h>
#include <cc++/thread.h>

class Instance
    : public org::sflphone::SFLphone::Instance_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor,
  public org::freedesktop::DBus_proxy,
  public DBus::IntrospectableProxy,
  public DBus::ObjectProxy
{
    public:
        Instance (DBus::Connection& connection);

        /**
         * The pid and name are currently useless. Only kept for compatibility issues.
         * The sender argument is being passed by Dbus-c++
         */
        void Register (const int32_t& pid, const std::string& name,  const std::string& sender);

        /**
         * Unused as we monitor disconnection directly on the DBUS interface.
         */
        void Unregister (const int32_t& pid);

        /**
         * Unused.
         */
        int32_t getRegistrationCount (void);

        /**
         * Set to /org/sflphone/SFLphone/Instance
         */
        static const char* SERVER_PATH;

        /**
         * Set to org.freedesktop.DBus
         */
        static const char* DBUS_SERVER_NAME;

        /**
         * Set to /org/freedesktop/DBus
         */
        static const char* DBUS_SERVER_PATH;

    protected:
        void removeClient (const std::string& uniqueName);

    private:
        /**
         * @Override
         */
        void NameOwnerChanged (const std::string& name, const std::string& old_owner, const std::string& new_owner);

        /**
         * @Override
         */
        void NameLost (const std::string&);

        /**
         * @Override
         */
        void NameAcquired (const std::string&);

        std::vector<std::string> clients;
        typedef std::vector<std::string>::iterator ClientsIterator;
        ost::Mutex clientsMutex;

        /**
         * A termination thread is spawned too avoid
         * deadlocks in the NameOwnerChanged signal.
         */
        class TerminationThread : public ost::Thread
        {
            public:
                TerminationThread (Instance* self, const std::string& clientName) :
                    parent (self),
                    clientName (clientName) {};
                virtual ~TerminationThread() {}

            protected:
                /**
                 * @Override
                 */
                void run() {
                    parent->removeClient (clientName);
                }

            private:
                Instance* parent;
                std::string clientName;
        };
};


#endif
