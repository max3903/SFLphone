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
#include "video/VideoEndpoint.h"
#include "logger.h"

#include <vector>

const char* VideoManager::SERVER_PATH = "/org/sflphone/SFLphone/VideoManager";
const char* VideoManager::SHM_ERROR_PATH = "/dev/null";

VideoManager::VideoManager(DBus::Connection& connection) :
	DBus::ObjectAdaptor(connection, SERVER_PATH)
{

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

	_debug("Enumerating devices");

	return outputVector;
}

std::string VideoManager::startLocalCapture(const std::string& device)
{
	_debug("Starting local capture on %s", device.c_str());

	// Do nothing if already capturing
	std::map<std::string, sfl::VideoEndpoint*>::iterator it = videoEndpoints.find(device);
	if (it != videoEndpoints.end()) {
		_debug((std::string("Device ") + std::string("is already opened.")).c_str());
		return SHM_ERROR_PATH;
	}

	// Create a new end point.
	sfl::VideoInputSourceGst* videoSource = new sfl::VideoInputSourceGst();
	videoSource->setDevice(device);

	// Start capturing
	try {
		videoSource->open(320, 240, 30);
	} catch (sfl::VideoDeviceIOException& e) {
		_debug ("Caught exception : %s", e.what());
		return SHM_ERROR_PATH;
	}

	sfl::VideoEndpoint* endpoint = new sfl::VideoEndpoint(videoSource);
	videoEndpoints.insert(std::pair<std::string, sfl::VideoEndpoint*>(device, endpoint));

	return endpoint->getShmName();
}

std::string VideoManager::getEventFdPasserNamespace(const std::string& device)
{
	sfl::VideoEndpoint* endpt;
	try {
		endpt = getVideoEndpoint(device);
	} catch (sfl::UnknownVideoDeviceException e) {
		_debug("%s", e.what());
	}

	_debug("**************** Returning passer namespace %s", endpt->getFdPasserName().c_str());
	return endpt->getFdPasserName();
}


sfl::VideoEndpoint* VideoManager::getVideoEndpoint(const std::string& device) throw(sfl::UnknownVideoDeviceException)
{
	std::map<std::string, sfl::VideoEndpoint*>::iterator it = videoEndpoints.find(device);
	if (it == videoEndpoints.end()) {
		throw sfl::UnknownVideoDeviceException("Could not find device " + device + " in getVideoEndpoint");
	}
	return it->second;
}
