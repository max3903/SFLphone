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
#include "video/VideoInputSource.h"

namespace sfl {

class FileDescriptorPasser;
class SharedMemoryPosix;

/**
 * This class is receiving video frames over RTP, writing them in
 * a dedicated shared memory segment, sending local frames from the VideoInputSource to
 * the remote RTP party as well as writing them in a shared memory segment for
 * displaying in the GUI.
 *
 * A video endpoint is unique to a given video source. In other words, only one source
 * can be defined in every instance, but multiple targets can be tied to it.
 */
class VideoEndpoint : public VideoFrameObserver
{
	public:

		/**
		 * Constructor for a video endpoint.
		 * @param src The mandatory video input source for this endpoint.
		 * @precondition The VideoInputSource must be configured to an existing video device.
		 * @see VideoInputSource#setDevice()
		 */
		VideoEndpoint(VideoInputSource* src);

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
		 * Capture from the local video device and write frames
		 * in a shared memory segment.
		 */
		void capture();

		/**
		 * @return The name for the shared memory allocated for frames capture.
		 */
		std::string getShmName();

		/**
		 * @return The name for the fd passer.
		 */
		std::string getFdPasserName();

		/**
		 * @Override
		 */
		void onNewFrame(const VideoFrame* frame);

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
		std::string getDigest(const std::string& name);

		static const useconds_t BUSY_WAIT_TIME = 500;

		VideoInputSource* videoSource;
		FileDescriptorPasser* sourceEventFdPasser;
		SharedMemoryPosix* shmVideoSource;

		int eventFileDescriptor;
};

}

#endif /* VIDEOENDPOINT_H_ */
