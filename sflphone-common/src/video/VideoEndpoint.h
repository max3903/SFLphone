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

#include "util/InetSocketAddress.h"
#include "util/FileDescriptorPasser.h"
#include "util/ipc/SharedMemoryPosix.h"
#include "video/source/VideoInputSource.h"
#include "video/rtp/VideoRtpSessionSimple.h"

#include <set>
#include <map>
#include <stdexcept>

#include <sys/eventfd.h>

namespace sfl
{

class FileDescriptorPasser;

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
 * This exception is thrown whenever a user attempts to
 * refer to some shared memory segment that does not
 * exists.
 */
class NoSuchShmException: public std::invalid_argument
{
    public:
	NoSuchShmException (const std::string& msg) :
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
         * @param address The address to listen on.
         * @postcondition An RTP session will be available for future calls referencing to "local".
         * TODO Support secured sessions.
         */
        void createRtpSession(const sfl::InetSocketAddress& address);

        /**
         * Add a unicast destination for receiving and sending video data.
         * @param local The local address on which some RTP session is bound to.
         * @param address The destination address to add to the RTP session.
         * @precondition A VideoRtpSession bound to "local" should be available.
         * @see sfl#VideoEndpoint#createRtpsession
         */
        void addDestination(const InetSocketAddress& localAddress, const InetSocketAddress& destinationAddress);

        /**
         * @param local The local address to bind to.
         * @param negotiatedCodecs A codec list of size at least 1, containing the supported and negotiated video codecs.
         * @precondition A VideoRtpSession bound to "local" should be available.
         * @return The shared memory name where the remote video frames get written to.
         * @see sfl#VideoEndpoint#createRtpsession
         */
        std::string startRtpSession(const InetSocketAddress& local, std::vector<const sfl::VideoCodec*> negotiatedCodecs);

        /**
         * @return The RTP session bound to the given socket.
         * TODO To implement (needed for SRTP, to retrieve crypto params).
         */
        sfl::VideoRtpSessionSimple* getRtpSession(sfl::InetSocketAddress local);

        /**
         * Start local capture, and issuing a token for accessing a given video source.
         * When a token is given to a client, the server guarantees that it won't closes a
         * video source unless all tokens have been handed back.
         *
         * The form of the token is left unspecified, but is guaranteed to be unique.
         *
         * @return A unique token for the requester.
         * @throw VideoDeviceIOException If an error occur while starting video capture.
         * @postcondition getSourceDeviceShmName() will return the name for the shared memory segment.
         * @see VideoEndpoint#getSourceDeviceShmName
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
         * @param shm The shared memory segment name to look presence for.
         * @return true if the the given shared memory segment is owned by this object.
         */
        bool hasShm(const std::string& shm);

        /**
         * @return The name for the shared memory allocated for frames capture.
         */
        std::string getSourceDeviceShmName();

        /**
         * @param shm A shared memory segment owned by this endpoint.
         * @return A video format for the frames that are written into the given
         * @precondition The given shared memory segment must exists, and be owned
         * by the the current object.
         * @throw NoSuchShmException if the shared memory segment cannot be found in this object.
         */
        sfl::VideoFormat getShmVideoFormat(const std::string& shm) throw(NoSuchShmException);

        /**
         * @return The name for the fd passer.
         * @throw NoSuchShmException if the shared memory segment cannot be found in this object.
         */
        std::string getFdPasserName() throw(NoSuchShmException);

        /**
         * @return The name for the fd passer for the given shm.
         * @precondition The given shm must be existing.
         * @throw NoSuchShmException if the shared memory segment cannot be found in this object.
         */
        std::string getFdPasserName(const std::string& shm) throw(NoSuchShmException);

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
         * Set to "org.sflphone.eventfd.source" by default.
         */
        static const std::string EVENT_SOURCE_NAMESPACE;

        /**
         * Set to "org.sflphone.eventfd.rtp" by default.
         */
        static const std::string EVENT_STREAM_NAMESPACE;

        VideoEndpoint();
        virtual ~VideoEndpoint();

