/*
 * VideoEndpoint.cpp
 *
 *  Created on: 2010-05-10
 *      Author: pierre-luc
 */

#include "VideoEndpoint.h"
#include "VideoInputSourceGst.h"
#include "logger.h"

namespace sfl
{

VideoEndpoint::VideoEndpoint(VideoInputSource* src) {
	videoSource = src;

	// Register this object as a frame observer
	videoSource->addVideoFrameObserver(this);

	// Create a shared memory segment for video
	_debug((std::string("Creating ") + std::string("/sflphone-shm-") + src->getDevice()->toString()).c_str());
	shmVideoSource = new SharedMemoryPosix("/sflphone-shm-device1", false);
}

VideoEndpoint::~VideoEndpoint()
{
	videoSource->removeVideoFrameObserver(this);
	shmVideoSource->remove();
}

void VideoEndpoint::onNewFrame(const VideoFrame* frame)
{
	// TODO Encode, then sends over RTP

	// Write in the shared memory segment

	// Make sure that the shared memory is still big enough to hold the new frame
	if (shmVideoSource->getSize() != frame->getSize()) {
		_debug("Truncating to %d", frame->getSize());

		shmVideoSource->truncate(frame->getSize());
	}

	_debug("Actual size : %d", shmVideoSource->getSize());
	_debug("Writing into shared memory ...");

	memcpy(shmVideoSource->getRegion(), frame->getFrame(), frame->getSize());
}


}
