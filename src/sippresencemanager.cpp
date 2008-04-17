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

#include "global.h"
#include "sippresencemanager.h"
#include "contact/presencestatus.h"

#include <eXosip2/eXosip.h>
#include <expat.h>

SIPPresenceManager* SIPPresenceManager::instance = NULL;

SIPPresenceManager::SIPPresenceManager()
{
}

SIPPresenceManager::~SIPPresenceManager()
{
	delete instance;
}

SIPPresenceManager*
SIPPresenceManager::getInstance()
{
	if(instance == NULL)
	{
		instance = new SIPPresenceManager();
	}
	return instance;
}

/**
 * Functions for XML parsing of subscription notify event
 */
void character_data_handler(void* userData, const char* name, int len)
{
	// Check with user data if we are in basic or not tag
	// If true, save name in user data and set lastElement empty
	presence_info* info = (presence_info*)userData;
	if(strcmp(info->lastElement.data(), "basic") == 0)
	{
		char* answer = new char[len + 1];
		strncpy(answer, name, len);
		answer[len] = '\0';
		info->basic = answer;
		delete [] answer;
		info->lastElement = "";
	}
	if(strcmp(info->lastElement.data(), "note") == 0)
	{
		char* answer = new char[len + 1];
		strncpy(answer, name, len);
		answer[len] = '\0';
		info->note = answer;
		delete [] answer;
		info->lastElement = "";
	}
}

void start_element_handler(void* userData, const char* name, const char** atts)
{
	// Remember the last opening tag if it is basic or note
	if(strcmp(name, "basic") == 0 || strcmp(name, "note") == 0)
	{
		presence_info* info = (presence_info*)userData;
		info->lastElement = name;
	}
}

void end_element_handler(void *userData, const char *name)
{
	// Nothing to do
}

std::string
SIPPresenceManager::parseNotificationPresenceStatus(char* body)
{
	// User data that will be used when parsing to remember
	// opened element and values for basic and note elements
	presence_info info;
	info.lastElement = "";
	info.basic = "";
	info.note = "";
	
	// Create parser for presence status provided in XML body
	XML_Parser parser = XML_ParserCreate(NULL);

	// Set user data that is a pointer that may be then used in the parsing functions
	XML_SetUserData(parser, &info);

	// Set functions that will be called when parsing doctype declarations, elements and character data
	// XML_SetDoctypeDeclHandler(parser, start_decl_handler, end_decl_handler);
	XML_SetElementHandler(parser, start_element_handler, end_element_handler);
	XML_SetCharacterDataHandler(parser,	character_data_handler);

	// Parse the data
	XML_Parse(parser, body, 4096, 1);

	// Print error if any
//	printf("%s at line %d\n",
//			XML_ErrorString(XML_GetErrorCode(parser)),
//			XML_GetCurrentLineNumber(parser));

	// Free parser
	XML_ParserFree(parser);
	
	// Transform basic and note tags in a defined presence status
	std::string status;
	transformBasicTagsInPresenceStatus(info.basic, info.note, status);
	return status;
}

void
SIPPresenceManager::buildPublishPresenceStatus(std::string userPart, std::string url, std::string status)
{
	_debug("Publishing presence status\n");
	char buf[4096];
	int i;
	osip_message_t* publication;	

	std::string basic;
	std::string note;
	transformPresenceStatusInBasicTags(status, basic, note);

	snprintf(buf, 4096,
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<presence xmlns=\"urn:ietf:params:xml:ns:pidf\"\n\
	xmlns:es=\"urn:ietf:params:xml:ns:pidf:status:rpid-status\"\n\
	entity=\"%s\">\n\
	<tuple id=\"%s\">\n\
		<status>\n\
			<basic>%s</basic>\n\
			<es:activities>\n\
				<es:activity>in-transit</es:activity>\n\
			</es:activities>\n\
		</status>\n\
	<contact priority=\"0.8\">%s</contact>\n\
	<note>%s</note>\n\
	</tuple>\n\
</presence>"
			, url.data(), userPart.data(), basic.data(), url.data(), note.data());

	// Build publish request in PIDF
	i = eXosip_build_publish(&publication, url.data(), url.data(), NULL, "presence", "1800", "application/pidf+xml", buf);

	eXosip_lock();
	i = eXosip_publish(publication, url.data());
	eXosip_unlock();
}

/**
 * Small function to capitalize all letters in a string
 */
const char* strUp(std::string string)
{
	const int length = string.length();
	for(int i = 0; i < length; i++)
	{
		string[i] = std::toupper(string[i]);
	}
	return string.data();
}

void
SIPPresenceManager::transformBasicTagsInPresenceStatus(std::string basic, std::string note, std::string& status)
{
	// Transform tags obtained in SIP/XML messages into an active status
	// TODO Other active status should be catched here
	// TOSEE How will Asterisk or other call servers support passive presence status sent in the future
	if(strcmp(strUp(basic), "OPEN") == 0)
	{
		if(strcmp(strUp(note), "READY") == 0)
			status = PRESENCE_READY;
		else if(strcmp(strUp(note), "RINGING") == 0)
			status = PRESENCE_RINGING;
		else if(strcmp(strUp(note), "ON THE PHONE") == 0 || strcmp(strUp(note), "ONTHEPHONE") == 0)
			status = PRESENCE_ON_THE_PHONE;
	}
	else if(strcmp(strUp(basic), "CLOSED") == 0)
	{
		if(strcmp(strUp(note), "NOT ONLINE") == 0 || strcmp(strUp(note), "NOTONLINE") == 0)
			status = PRESENCE_UNAVAILABLE;
	}
	// Return unknown presence if no status has been found
	else
	{
		status = PRESENCE_UNKNOWN;
		_debug("!!! New unknown SIP/XML status %s, %s received : Implement it rigth away !!!", basic.data(), note.data());
	}
}

void
SIPPresenceManager::transformPresenceStatusInBasicTags(std::string status, std::string& basic, std::string& note)
{
	// Send appropriate SIMPLE XML+PIDF message corresponding to status
	// TODO Follow SIMPLE drafts and RFC to be up to date with standards, fill missing parts
	// TOSEE Maybe the open and note tags are not the one required in the SIMPLE protocol definition
	// Asterisk still does not support these status and will return a 501 not implemented if sent
	if(strcmp(status.data(), PRESENCE_ONLINE) == 0) {
		basic = "open";
		note = "Ready";
	} else if(strcmp(status.data(), PRESENCE_BUSY) == 0) {
		basic = "open";
		note = "";
	} else if(strcmp(status.data(), PRESENCE_AWAY) == 0) {
		basic = "open";
		note = "";
	} else if(strcmp(status.data(), PRESENCE_DO_NOT_DISTURB) == 0) {
		basic = "";
		note = "";
	} else if(strcmp(status.data(), PRESENCE_BE_RIGHT_BACK) == 0) {
		basic = "open";
		note = "";
	} else if(strcmp(status.data(), PRESENCE_OUT_TO_LUNCH) == 0) {
		basic = "open";
		note = "";
	} else if(strcmp(status.data(), PRESENCE_OFFLINE) == 0) {
		basic = "closed";
		note = "";
	} else {
		_debug("Error - publishPresenceStatus : %s status not found", status.data());
	}	
}
