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
#ifndef __SFL_INET_ADDRESS_H__
#define __SFL_INET_ADDRESS_H__

#include <cc++/address.h>

namespace sfl {
/**
 * Interface type for IPV4 and IPV6 address.
 */
class InetAddress {
public:
	/**
	 * Provide a string representation of the value (Internet Address)
	 * held in the IPV4Address object.
	 *
	 * @return string representation of IPV4Address.
	 */
	virtual const char *getHostname(void) const = 0;

	/**
	 * May be used to verify if a given IPV4Address returned
	 * by another function contains a "valid" address, or "0.0.0.0"
	 * which is often used to mark "invalid" IPV4Address values.
	 *
	 * @return true if address != 0.0.0.0.
	 */
	virtual bool isInetAddress(void) const = 0;

	/**
	 * Provide a low level system usable struct in_addr object from
	 * the contents of IPV4Address.  This is needed for services such
	 * as bind() and connect().
	 *
	 * @return system binary coded internet address.
	 */
	virtual struct in_addr getAddress(void) const = 0;

	/**
	 * Provide a low level system usable struct in_addr object from
	 * the contents of IPV4Address.  This is needed for services such
	 * as bind() and connect().
	 *
	 * @param i for IPV4Addresses with multiple addresses, returns the
	 *	address at this index.  User should call getAddressCount()
	 *	to determine the number of address the object contains.
	 * @return system binary coded internet address.  If parameter i is
	 *	out of range, the first address is returned.
	 */
	virtual struct in_addr getAddress(size_t i) const  = 0;

	/**
	 * Returns the number of internet addresses that an IPV4Address object
	 * contains.  This usually only happens with IPV4Host objects
	 * where multiple IP addresses are returned for a DNS lookup
	 */
	virtual size_t getAddressCount() const  = 0;

protected:
	InetAddress();
	virtual inline ~InetAddress();
};

}

#endif
