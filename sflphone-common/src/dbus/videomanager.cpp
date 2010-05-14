/*
 *  Copyright (C) 2010 Savoir-Faire Linux inc.
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

#include <videomanager.h>
#include "video/VideoInputSourceGst.h"

#include <vector>

const char* VideoManager::SERVER_PATH = "/org/sflphone/SFLphone/VideoManager";

VideoManager::VideoManager(DBus::Connection& connection) :
	DBus::ObjectAdaptor(connection, SERVER_PATH) {
}

std::vector<std::string> VideoManager::enumerateDevices()
{
	sfl::VideoInputSource* videoInputSource = new sfl::VideoInputSourceGst();

	std::vector<std::string> outputVector;

	std::vector<sfl::VideoDevice*> devices = videoInputSource->enumerateDevices();
	std::vector<sfl::VideoDevice*>::iterator it;
	for (it = devices.begin(); it < devices.end(); it++) {
		outputVector.push_back((*it)->getName());
	}

	return outputVector;
}

