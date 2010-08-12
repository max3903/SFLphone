#include "VideoEndpoint.h"
#include "source/VideoFrame.h"

#include "video/source/VideoInputSourceGst.h"

#include "util/FileDescriptorPasser.h"
#include "util/ipc/SharedMemoryPosix.h"

#include "dbus/dbusmanager.h"
#include "dbus/videomanager.h"

#include "logger.h"

#include <iostream>
#include <sstream>
#include <cc++/digest.h>
#include <uuid/uuid.h>

#include <string.h>
#include <errno.h>
#include <assert.h>

#define UUID_UNPARSED_SIZE 37

namespace sfl {

const std::string VideoEndpoint::EVENT_NAMESPACE = "org.sflphone.eventfd.";

VideoEndpoint::VideoEndpoint(VideoInputSource* src) :
	sourceTokens(), videoSource(src), capturing(false) {
	// Compute a simple CRC16 hash digest for this device. We want that to limit the
	// length of possible device/path names and offer consistent syntax.
	std::string hash = getDigest(src->getDevice()->getName());

	_debug ("Device %s gets hashed to %s ", src->getDevice()->getName().c_str(), hash.c_str());

	// Create a shared memory segment for video
	shmVideoSource = new SharedMemoryPosix("/sflphone-shm-" + hash, false);

	// Open a new event file descriptor
	eventFileDescriptor = eventfd(0, 0);

	if (eventFileDescriptor < 0) {
		// TODO Think of something more/better.
		_debug ("Failed to create an event file descriptor because (%s)", strerror (errno));
	}

	// Create a "server" on the UNIX domain for FD passing only.
	// The client UI receives the FD, and process events on it. This is the only way we can share FDs
	// between different processes. Note that the client only has to do it once for each event source, and does
	// *NOT* need permanent connection. It is only needed for passing the FD. At the time this was written, FD_TYPE
	// is available in DBUS 1.3, but this one is not yet stable.
	sourceEventFdPasser = new FileDescriptorPasser(EVENT_NAMESPACE + hash,
			eventFileDescriptor);
	sourceEventFdPasser->start();

	while (!sourceEventFdPasser->isReady()) {
		usleep(BUSY_WAIT_TIME);
	}

	// Register this object as a frame observer
	videoSource->addVideoFrameObserver(this);
}

VideoEndpoint::~VideoEndpoint() {
	videoSource->removeVideoFrameObserver(this);
	delete sourceEventFdPasser;
	close(eventFileDescriptor);
	shmVideoSource->remove();
}

std::string VideoEndpoint::generateToken() {

	uuid_t uuid;
	char uuidstr[UUID_UNPARSED_SIZE];

	uuid_generate(uuid);
	uuid_unparse(uuid, uuidstr);

	return std::string(uuidstr);
}

void VideoEndpoint::broadcastNewFrameEvent() {
	//_debug("Broadcasting NEW_FRAME_EVENT");
	eventfd_write(eventFileDescriptor, NEW_FRAME_EVENT);
}

std::string VideoEndpoint::getDigest(const std::string& name) {
	ost::CRC32Digest digest;

	digest.putDigest((const unsigned char*) name.c_str(), name.length());

	std::ostringstream buffer;
	buffer << digest;

	return buffer.str();
}

void VideoEndpoint::createRtpSession(sfl::InetSocketAddress address) {

	SocketAddressToVideoRtpSessionIterator socketIt = socketAddressToVideoRtpSessionMap.find(address);
	if (socketIt == socketAddressToVideoRtpSessionMap.end()) {
		// TODO throw UnavailableSomethingException
	}

	ost::InetHostAddress localAddress = address.getAddress();
	sfl::VideoRtpSessionSimple* rtpSession = new sfl::VideoRtpSessionSimple(localAddress, address.getPort());

	// Set input format
	rtpSession->setVideoInputFormat(videoSource->getOutputFormat());

	socketAddressToVideoRtpSessionMap.insert(SocketAddressToVideoRtpSessionEntry(address, rtpSession));
}


VideoInputSource* VideoEndpoint::getVideoInputSource() {
	return videoSource;
}

std::string VideoEndpoint::getShmName() {
	return shmVideoSource->getName();
}

std::string VideoEndpoint::requestTokenForSource() {
	std::string token = generateToken();
	sourceTokens.insert(token);

	return token;
}

std::string VideoEndpoint::capture() throw (VideoDeviceIOException) {
	videoSource->open();
	capturing = true;
	return requestTokenForSource();
}

void VideoEndpoint::stopCapture(std::string token)
		throw (VideoDeviceIOException, InvalidTokenException) {
	if (!sourceTokens.erase(token)) {
		throw InvalidTokenException("Token " + token + " is not valid.");
	}

	if (sourceTokens.size() == 0) {
		videoSource->close();
		capturing = false;
	}
}

std::string VideoEndpoint::getFdPasserName() {
	return sourceEventFdPasser->getAbstractNamespace();
}

bool VideoEndpoint::isCapturing() {
	return capturing;
}

bool VideoEndpoint::isDisposable() {
	return !capturing;
}

void VideoEndpoint::sendInAllRtpSession(const VideoFrame* frame)
{
	SocketAddressToVideoRtpSessionIterator it;
	for (it = socketAddressToVideoRtpSessionMap.begin(); it != socketAddressToVideoRtpSessionMap.end(); it++) {
		((*it).second)->sendPayloaded(frame);
	}
}

void VideoEndpoint::onNewFrame(const VideoFrame* frame) {
	if (!frame) {
		_error ("Null frame in VideoEndpoint"); // FIXME Should not happen.
		return;
	}

	// Send in all RTP sessions
	sendInAllRtpSession(frame);

	// Make sure that the shared memory is still big enough to hold the new frame
	// Should be required once.
	if (shmVideoSource->getSize() != frame->getSize()) {
		_debug ("Truncating to %d", frame->getSize());
		shmVideoSource->truncate(frame->getSize());
	}

	// Write into the shared memory segment
	memcpy(shmVideoSource->getRegion(), frame->getFrame(), frame->getSize());

	// Notify other processes
	broadcastNewFrameEvent();
}

void VideoEndpoint::startRtpSession(const InetSocketAddress& localAddress, std::vector<const sfl::VideoCodec*> negotiatedCodecs)
{
	// Find the RTP session identified by "localAddress"
	SocketAddressToVideoRtpSessionIterator socketIt = socketAddressToVideoRtpSessionMap.find(localAddress);
	if (socketIt == socketAddressToVideoRtpSessionMap.end()) {
		// TODO throw UnavailableSomethingException
	}
	VideoRtpSessionSimple* rtpSession = ((*socketIt).second);

	// Register the session codecs
	std::vector<const sfl::VideoCodec*>::iterator codecIt;
	for (codecIt = negotiatedCodecs.begin(); codecIt != negotiatedCodecs.end(); codecIt++) {
		// Note that there should be no risk of conflicting payload types as this is taken care in the SdpMedia object.
		rtpSession->addSessionCodec((*codecIt)->getPayloadType(), (*codecIt));
	}
	rtpSession->start();
}

void VideoEndpoint::addDestination(const InetSocketAddress& localAddress, const InetSocketAddress& destinationAddress) {
	_debug("Adding destination %s to RTP session bound on %s", destinationAddress.toString().c_str(), localAddress.toString().c_str());

	SocketAddressToVideoRtpSessionIterator socketIt = socketAddressToVideoRtpSessionMap.find(localAddress);

	if (socketIt == socketAddressToVideoRtpSessionMap.end()) {
		// TODO throw UnavailableSomethingException
		_error("Could not find RTP session for local address %s (%s:%d)", localAddress.toString().c_str(), __FILE__, __LINE__);
	}

	ost::InetHostAddress address = destinationAddress.getAddress();
	((*socketIt).second)->addDestination(address, destinationAddress.getPort());
}

}
