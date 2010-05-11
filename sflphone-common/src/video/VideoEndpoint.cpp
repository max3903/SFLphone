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
	shmVideoSource = new SharedMemoryPosix("/sflphone-shm-" + src->getDevice()->toString(), false);
	semVideoSource = new sfl::SemaphorePosix("/sflphone-sem-" + src->getDevice()->toString(), false, 0666, 1);
}

VideoEndpoint::~VideoEndpoint()
{
	videoSource->removeVideoFrameObserver(this);
	shmVideoSource->remove();
	semVideoSource->remove();
}

void VideoEndpoint::onNewFrame(const VideoFrame* frame)
{
	// Make sure that the shared memory is still big enough to hold the new frame
	if (shmVideoSource->getSize() != frame->getSize()) {
		_debug("Truncating to %d", frame->getSize());

		shmVideoSource->truncate(frame->getSize());
	}

	// Write into the shared memory segment
	semVideoSource->wait();
		memcpy(shmVideoSource->getRegion(), frame->getFrame(), frame->getSize());
	semVideoSource->post();

	// TODO Encode, then sends over RTP
}


}
