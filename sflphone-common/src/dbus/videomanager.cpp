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

#include "videomanager.h"
#include "video/VideoInputSourceGst.h"
#include "video/VideoEndpoint.h"
#include "video/FrameFormat.h"

#include "logger.h"

#include <vector>

const char* VideoManager::SERVER_PATH = "/org/sflphone/SFLphone/VideoManager";
const char* VideoManager::SHM_ERROR_PATH = "/dev/null";

VideoManager::VideoManager(DBus::Connection& connection) :
	DBus::ObjectAdaptor(connection, SERVER_PATH) {
	sfl::VideoInputSource* videoInputSource = new sfl::VideoInputSourceGst();
	std::vector<sfl::VideoDevicePtr> devicesList =
			videoInputSource->enumerateDevices();

	// TODO make this refresh when device is disconnected.
	std::vector<sfl::VideoDevicePtr>::iterator it;
	for (it = devicesList.begin(); it < devicesList.end(); it++) {
		videoDevices.insert(std::pair<std::string, sfl::VideoDevicePtr>(
				(*it)->getName(), (*it)));
	}
}

std::vector<std::string> VideoManager::enumerateDevices() {
	_debug("Enumerating devices");

	std::vector<std::string> devices;

	std::map<std::string, sfl::VideoDevicePtr>::iterator it;
	for (it = videoDevices.begin(); it != videoDevices.end(); it++) {
		devices.push_back(((*it).second)->getName());
	}

	return devices;
}

std::vector< ::DBus::Struct<int32_t, int32_t> > VideoManager::getResolutionForDevice(
		const std::string& device) {
	std::vector< ::DBus::Struct<int32_t, int32_t> > resolutions;
	std::map<std::string, sfl::VideoDevicePtr>::iterator itDevice =
			videoDevices.find(device);

	if (itDevice != videoDevices.end()) {
		std::vector<sfl::FrameFormat> formats =
				((*itDevice).second)->getFilteredFormats();

		std::vector<sfl::FrameFormat>::iterator itFormat;
		for (itFormat = formats.begin(); itFormat < formats.end(); itFormat++) {
			::DBus::Struct<int32_t, int32_t> resolutionStruct;
			resolutionStruct._1 = (*itFormat).getWidth();
			resolutionStruct._2 = (*itFormat).getHeight();
			resolutions.push_back(resolutionStruct);
		}
	}

	return resolutions;
}

std::vector<std::string> VideoManager::getFrameRates(const std::string& device,
		const int32_t& width, const int32_t& height) {
	std::vector<std::string> ratesList;

	std::map<std::string, sfl::VideoDevicePtr>::iterator itDevice =
			videoDevices.find(device);

	if (itDevice != videoDevices.end()) {
		std::vector<sfl::FrameFormat> formats =
				((*itDevice).second)->getSupportedFormats();

		std::vector<sfl::FrameFormat>::iterator itFormat;
		for (itFormat = formats.begin(); itFormat < formats.end(); itFormat++) {
			if (((*itFormat).getWidth() == width) && ((*itFormat).getHeight()
					== height)) {

				_debug("Found resolution %d x %d %s", (*itFormat).getWidth(), (*itFormat).getHeight(), (*itFormat).getMimetype().c_str());
				std::vector<sfl::FrameRate> rates = (*itFormat).getFrameRates();
				std::vector<sfl::FrameRate>::iterator itRate;
				for (itRate = rates.begin(); itRate < rates.end(); itRate++) {
					ratesList.push_back((*itRate).toString());
				}

				break;
			}
		}
	}

	return ratesList;
}

std::string VideoManager::startLocalCapture(const std::string& device) {
	_debug("Starting local capture on %s", device.c_str());

	// Return the SHM if already running.
	std::map<std::string, sfl::VideoEndpoint*>::iterator it =
			videoEndpoints.find(device);
	if (it != videoEndpoints.end()) {
		_debug((std::string("Device ") + std::string("is already opened.")).c_str());
		return (*it).second->getShmName();
	}

	// Create a new end point.
	sfl::VideoInputSourceGst* videoSource = new sfl::VideoInputSourceGst();
	videoSource->setDevice(device);

	// Start capturing
	try {
		videoSource->open();
	} catch (sfl::VideoDeviceIOException& e) {
		_debug ("Caught exception : %s", e.what());
		return SHM_ERROR_PATH;
	}

	// Keep the alive endpoint in our internal list.
	sfl::VideoEndpoint* endpoint = new sfl::VideoEndpoint(videoSource);
	videoEndpoints.insert(std::pair<std::string, sfl::VideoEndpoint*>(device,
			endpoint));

	return endpoint->getShmName();
}

std::string VideoManager::startLocalCapture(const std::string& device, const int32_t& width,
		const int32_t& height, const std::string& fps) {

	// Return the SHM if already running.
	std::map<std::string, sfl::VideoEndpoint*>::iterator it =
			videoEndpoints.find(device);

	if (it != videoEndpoints.end()) {
		_debug((std::string("Device ") + std::string("is already opened.")).c_str());
		return (*it).second->getShmName();
	}

	// Find the device
	std::map<std::string, sfl::VideoDevicePtr>::iterator itDevice =
			videoDevices.find(device);

	// Set the desired properties.
	sfl::FrameFormat format;
	format.setWidth(width);
	format.setHeight(height);
	format.setFramerate(fps);
	((*itDevice).second)->setPreferredFormat(format);

	// Create a new video source for that device.
	sfl::VideoInputSourceGst* videoSource = new sfl::VideoInputSourceGst();
	videoSource->setDevice((*itDevice).second);

	// Start capturing
	try {
		videoSource->open();
	} catch (sfl::VideoDeviceIOException& e) {
		_debug ("Caught exception : %s", e.what());
		return SHM_ERROR_PATH;
	}

	// Keep the alive endpoint in our internal list.
	sfl::VideoEndpoint* endpoint = new sfl::VideoEndpoint(videoSource);
	videoEndpoints.insert(std::pair<std::string, sfl::VideoEndpoint*>(device,
			endpoint));

	return endpoint->getShmName();

}

std::string VideoManager::getEventFdPasserNamespace(const std::string& device) {
	sfl::VideoEndpoint* endpt;
	try {
		endpt = getVideoEndpoint(device);
	} catch (sfl::UnknownVideoDeviceException e) {
		_debug("%s", e.what());
	}

	_debug("Returning passer namespace %s", endpt->getFdPasserName().c_str());
	return endpt->getFdPasserName();
}

sfl::VideoEndpoint* VideoManager::getVideoEndpoint(const std::string& device)
		throw (sfl::UnknownVideoDeviceException) {
	std::map<std::string, sfl::VideoEndpoint*>::iterator it =
			videoEndpoints.find(device);
	if (it == videoEndpoints.end()) {
		throw sfl::UnknownVideoDeviceException("Could not find device "
				+ device + " in getVideoEndpoint");
	}
	return it->second;
}
