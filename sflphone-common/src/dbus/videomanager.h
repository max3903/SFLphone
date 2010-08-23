/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#ifndef _SFL_VIDEO_MANAGER_H_
#define _SFL_VIDEO_MANAGER_H_

#include "videomanager-glue.h"
#include "video/VideoEndpoint.h"
#include "video/source/VideoInputSource.h"

#include "sip/sipcall.h"

#include <map>
#include <dbus-c++/dbus.h>

namespace sfl
{
class VideoEndpoint;
}

/**
 * Note that this exception is not the same as the one
 * in the "sfl" namespace. This exception is thrown
 * over dbus.
 */
namespace DBus
{

/**
 * @see sfl#VideoDeviceIOException
 */
class VideoIOException : public DBus::Error
{
    public:
        VideoIOException (const sfl::VideoDeviceIOException e) :
                DBus::Error ("VideoIOException", e.what()) {
        }
};

/**
 * @see sfl#InvalidTokenException
 */
class InvalidTokenException : public DBus::Error
{
    public:
        InvalidTokenException (const sfl::InvalidTokenException e) :
                DBus::Error ("InvalidTokenException", e.what()) {
        }
};

}

class VideoManager: public org::sflphone::SFLphone::VideoManager_adaptor,
        public DBus::IntrospectableAdaptor,
        public DBus::ObjectAdaptor
{
    public:

        VideoManager (DBus::Connection& connection);

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
        std::vector< ::DBus::Struct< int32_t, int32_t > > getResolutionForDevice (const std::string& device);

        /**
         * Find out what framerates are supported for a device under some resolution.
         * @param device The name for the video device.
         * @param width The corresponding width.
         * @param height The existing height.
         * @return The list of supported framerates for this resolution.
         */
        std::vector<std::string> getFrameRates (const std::string& device, const int32_t& width, const int32_t& height);

        /**
         * @param device The device to start capturing from.
         * @param width The source width.
         * @param height The source height.
         * @param fps The preferred frame rate, expressed as a ratio.
         * @throw DBus#VideoIOException If a error occur while stopping video.
         * @return A path to the shared memory segment, followed by a token for referring to this request when calling stop.
         */
        ::DBus::Struct< std::string, std::string > startLocalCapture (const std::string& device, const int32_t& width, const int32_t& height, const std::string& fps) throw (DBus::VideoIOException);

        /**
         * @param token The token that was obtained when startLocalCapture() was called.
         * @throw DBus#VideoIOException If a error occur while stopping video.
         * @throw DBus#InvalidTokenException If the token that was provided is invalid.
         * @see VideoManager#startLocalCapture
         */
        void stopLocalCapture (const std::string& device, const std::string& token) throw (DBus::VideoIOException, DBus::InvalidTokenException);

        /**
         * @param device The device of interest for event notifications (frame capture).
         * @return The namespace of a running instance of a file descriptor passer for the corresponding device.
         */
        std::string getEventFdPasserNamespace (const std::string& device);

        /**
         * Kept unexposed over DBus.
         *
         * @param call The SipCall object containing the SdpSession.
         * Instantiate (if needed) and prepare a VideoRtpSession for sending data.
         * Once the the SDP answer is received, the session will be configured with the negotiated codec.
         */
        void stageRtpSession(SipCall* call);

        /**
         * Kept unexposed over DBus.
         * @param call The SipCall object containing the SdpSession.
         * @param negotiatedCodecs A list containing at least one negotiated VideoCodec.
         */
        void startRtpSession(SipCall* call, std::vector<const sfl::VideoCodec*> negotiatedCodecs);

        /**
         * Stop the current RTP session, releasing access to the video device.
         * @param The SipCall for which an RTP session must be stopped.
         * @precondition An RTP session should be already running. #startRtpSession() has to be started prior to that.
         * @see #startRtpSession()
         */
        void stopRtpSession(SipCall* call);

    private :
        /**
         * @param device The device corresponding to the VideoEndpoint
         * @return The video endpoint corresponding to the given device.
         */
        sfl::VideoEndpoint* getVideoEndpoint (const std::string& device) throw (sfl::UnknownVideoDeviceException);

        // Key : device name. Value : Corresponding VideoEndpoint
        std::map<std::string, sfl::VideoEndpoint*> videoEndpoints;
        typedef std::map<std::string, sfl::VideoEndpoint*>::iterator DeviceNameToVideoEndpointIterator;
        typedef std::pair<std::string, sfl::VideoEndpoint*> DeviceNameToVideoEndpointEntry;

        // Key : device id. Value : Corresponding VideoDevice
        std::map<std::string, sfl::VideoDevicePtr> videoDevices;
        typedef	std::map<std::string, sfl::VideoDevicePtr>::iterator DeviceIdToVideoDevicePtrIterator;
        typedef	std::pair<std::string, sfl::VideoDevicePtr> DeviceIdToVideoDevicePtrEntry;
};

#endif//CONTACTMANAGER_H
