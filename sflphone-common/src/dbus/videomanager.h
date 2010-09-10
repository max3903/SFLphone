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
 * @see sfl#UnknownVideoDeviceException
 */
class UnknownVideoDeviceException : public DBus::Error
{
    public:
	UnknownVideoDeviceException (const sfl::UnknownVideoDeviceException e) :
                DBus::Error ("UnknownVideoDeviceException", e.what()) {
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

/**
 * @see sfl#NoSuchShmException
 */
class NoSuchShmException : public DBus::Error
{
    public:
	NoSuchShmException (const std::string& msg) :
                DBus::Error ("NoSuchShmException", msg.c_str()) {
        }
	NoSuchShmException (const sfl::NoSuchShmException e) :
                DBus::Error ("NoSuchShmException", e.what()) {
        }
};

}

/**
 * A structure containing video information about a given shared memory segment.
 * Fields are of the form :
 * 	- width
 * 	- height
 *  - fourcc
 */
typedef ::DBus::Struct<uint32_t, uint32_t, uint32_t> DbusVideoShmInfo;

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
         * @param shm The shared memory segment.
         * @return The namespace of a running instance of a file descriptor passer for the corresponding shm.
         * @throw DBus::NoSuchShmException if the given shared memory segment can't be found.
         */
        std::string getEventFdPasserNamespace (const std::string& shm) throw(DBus::NoSuchShmException) ;

        /**
         * @param shm The path to an existing shared memory segment.
         * @return A structure describing the shared memory segment. This one contains, in the following order :
         *	- width
         *	- height
         *	- fourcc
         * @precondition The memory segment must have been created by the application prior to this call.
         * @throw DBus::NoSuchShmException if the given shared memory segment can't be found.
         */
        DbusVideoShmInfo getShmInfo(const std::string& shm) throw(DBus::NoSuchShmException);

        /**
         * Kept unexposed over DBus.
         *
         * @param call The SipCall object containing the SdpSession.
         * Instantiate (if needed) and prepare a VideoRtpSession for sending data.
         * Once the the SDP answer is received, the session will be configured with the negotiated codec.
         * @throw sfl::UnknownVideoDeviceException if the specified video device name in the SipCall cannot be found
         * in the list of known devices in the VideoManager.
         */
        void stageRtpSession(SipCall* call) throw(sfl::UnknownVideoDeviceException);

        /**
         * Kept unexposed over DBus.
         * @param call The SipCall object containing the SdpSession.
         * @param negotiatedCodecs A list containing at least one negotiated VideoCodec.
         */
        void startRtpSession(SipCall* call, std::vector<const sfl::VideoCodec*> negotiatedCodecs);

        /**
         * Kept unexposed over DBus.
         * Stop the current RTP session, releasing access to the video device.
         * @param The SipCall for which an RTP session must be stopped.
         * @precondition An RTP session should be already running. #startRtpSession() has to be started prior to that.
         * @see #startRtpSession()
         */
        void stopRtpSession(SipCall* call);

        /**
         * @return true if the instance has/knowns a video device with the given name.
         */
        bool hasDevice(const std::string& name) const;

        /**
         * @return The name for the default video device.
         * @throw sfl::NoVideoDeviceAvailableException if no video device is known to the video manager.
         */
        std::string getDefaultDevice() const throw(sfl::NoVideoDeviceAvailableException);

    private :
        /**
         * @param device The device corresponding to the VideoEndpoint
         * @return The video endpoint corresponding to the given device.
         */
        sfl::VideoEndpoint* getVideoEndpoint (const std::string& device) throw (sfl::UnknownVideoDeviceException);

        /**
         * Observer type for a given VideoEndpoint.
         */
        class EndpointObserver : public sfl::VideoEndpointObserver {
        public:
        	EndpointObserver(VideoManager* parent, CallId callId) : parent(parent), callId(callId) {}
        	/**
        	 * @Override
        	 */
        	void onRemoteVideoStreamStarted(const std::string& shm) {
        		if (callId != "") {
        			_debug("Sending signal onRemoteVideoStreamStarted for shm %s and callid %s", shm.c_str(), callId.c_str());
        			parent->onNewRemoteVideoStream(callId, shm);
        		}
        	}
        private:
        	VideoManager* parent;
        	CallId callId;
        };

        /**
         * This class is used for maintaining various information related to
         * the VideoEndoint.
         */
        class EndpointRecord {
        public:
        	explicit EndpointRecord(sfl::VideoEndpoint* endpoint, sfl::VideoEndpointObserver* observer) :
        	endpoint(endpoint), observer(observer) {}

        	sfl::VideoEndpoint* getVideoEndpoint() const { return endpoint; }

        	sfl::VideoEndpointObserver* getVideoEndpointObserver() const { return observer; }

        	sfl::VideoEndpoint* operator ->() const {
        		return endpoint;
        	}
        private:
        	sfl::VideoEndpoint* endpoint;
        	sfl::VideoEndpointObserver* observer;
        };

        /**
         * Helper predicate used for std::find_if
         */
        struct HasSameShmName
        {
          explicit HasSameShmName(const std::string& name) : shm(name) {}

          bool operator() (const std::pair<std::string, EndpointRecord>& endpoint) const
          { return ((endpoint.second).getVideoEndpoint())->hasShm(shm); }

          const std::string& shm;
        };

        // Key : device name. Value : Corresponding VideoEndpoint
        std::map<std::string, EndpointRecord> videoEndpoints;
        typedef std::map<std::string, EndpointRecord>::iterator DeviceNameToEndpointRecordIterator;
        typedef std::pair<std::string, EndpointRecord> DeviceNameToEndpointRecord;

        // Key : device id. Value : Corresponding VideoDevice
        std::map<std::string, sfl::VideoDevicePtr> videoDevices;
        typedef	std::map<std::string, sfl::VideoDevicePtr>::iterator DeviceIdToVideoDevicePtrIterator;
        typedef	std::map<std::string, sfl::VideoDevicePtr>::const_iterator DeviceIdToVideoDevicePtrConstIterator;
        typedef	std::pair<std::string, sfl::VideoDevicePtr> DeviceIdToVideoDevicePtrEntry;
};

#endif//CONTACTMANAGER_H