    private:
        /**
         * Broadcast a video event belonging to the source device.
         */
        void broadcastNewFrameEvent();

        /**
         * Send a video frame accross multiple RTP sessions.
         * @param frame The frame to encode then send over RTP.
         */
        void sendInAllRtpSession(const VideoFrame* frame);

        /**
         * Compute a hash digest for the given name.
         */
        std::string getDigest (const std::string& name);

        /**
         * @return a unique token.
         */
        std::string generateToken();

        /**
         * This observer type is used for writing asynchronously into
         * the shared memory segment allocated for a given RTP session.
         */
        class RtpStreamDecoderObserver: public VideoFrameDecodedObserver {
        public:
        	RtpStreamDecoderObserver(int fd, SharedMemoryPosix* shm) :
        	 fd(fd), shm(shm) {}

        	void onNewFrameDecoded (Buffer<uint8_t>& data) {
        		if (data.getBuffer() == NULL) {
        			_error ("Null frame in RtpStreamDecoderObserver"); // FIXME Should not happen.
        			return;
        		}

        		// Make sure that the shared memory is still big enough to hold the new frame
        		// Should be required once.
        		if (shm->getSize() != data.getSize()) {
        			_debug ("Truncating to %d", data.getSize());
        			shm->truncate(data.getSize());
        		}

        		// Write into the shared memory segment
        		memcpy(shm->getRegion(), data.getBuffer(), data.getSize());

        		eventfd_write(fd, NEW_FRAME_EVENT);

        		//_debug("Wrote decoded frame of size %d in shm segment %s", data.getSize(), shm->getName().c_str());
        	}
        private:
        	int fd;
        	SharedMemoryPosix* shm;
        };

        /**
         * This class is used to hold various information
         * tied to an active RTP session.
         */
        class RtpSessionRecord {
        public:
        	RtpSessionRecord(SharedMemoryPosix* shm,
        			FileDescriptorPasser* passer,
        			VideoRtpSessionSimple* session,
        			RtpStreamDecoderObserver* observer) :
        	shm(shm), passer(passer), session(session), observer(observer){}

            inline FileDescriptorPasser* getFileDescriptorPasser() const
            {
                return passer;
            }

            inline VideoRtpSessionSimple* getVideoRtpSession() const
            {
                return session;
            }

            inline SharedMemoryPosix* getSharedMemoryPosix() const
            {
                return shm;
            }

            inline RtpStreamDecoderObserver* getDecoderObserver() const
            {
                return observer;
            }

            inline int getEventFd() const {
            	return passer->getFileDescriptor();
            }

        private:
        	SharedMemoryPosix* shm;
        	FileDescriptorPasser* passer;
        	VideoRtpSessionSimple* session;
        	RtpStreamDecoderObserver* observer;
        };

        std::map<InetSocketAddress, RtpSessionRecord> socketAddressToRtpSessionRecord;
        typedef std::map<InetSocketAddress, RtpSessionRecord>::iterator SocketAddressToRtpSessionRecordIterator;
        typedef std::pair<InetSocketAddress, RtpSessionRecord> SocketAddressToRtpSessionRecordEntry;

        /**
         * Used in std::find_if
         */
        struct HasSameShmName
        {
          explicit HasSameShmName(const std::string& name) : shm(name) {}

          bool operator() (const std::pair<InetSocketAddress, RtpSessionRecord>& record) const
          { return record.second.getSharedMemoryPosix()->getName() == shm; }

          const std::string& shm;
        };

        // The token set
        std::set<std::string> sourceTokens;

        // The *unique* video source for this endpoint
        VideoInputSource* videoSource;

        // Instance of a local server in the UNIX domain for passing FD.
        FileDescriptorPasser* sourceEventFdPasser;

        // The shared memory segment where frames are written to
        SharedMemoryPosix* shmVideoSource;

        int eventFileDescriptorSource;

        bool capturing;

        static const useconds_t BUSY_WAIT_TIME = 500;
};

}

#endif /* VIDEOENDPOINT_H_ */
