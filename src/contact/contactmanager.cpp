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

#include <expat.h>
#include <fstream>
#include "contactmanager.h"

ContactManager* ContactManager::instance = NULL;

ContactManager::ContactManager()
{
}

ContactManager::~ContactManager()
{
	delete instance;
}

ContactManager*
ContactManager::getInstance()
{
	if(instance == NULL)
	{
		instance = new ContactManager();
	}
	return instance;
}

void contact_character_data_handler(void* userData, const char* name, int len)
{
	// Obtain contact info contained in user data
	contact_info* info = (contact_info*)userData;
	
	// Create answer from len and non null terminated array name
	char* answer = new char[len + 1];
	strncpy(answer, name, len);
	answer[len] = '\0';
	
	if(strcmp(info->lastElement.data(), "firstName") == 0)
	{
		info->contact->setFirstName(answer);
		info->lastElement = "";
	}
	if(strcmp(info->lastElement.data(), "lastName") == 0)
	{
		info->contact->setLastName(answer);
		info->lastElement = "";
	}
	if(strcmp(info->lastElement.data(), "email") == 0)
	{
		info->contact->setEmail(answer);
		info->lastElement = "";
	}
	delete [] answer;
}

void contact_start_element_handler(void* userData, const char* name, const char** atts)
{
	// Remember current element in user data
	contact_info* info = (contact_info*)userData;
	info->lastElement = name;
	
	if(strcmp(name, "contact") == 0)
	{
		bool value = false;		// To know if parsing name or value
		const char** attribute;
		for(attribute = atts; *attribute; attribute++)
		{
			if(value && strcmp(*(attribute-1), "id") == 0)
			{
				info->contact = new Contact(*attribute);
			}
			// Alterning between name and value
			value = ! value;
		}
	}
	if(strcmp(name, "entry") == 0)
	{
		bool value = false;		// To know if parsing name or value
		const char** attribute;
		for(attribute = atts; *attribute; attribute++)
		{
			if(value)
			{
				// Parsing a value so its name is the previous string
				const char* attributeName = *(attribute-1);
				if(strcmp(attributeName, "id") == 0)
				{
					info->entry = new ContactEntry(*attribute);
				}
				if(strcmp(attributeName, "text") == 0)
				{
					info->entry->setText(*attribute);
				}
				if(strcmp(attributeName, "type") == 0)
				{
					info->entry->setType(*attribute);
				}
				if(strcmp(attributeName, "inConsole") == 0)
				{
					if(strcmp(*attribute, "true") == 0)
						info->entry->setShownInCallConsole(true);
					if(strcmp(*attribute, "false") == 0)
						info->entry->setShownInCallConsole(false);
				}
				if(strcmp(attributeName, "subscribed") == 0)
				{
					if(strcmp(*attribute, "true") == 0)
						info->entry->setSubscribedToPresence(true);
					if(strcmp(*attribute, "false") == 0)
						info->entry->setSubscribedToPresence(false);
				}
			}
			// Alterning between name and value
			value = ! value;
		}		
	}
}

void contact_end_element_handler(void *userData, const char *name)
{
	contact_info* info = (contact_info*)userData;
	if(strcmp(name, "contact") == 0)
	{
		// Save contact, push in list
		info->contactList->push_back(info->contact);
	}
	if(strcmp(name, "entry") == 0)
	{
		// Save entry, push in contact
		info->contact->addEntry(info->entry);
	}
}

void
ContactManager::readContacts(std::string accountID, std::vector<Contact*>& contactList)
{
	std::string path;
	std::fstream file;
	char buf[4096];
	
	// Open XML file
	path = std::string(HOMEDIR) + DIR_SEPARATOR_STR + "." + PROGDIR	+ DIR_SEPARATOR_STR + accountID + "-contactlist.xml";
	file.open(path.data(), std::fstream::in);
	
	if(!file.is_open()) {
		return;
	}
	file.seekg(0, std::ios::end);
	int length = file.tellg();
	file.seekg(0, std::ios::beg);
	
	if(length == 0)
	{
		file.close();
		return;
	}

	// User data that will be used when parsing to remember
	// opened element and values for basic and note elements
	contact_info info;
	info.lastElement = "";
	info.contactList = &contactList;
	info.contact = NULL;
	info.entry = NULL;
	
	// Create parser for presence status provided in XML body
	XML_Parser parser = XML_ParserCreate(NULL);

	// Set user data that is a pointer that may be then used in the parsing functions
	XML_SetUserData(parser, &info);

	// Set functions that will be called when parsing doctype declarations, elements and character data
	// XML_SetDoctypeDeclHandler(parser, start_decl_handler, end_decl_handler);
	XML_SetElementHandler(parser, contact_start_element_handler, contact_end_element_handler);
	XML_SetCharacterDataHandler(parser,	contact_character_data_handler);

	// Parse the data
	while(!file.eof())
	{
		file.read(buf, 4096);
		XML_Parse(parser, buf, 4096, 0);
	}
	
	file.close();

	// Print error if any
//	printf("%s at line %d\n",
//			XML_ErrorString(XML_GetErrorCode(parser)),
//			XML_GetCurrentLineNumber(parser));

	// Free parser
	XML_ParserFree(parser);
}

void
ContactManager::saveContacts(std::string accountID, std::vector<Contact*>& contactList)
{
	std::fstream file;
	std::string path;
	
	path = std::string(HOMEDIR) + DIR_SEPARATOR_STR + "." + PROGDIR	+ DIR_SEPARATOR_STR + accountID + "-contactlist.xml";
	file.open(path.data(), std::fstream::out);
	
	if(!file.is_open()) {
		return;
	}
	
	_debug("Saving contacts in %s\n", path.data());
	file.seekg(0, std::ios::beg);
	
	// TODO Clear file before saving in it
	
	file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	file << "<contacts>" << std::endl;
	
	std::vector<Contact*>::iterator iter;
	for(iter = contactList.begin(); *iter; iter++)
	{
		Contact* contact = (Contact*)*iter;
		file << "\t<contact id=\"" << contact->getContactID() << "\">" << std::endl;
		file << "\t\t<firstName>" << contact->getFirstName() << "</firstName>" << std::endl;
		file << "\t\t<lastName>" << contact->getLastName() << "</lastName>" << std::endl;
		file << "\t\t<email>" << contact->getEmail() << "</email>" << std::endl;
		
		// Save all entries
		if(contact->getEntries().size() > 0)
		{
			file << "\t\t<entries>" << std::endl;
			std::vector<ContactEntry*> entries = contact->getEntries();
			std::vector<ContactEntry*>::iterator entryIter = entries.begin();
			while(entryIter != entries.end())
			{
				ContactEntry* entry = (ContactEntry*)*entryIter;
				file << "\t\t\t<entry id=\"" << entry->getEntryID() << "\" text=\"" << entry->getText() << "\" type=\"" << entry->getType();
				if(entry->getShownInCallConsole())
					file << "\" inConsole=\"true";
				else
					file << "\" inConsole=\"false";
				if(entry->getSubscribedToPresence())
					file << "\" subscribed=\"true";
				else
					file << "\" subscribed=\"false";
				file << "\" />" << std::endl;
				entryIter++;
			}
			file << "\t\t</entries>" << std::endl;
		}
		file << "\t</contact>" << std::endl;
	}
	file << "</contacts>" << std::endl;
	
	file.close();
}
