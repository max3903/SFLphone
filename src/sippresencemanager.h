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

#ifndef SIPPRESENCEMANAGER_H
#define SIPPRESENCEMANAGER_H

#include <string>

struct presence_info {			/// Struct for user data in XML PIDF presence parsing
	std::string lastElement;	/// Used to remember last opened tag
	std::string basic;			/// Value of element tag <basic></basic>
	std::string note;			/// Value of element tag <note></note>
};

/// Singleton that transforms presence to SIP event and vice versa
/**
 * Singleton used to transform presence information in SIP/PIDF event and
 * inversly to transform SIP/PIDF presence event in presence information
 * Since PIDF is an XML format, it uses expat to parse event's body receveived
 * and to gather in presence info all content in significative XML tags
 */
class SIPPresenceManager
{
private:
	static SIPPresenceManager* instance;
	
protected:
	SIPPresenceManager();
public:
	~SIPPresenceManager();

	static SIPPresenceManager* getInstance();
	
	/**
	 * Parse body of SIP PIDF message to obtain information it contains
	 * and save in the presence info struct
	 */
	std::string parseNotificationPresenceStatus(char* body);
	
	/**
	 * Send a SIP PIDF/XML message to publish passive presence information to server
	 * Online status should not be sent since it is the call server duty to create these events at least with Asterisk
	 */
	void buildPublishPresenceStatus(std::string userPart, std::string url, std::string status);
	
private:
	/**
	 * Return the defined active status corresponding to basic and note tags
	 * Passive tags should also be transformed as soon as the call server is able to send them
	 */
	void transformBasicTagsInPresenceStatus(std::string basic, std::string note, std::string& status);
	
	/**
	 * Return tags basic and note corresponding to passive status
	 */
	void transformPresenceStatusInBasicTags(std::string status, std::string& basic, std::string& note);
};

#endif
