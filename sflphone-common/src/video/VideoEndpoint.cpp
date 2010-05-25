#include "VideoEndpoint.h"

#include <iostream>
#include <sstream>
#include <cc++/digest.h>

#include <string.h>
#include <errno.h>
#include <assert.h>

#include "logger.h"
#include "VideoInputSourceGst.h"
#include "util/FileDescriptorPasser.h"
#include "util/SharedMemoryPosix.h"

namespace sfl {

const std::string VideoEndpoint::EVENT_NAMESPACE = "org.sflphone.eventfd.";

VideoEndpoint::VideoEndpoint(VideoInputSource* src) :
	videoSource(src) {
	// Compute a simple CRC16 hash digest for this device. We want that to limit the
	// length of possible device/path names and offer consistent syntax.
	std::string hash = getDigest(src->getDevice()->getName());

	_debug("Device %s gets hashed to %s ", src->getDevice()->getName().c_str(), hash.c_str());

	// Create a shared memory segment for video
	shmVideoSource = new SharedMemoryPosix("/sflphone-shm-" + hash, false);

	// Open a new event file descriptor
	eventFileDescriptor = eventfd(0, 0);
	if (eventFileDescriptor < 0) {
		// TODO Think of something more/better.
		_debug("Failed to create an event file descriptor because (%s)", strerror(errno));
	}

	// Create a "server" on the UNIX domain for FD passing only.
	// The client UI receives the FD, and process events on it. This is the only way we can share FDs
	// between different processes. Note that the client only has to do it once for each event source, and does
	// *NOT* need permanent connection. It is only needed for passing the FD. At the time this was written, FD_TYPE
	// is available in DBUS 1.3, but this one is not yet stable.

	sourceEventFdPasser = new FileDescriptorPasser(EVENT_NAMESPACE + hash,
			eventFileDescriptor);
	sourceEventFdPasser->detach();

	_debug("Thread is ready : %d", sourceEventFdPasser->isReady());
	sleep(2);
	_debug("After 2 sec : Thread is ready : %d", sourceEventFdPasser->isReady());


	// Register this object as a frame observer
	videoSource->addVideoFrameObserver(this);
}

VideoEndpoint::~VideoEndpoint() {
	videoSource->removeVideoFrameObserver(this);
	close(eventFileDescriptor);
	shmVideoSource->remove();
}

VideoInputSource* VideoEndpoint::getVideoInputSource() {
	return videoSource;
}

std::string VideoEndpoint::getShmName() {
	return shmVideoSource->getName();
}

std::string VideoEndpoint::getFdPasserName() {
	return sourceEventFdPasser->getAbstractNamespace();
}


void VideoEndpoint::onNewFrame(const VideoFrame* frame) {
	if (!frame) {
		_error("Null frame in VideoEndpoint"); // FIXME Should not happen.
		return;
	}

	// Make sure that the shared memory is still big enough to hold the new frame
	if (shmVideoSource->getSize() != frame->getSize()) {
		_debug("Truncating to %d", frame->getSize());
		shmVideoSource->truncate(frame->getSize());
	}

	// Write into the shared memory segment
	memcpy(shmVideoSource->getRegion(), frame->getFrame(), frame->getSize());

	// Notify other processes
	brodcastSourceEvent();

	// TODO Encode, then sends over RTP
}

void VideoEndpoint::brodcastSourceEvent() {
	_debug("Broadcasting NEW_FRAME_EVENT");
	eventfd_write(eventFileDescriptor, NEW_FRAME_EVENT);
}

std::string VideoEndpoint::getDigest(const std::string& name) {
	ost::CRC32Digest digest;

	digest.putDigest((const unsigned char*) name.c_str(), name.length());

	std::ostringstream buffer;
	buffer << digest;

	return buffer.str();
}

}
