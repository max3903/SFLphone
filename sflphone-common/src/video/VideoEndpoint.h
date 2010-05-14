/*
 * VideoEndpoint.h
 *
 *  Created on: 2010-05-10
 *      Author: pierre-luc
 */

#ifndef VIDEOENDPOINT_H_
#define VIDEOENDPOINT_H_

#include <pthread.h>

#include "util/SharedMemoryPosix.h"
#include "util/SemaphorePosix.h"
#include "video/VideoInputSource.h"

namespace sfl {

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
		 * @Override
		 */
		void onNewFrame(const VideoFrame* frame);

		VideoEndpoint();
		virtual ~VideoEndpoint();

	private:
		VideoInputSource* videoSource;

		SharedMemoryPosix* shmVideoSource;
		SharedMemoryPosix* shmRwLockVideoSource;

		pthread_rwlock_t readerWriterLock;
};

}

#endif /* VIDEOENDPOINT_H_ */
