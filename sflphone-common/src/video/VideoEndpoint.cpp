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

	// Create a Reader-Writer lock in shared memory
	pthread_rwlockattr_t readerWriterAttributes;
	pthread_rwlockattr_init(&readerWriterAttributes);
	pthread_rwlockattr_setpshared(&readerWriterAttributes, PTHREAD_PROCESS_SHARED);

	pthread_rwlock_init(&readerWriterLock, &readerWriterAttributes);
	pthread_rwlockattr_destroy(&readerWriterAttributes);

	// Create a shared memory segment for video
	_debug((std::string("Creating ") + std::string("/sflphone-shm-") + src->getDevice()->toString()).c_str());
	shmVideoSource = new SharedMemoryPosix("/sflphone-shm-" + src->getDevice()->toString(), false);

	// Create a shared memory segment for the lock alone (easier than placing it in the same segment)
	shmRwLockVideoSource = new SharedMemoryPosix("/sflphone-rwlock-shm-" + src->getDevice()->toString(), false);
	shmRwLockVideoSource->truncate(sizeof(pthread_rwlock_t));
	memcpy(shmRwLockVideoSource->getRegion(), &readerWriterLock, sizeof(pthread_rwlock_t));
}

VideoEndpoint::~VideoEndpoint()
{
	videoSource->removeVideoFrameObserver(this);
	shmVideoSource->remove();
}

void VideoEndpoint::onNewFrame(const VideoFrame* frame)
{
	// Make sure that the shared memory is still big enough to hold the new frame
	if (shmVideoSource->getSize() != frame->getSize()) {
		_debug("Truncating to %d", frame->getSize());
		shmVideoSource->truncate(frame->getSize());
	}

	// Write into the shared memory segment
	pthread_rwlock_wrlock(&readerWriterLock);
		memcpy(shmVideoSource->getRegion(), frame->getFrame(), frame->getSize());
	pthread_rwlock_unlock(&readerWriterLock);

	// TODO Encode, then sends over RTP
}


}
