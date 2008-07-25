/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Florian Desportes <florian.desportes@savoirfairelinux.com>
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

//#ifdef USE_VOICEMAIL

#include "voicemailmanager.h"
#include "../manager.h"

const char* VoicemailManager::SERVER_PATH = "/org/sflphone/SFLphone/VoicemailManager";

VoicemailManager::VoicemailManager(DBus::Connection& connection)
		: DBus::ObjectAdaptor(connection, SERVER_PATH) {
}

VoicemailManager::~VoicemailManager() {
}

void
VoicemailManager::openConnection(void) {
	Manager::instance().openConnection();
}

void
VoicemailManager::closeConnection(void) {
	Manager::instance().destroyVoicemailViewer();
}

std::vector< ::DBus::String >
VoicemailManager::getListFolders(void) {
	return Manager::instance().getListFolders();
}

int
VoicemailManager::getFolderCount(const ::DBus::String& folder) {
	return Manager::instance().getFolderCount(folder);
}


std::vector< ::DBus::String >
VoicemailManager::getListMails(const ::DBus::String& folder) {
	return Manager::instance().getListMails(folder);
}

std::vector< ::DBus::String >
VoicemailManager::getListErrors(void) {
	return Manager::instance().getListErrors();
}

::DBus::String
VoicemailManager::getVoicemailInfo(const ::DBus::String& folderName, const ::DBus::String& voicemailName) {
	return Manager::instance().getVoicemailInfo(folderName, voicemailName);
}

void
VoicemailManager::playVoicemail(const ::DBus::String& folderName, const ::DBus::String& voicemailName) {
	Manager::instance().playVoicemail(folderName, voicemailName);
}

void
VoicemailManager::stopVoicemail(void) {
	Manager::instance().stopVoicemail();
}


/***************************************************/
// VOICEMAIL CONFIGURATION 
/***************************************************/
::DBus::Bool
VoicemailManager::isVoicemailServerEnabled(void) {
	return Manager::instance().isVoicemailServerEnabled();
}

void
VoicemailManager::voicemailServerEnable(void) {
	Manager::instance().voicemailServerEnable();
}

::DBus::String
VoicemailManager::getVoicemailConfigAddress(void) {
	return Manager::instance().getVoicemailConfigAddress();
}

void
VoicemailManager::setVoicemailConfigAddress(const ::DBus::String& address) {
	return Manager::instance().setVoicemailConfigAddress(address);
}

::DBus::String
VoicemailManager::getVoicemailConfigPath(void) {
	return Manager::instance().getVoicemailConfigPath();
}

void
VoicemailManager::setVoicemailConfigPath(const ::DBus::String& path) {
	return Manager::instance().setVoicemailConfigPath(path);
}

::DBus::Int32
VoicemailManager::getVoicemailConfigPort(void) {
	return Manager::instance().getVoicemailConfigPort();
}

void
VoicemailManager::setVoicemailConfigPort(const ::DBus::Int32& port) {
	return Manager::instance().setVoicemailConfigPort(port);
}

::DBus::Bool
VoicemailManager::isVoicemailConfigHttpsEnabled(void) {
	return Manager::instance().isVoicemailConfigHttpsEnabled();
}

void
VoicemailManager::voicemailConfigHttpsEnable(const ::DBus::Bool& enabled) {
	Manager::instance().setVoicemailConfigHttps(enabled);
}
//#end
