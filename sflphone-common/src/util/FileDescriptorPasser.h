/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
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

#ifndef __FILE_DESCRIPTOR_PASSER_H__
#define __FILE_DESCRIPTOR_PASSER_H__

#include <cc++/thread.h>
#include <sys/socket.h>

namespace sfl
{
class FileDescriptorPasser: public ost::Thread
{
    public:
        /**
         * Construct a new instance of a server that will let client applications to
         * be passed a file descriptor using Unix domain. This class takes the form
         * of a thread. Once started, it will go into an accept() loop, replying to
         * a connection with the file descriptor to pass.
         * @param abstractNamespace The abstract namespace in the UNIX domain that is referring to this server instance. Eg. org.sflphone.eventfd
         * @param fd The file descriptor to send to every client.
         */
        FileDescriptorPasser (const std::string& abstractNamespace, int fd);

        ~FileDescriptorPasser();

        /**
         * @return The namespace for that running instance.
         */
        std::string getAbstractNamespace() const {
            return path;
        };

        /**
         * @return true if the server is up and running.
         */
        bool isReady() const {
            return ready;
        };

        /**
         * @return the file descriptor that is being passed by this server instance.
         */
        inline int getFileDescriptor() const {
        	return fdPassed;
        }

    protected:
        /**
         * @Override
         */
        void run();
        /**
         * @Override
         */
        void initial();
        /**
         * @Override
         */
        void final();
    private:
        int sendFd (int clientFd);

        std::string path;
        int serverSocket;
        int fdPassed;
        bool ready;
};
}
#endif

