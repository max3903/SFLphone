/*
 *  Copyright (C) 2006-2010 Savoir-Faire Linux inc.
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

#ifndef __SFL_VIDEO_ENDPOINT_H___
#define __SFL_VIDEO_ENDPOINT_H___

#include <sys/eventfd.h>
#include "video/source/VideoInputSource.h"

#include <set>
#include <stdexcept>

namespace sfl
{

class FileDescriptorPasser;
class SharedMemoryPosix;

/**
 * This exception is thrown when an invalid token is passed to some method.
 * Invalidity is not specified. Can be syntactical or semantical.
 */
class InvalidTokenException: public std::invalid_argument
{
    public:
        InvalidTokenException (const std::string& msg) :
                std::invalid_argument (msg) {
        }
};

/**
 * This class is receiving video frames over RTP, writing them in
 * a dedicated shared memory segment, sending local frames from the VideoInputSource to
 * the remote RTP party as well as writing them in a shared memory segment for
 * displaying in the GUI.
 *
 * A video endpoint is unique to a given video source. In other words, only one source
 * can be defined in every instance, but multiple targets can be tied to it.
 */
class VideoEndpoint: public VideoFrameObserver
{
    public:

        /**
         * Constructor for a video endpoint.
         * @param src The mandatory video input source for this endpoint.
         * @precondition The VideoInputSource must be configured to an existing video device.
         * @see VideoInputSource#setDevice()
         */
        VideoEndpoint (VideoInputSource* src);

        /**
         * @return The video input source for this endpoint.
         */
        VideoInputSource* getVideoInputSource();

        /**
         * Sends the VideoInputSource to the remote RTP peer.
         */
        void sendRtpData();

        /**
         * Receive video from remote RTP peer.
         */
        void receiveRtpData();

        /**
         * @return The name for the shared memory allocated for frames capture.
         */
        std::string getShmName();

        /**
         * Start local capture, and issuing a token for accessing a given video source.
         * When a token is given to a client, the server guarantees that it won't closes a
         * video source unless all tokens have been handed back.
         *
         * The form of the token is left unspecified, but is guaranteed to be unique.
         *
         * @return A unique token for the requester.
         * @throw VideoDeviceIOException If an error occur while starting video capture.
         * @postcondition getShmName() will return the name for the shared memory segment.
         * @see VideoEndpoint#getShmName
         */
        std::string capture() throw (VideoDeviceIOException);

        /**
         * Release access to the video input source. If all tokens are handed back, the video
         * subsystem will be instructed to stop capturing from the source device.
         * @precondition The token must be existing (ie was issued by this same endpoint).
         * @postcondition The token is no more valid. If all tokens are handed back, capturing stops.
         * @param token The token that was obtained either from capture() or requestTokenForSource()
         * @throw VideoDeviceIOException If an unrecoverable error occur while stopping video capture.
         * @throw InvalidTokenException If the token that was passed is not valid.
         * @see VideoEndpoint#capture
         * @see VideoEndpoint#requestTokenForSource
         */
        void stopCapture (std::string token) throw (VideoDeviceIOException,
                InvalidTokenException);

        /**
         * Issue a new token for an existing and running video input source.
         * @return a unique token
         * @precondition The video input source should be mapped in memory.
         * @postcondition The token that was issued will be kept in memory for further operations.
         * @see VideoEndpoint#capture
         */
        std::string requestTokenForSource();

        /**
         * @return The name for the fd passer.
         */
        std::string getFdPasserName();

        /**
         * @return true if this endpoint is capturing video from the local device.
         */
        bool isCapturing();

        /**
         * @return true if this endpoint can be destroyed. This is true when no capture is
         * taking place and that no session is established over RTP.
         */
        bool isDisposable();

        /**
         * @Override
         */
        void onNewFrame (const VideoFrame* frame);

        /**
         * eventfd() constant that represents the arrival of a new frame.
         */
        static const eventfd_t NEW_FRAME_EVENT = 1;

        /**
         * Set to "org.sflphone.event" by default.
         */
        static const std::string EVENT_NAMESPACE;

        VideoEndpoint();
        virtual ~VideoEndpoint();

    private:
        /**
         * Broadcast a video event belonging to the source device.
         */
        void broadcastNewFrameEvent();

        /**
         * Compute a hash digest for the given name.
         */
        std::string getDigest (const std::string& name);

        /**
         * @return a unique token.
         */
        std::string generateToken();

        static const useconds_t BUSY_WAIT_TIME = 500;
        std::set<std::string> sourceTokens;
        VideoInputSource* videoSource;
        FileDescriptorPasser* sourceEventFdPasser;
        SharedMemoryPosix* shmVideoSource;
        int eventFileDescriptor;
        bool capturing;
};

}

#endif /* VIDEOENDPOINT_H_ */
