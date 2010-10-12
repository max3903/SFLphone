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
#ifndef __SFL_INET_SOCKET_ADDRESS_H__
#define __SFL_INET_SOCKET_ADDRESS_H__

#include "InetAddress.h"
#include "logger.h"
#include <cc++/address.h>

namespace sfl {
class InetSocketAddress {
public:
	/**
	 * Construct a new socket address object.
	 * @param host dns or physical address of an Internet host.
	 * @param port The port for reaching the given host.
	 */
	InetSocketAddress(const char* host, ost::tpport_t port) :
		address(ost::InetHostAddress(host)), port(port) {
	}

	/**
	 * Construct a new socket address object.
	 * @param host dns or physical address of an Internet host.
	 * @param port The port for reaching the given host.
	 */
	InetSocketAddress(const std::string& host, ost::tpport_t port) :
		address(ost::InetHostAddress(host.c_str())), port(port) {
	}

	/**
	 * Destructor.
	 */
	virtual inline ~InetSocketAddress() {};

	/**
	 * Get the address
	 * @return The internet address.
	 */
	ost::InetHostAddress getAddress() const {
		return address;
	}

	/**
	 * Gets the port number.
	 * @return The port number.
	 */
	ost::tpport_t getPort() const {
		return port;
	}

	/**
	 * This operator makes it easier for mapping InetSocketAddress types into std::map structures.
	 */
	bool operator<(const InetSocketAddress& other) const {
		return (toString()).compare(other.toString()) < 0;
	}

	std::string toString() const {
		std::stringstream ss;
		ss << getPort();
		return  std::string(getAddress().getHostname()) + std::string(":") + ss.str();
	}

private:
	ost::InetHostAddress address;
	ost::tpport_t port;
};
}

#endif
