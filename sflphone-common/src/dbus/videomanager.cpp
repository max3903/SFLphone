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

#include "video/source/VideoInputSourceGst.h"
#include "video/VideoEndpoint.h"
#include "video/source/VideoFormat.h"

#include "sip/sdp/sdp.h"

#include "logger.h"

#include <vector>
#include <algorithm>

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
	_debug ("Enumerating devices");

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
		std::vector<sfl::VideoFormat> formats =
		((*itDevice).second)->getFilteredFormats();

		std::vector<sfl::VideoFormat>::iterator itFormat;

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
		std::vector<sfl::VideoFormat> formats =
				((*itDevice).second)->getSupportedFormats();

		std::vector<sfl::VideoFormat>::iterator itFormat;

		for (itFormat = formats.begin(); itFormat < formats.end(); itFormat++) {
			if (((*itFormat).getWidth() == width) && ((*itFormat).getHeight()
					== height)) {

				_debug ("Found resolution %d x %d %s", (*itFormat).getWidth(), (*itFormat).getHeight(), (*itFormat).getMimetype().c_str());
				std::set<sfl::FrameRate> rates = (*itFormat).getFrameRates();
				std::set<sfl::FrameRate>::iterator itRate;

				for (itRate = rates.begin(); itRate != rates.end(); itRate++) {
					ratesList.push_back((*itRate).toString());
				}

				break;
			}
		}
	}

	std::reverse(ratesList.begin(), ratesList.end());
	return ratesList;
}

::DBus::Struct<std::string, std::string> VideoManager::startLocalCapture(
		const std::string& device, const int32_t& width, const int32_t& height,
		const std::string& fps) throw (DBus::VideoIOException) {
	_debug ("Starting video capture on DBus request.");
	// The code below deals with a device that is already capturing.
	DeviceNameToVideoEndpointIterator it = videoEndpoints.find(device);

	if (it != videoEndpoints.end()) {
		_debug ( (std::string ("Device ") + std::string ("is already mapped to some endpoint.")).c_str());

		sfl::VideoEndpoint* runningEndpoint = (*it).second;

		// Paranoid check. Should always be the case.
		if (!runningEndpoint->isCapturing()) {
			_error ("Mapped device but not capturing.");
			// throw DBus::VideoIOException(e);
		}

		::DBus::Struct<std::string, std::string> reply;
		reply._1 = runningEndpoint->getShmName();
		// Request additional token
		reply._2 = runningEndpoint->requestTokenForSource();

		_debug ("Sending reply %s with token %s", reply._1.c_str(), reply._2.c_str());

		return reply;
	}

	// The code below deals with creating a new endpoint.
	// Find the device
	DeviceIdToVideoDevicePtrIterator itDevice = videoDevices.find(device);

	// We know it's a GstVideoDevice
	sfl::GstVideoDevicePtr gstDevice = std::static_pointer_cast<
			sfl::GstVideoDevice, sfl::VideoDevice>((*itDevice).second);

	// Set the desired properties.
	sfl::VideoFormat format;
	format.setWidth(width);
	format.setHeight(height);
	format.setFramerate(fps);

	gstDevice->setPreferredFormat(format);

	// Create a new video source for that device.
	sfl::VideoInputSourceGst* videoSource = new sfl::VideoInputSourceGst();
	videoSource->setDevice((*itDevice).second);

	// Keep the alive endpoint in our internal list.
	sfl::VideoEndpoint* endpoint = new sfl::VideoEndpoint(videoSource);

	// Start capturing
	std::string token;

	try {
		token = endpoint->capture();
	} catch (sfl::VideoDeviceIOException& e) {
		_debug ("Caught exception : %s", e.what());
		throw DBus::VideoIOException(e);
	}

	// Record (Device Name : Endpoint)
	videoEndpoints.insert(DeviceNameToVideoEndpointEntry(device, endpoint));

	// Send the path to the SHM as well as token to get access to this resource.
	::DBus::Struct<std::string, std::string> reply;
	reply._1 = endpoint->getShmName();
	reply._2 = token;

	_debug ("Sending reply %s with token %s", reply._1.c_str(), reply._2.c_str());

	return reply;
}

