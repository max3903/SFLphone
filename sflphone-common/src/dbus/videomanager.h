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

#ifndef _SFL_VIDEO_MANAGER_H_
#define _SFL_VIDEO_MANAGER_H_

#include "video/VideoInputSource.h"
#include "videomanager-glue.h"
#include "video/VideoInputSource.h"

#include <dbus-c++/dbus.h>
#include <map>

namespace sfl {
	class VideoEndpoint;
}

class VideoManager: public org::sflphone::SFLphone::VideoManager_adaptor,
		public DBus::IntrospectableAdaptor,
		public DBus::ObjectAdaptor {
public:

	VideoManager(DBus::Connection& connection);

	static const char* SERVER_PATH;
    static const char* SHM_ERROR_PATH;

	/**
	 * @return A list containing the names of all the available capture devices.
	 */
	std::vector<std::string> enumerateDevices();

	/**
	 * @param device The device for which to get this information.
	 * @return The list of supported resolutions for the device.
	 */
	std::vector< ::DBus::Struct< int32_t, int32_t > > getResolutionForDevice(const std::string& device);

	/**
	 * Find out what framerates are supported for a device under some resolution.
	 * @param device The name for the video device.
	 * @param width The corresponding width.
	 * @param height The existing height.
	 * @return The list of supported framerates for this resolution.
	 */
	std::vector<std::string> getFrameRates(const std::string& device, const int32_t& width, const int32_t& height);

	/**
	 * @param device The device to start capturing from. The resolution and framerate will be discovered automatically.
	 * @return The shared memory segment where the data can be obtained or /dev/null if a problem has occurred.
	 */
	std::string startLocalCapture(const std::string& device);

	/**
	 * @param device The device to start capturing from.
	 * @param width The source width.
	 * @param height The source height.
	 * @param fps The preferred frame rate, expressed as a ratio.
	 * @return The shared memory segment where the data can be obtained or /dev/null if a problem has occurred.
	 */
	std::string startLocalCapture(const std::string& device, const int32_t& width, const int32_t& height, const std::string& fps);

	/**
	 * @param device The device of interest for event notifications (frame capture).
	 * @return The namespace of a running instance of a file descriptor passer for the corresponding device.
	 */
	std::string getEventFdPasserNamespace(const std::string& device);

private :
	/**
	 * @param device The device corresponding to the VideoEndpoint
	 * @return The video endpoint corresponding to the given device.
	 */
	sfl::VideoEndpoint* getVideoEndpoint(const std::string& device) throw(sfl::UnknownVideoDeviceException);

	// Key : device name. Value : Corresponding VideoEndpoint
	std::map<std::string, sfl::VideoEndpoint*> videoEndpoints;

	// Key : device id. Value : Corresponding VideoDevice
	std::map<std::string, sfl::VideoDevicePtr> videoDevices;
};

#endif//CONTACTMANAGER_H
