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

struct presence_info {			// Struct for user data in XML PIDF presence parsing
	std::string lastElement;	// Used to remember last opened tag
	std::string basic;			// Value of element tag <basic></basic>
	std::string note;			// Value of element tag <note></note>
};

/**
 * TOCOMMENT
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
	
	std::string parseNotificationPresenceStatus(char* body);
	void buildPublishPresenceStatus(std::string userPart, std::string url, std::string status);
	
private:
	void transformTagsInPresenceStatus(std::string basic, std::string note, std::string& status);
	void transformPresenceStatusInTags(std::string status, std::string& basic, std::string& note);
};

#endif
