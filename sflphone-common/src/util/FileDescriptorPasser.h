/*
 *  Copyright (C) 2006-2010 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
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

#ifndef __FILE_DESCRIPTOR_PASSER_H__
#define __FILE_DESCRIPTOR_PASSER_H__

#include <cc++/thread.h>
#include <sys/socket.h>

namespace sfl {
class FileDescriptorPasser: public ost::Thread {
public:
	/**
	 * Construct a new instance of a server that will let client applications to
	 * be passed a file descriptor using unix domain. This class takes the form
	 * of a thread. Once started, it will go into an accept() loop, replying to
	 * a connection with the file descriptor to pass.
	 * @param abstractNamespace The abstract namespace in the UNIX domain that is refering to this server instance. Eg. org.sflphone.eventfd
	 * @param fd The file descriptor to send to every client.
	 */
	FileDescriptorPasser(const std::string& abstractNamespace, int fd);

	/**
	 * @return The namespace for that running instance.
	 */
	std::string getAbstractNamespace() { return path; };

	virtual ~FileDescriptorPasser() {};

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
	/**
	 * Initialize the message header structure.
	 */
	void initMessage();

	int serverSocket;
	int passedFd;

	struct msghdr message;
	struct cmsghdr * cmessage;
	struct iovec pingVec;

	int fileDescriptors[1];
	char messageControlBuffer[CMSG_SPACE(sizeof fileDescriptors)];

	std::string path;
};
}
#endif