void VideoManager::stopLocalCapture(const std::string& device,
		const std::string& token) throw (DBus::VideoIOException,
		DBus::InvalidTokenException) {
	std::map<std::string, sfl::VideoEndpoint*>::iterator it =
			videoEndpoints.find(device);

	_debug ("Stopping device %s with token %s", device.c_str(), token.c_str());

	if (it != videoEndpoints.end()) {
		try {
			((*it).second)->stopCapture(token);
		} catch (sfl::VideoDeviceIOException e) {
			_error ("Throwing VideoDeviceIOException over DBus because : %s", e.what());
			throw DBus::VideoIOException(e);
		} catch (sfl::InvalidTokenException e1) {
			_error ("Throwing InvalidTokenException over DBus because : %s", e1.what());
			throw DBus::InvalidTokenException(e1);
		}

		if (((*it).second)->isDisposable()) {
			delete ((*it).second);
			videoEndpoints.erase(it);
			_debug ("Endpoint was disposable. Removed.");
		}
	}

}

std::string VideoManager::getEventFdPasserNamespace(const std::string& device) {
	sfl::VideoEndpoint* endpt = NULL;
	try {
		endpt = getVideoEndpoint(device);
	} catch (sfl::UnknownVideoDeviceException e) {
		_error ("%s", e.what());
		// TODO Throw an exception
	}

	_debug ("Returning passer namespace %s", endpt->getFdPasserName().c_str());
	return endpt->getFdPasserName();
}

void VideoManager::stageRtpSession(SipCall* call) {
	_info("Staging video RTP session ...");

	DeviceNameToVideoEndpointIterator it = videoEndpoints.find(call->getVideoDevice());
	sfl::VideoEndpoint* endpoint;
	if (it == videoEndpoints.end()) {
		/* Create new endpoint for the given device. */

		// Find the VideoDevice to use.
		std::string device = call->getVideoDevice();
		DeviceIdToVideoDevicePtrIterator deviceIt = videoDevices.find(device);
		if (deviceIt == videoDevices.end()) {
			// TODO throw UnknownDeviceException
		}

		// Configure the VideoDevice with the preferred format, if any
		sfl::VideoFormat format = call->getVideoFormat();
		((*deviceIt).second)->setPreferredFormat(format);

		// Create a new video source for that device.
		sfl::VideoInputSourceGst* videoSource = new sfl::VideoInputSourceGst();
		videoSource->setDevice(((*deviceIt).second));

		// Keep the alive endpoint in our internal list.
		endpoint = new sfl::VideoEndpoint(videoSource);

		// Record (Device Name : Endpoint)
		videoEndpoints.insert(DeviceNameToVideoEndpointEntry(device, endpoint));
	} else {
		endpoint = (*it).second;
	}

	sfl::InetSocketAddress address(call->getLocalIp(), call->getLocalVideoPort());
	endpoint->createRtpSession(address);
}

void VideoManager::startRtpSession(SipCall* call, std::vector<const sfl::VideoCodec*> negotiatedCodecs) {
	_info("Starting video RTP session ...");

	DeviceNameToVideoEndpointIterator it = videoEndpoints.find(call->getVideoDevice());
	if (it == videoEndpoints.end()) {
		// TODO Throw UnstagedRtpSessionException
		_error("Cannot find a video endpoint for device %s", call->getVideoDevice().c_str());
		return;
	}

	// Add a destination
	sfl::InetSocketAddress localAddress(call->getLocalIp(), call->getLocalVideoPort());
    sfl::InetSocketAddress destinationAddress(call->getLocalSDP()->getRemoteIp(), call->getLocalSDP()->getRemoteVideoPort());

	sfl::VideoEndpoint* endpoint = (*it).second;
    endpoint->addDestination(localAddress, destinationAddress);

    // Configure the RTP session with the given codec list (at least of size 1) and start sending data.
    endpoint->startRtpSession(localAddress, negotiatedCodecs);
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