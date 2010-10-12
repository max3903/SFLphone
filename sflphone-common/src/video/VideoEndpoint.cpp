#include "VideoEndpoint.h"
#include "source/VideoFrame.h"

#include "video/source/VideoInputSourceGst.h"

#include "util/FileDescriptorPasser.h"
#include "util/ipc/SharedMemoryPosix.h"

#include "dbus/dbusmanager.h"
#include "dbus/videomanager.h"

#include "logger.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <cc++/digest.h>
#include <uuid/uuid.h>

#include <string.h>
#include <errno.h>
#include <assert.h>

#define UUID_UNPARSED_SIZE 37

namespace sfl {

const std::string VideoEndpoint::EVENT_SOURCE_NAMESPACE =
		"org.sflphone.eventfd.device.";
const std::string VideoEndpoint::EVENT_STREAM_NAMESPACE =
		"org.sflphone.eventfd.rtp.";

VideoEndpoint::VideoEndpoint(VideoInputSource* src) :
	sourceTokens(), videoSource(src), capturing(false) {
	// Compute a simple CRC16 hash digest for this device. We want that to limit the
	// length of possible device/path names and offer consistent syntax.
	std::string hash = getDigest(src->getDevice()->getName());
	_debug ("Device %s gets hashed to %s ", src->getDevice()->getName().c_str(), hash.c_str());

	// Create a shared memory segment for video
	shmVideoSource = new SharedMemoryPosix("/sflphone-shm-" + hash, false);

	// Open a new event file descriptor
	eventFileDescriptorSource = eventfd(0, 0);

	if (eventFileDescriptorSource < 0) {
		// TODO Think of something more/better.
		_debug ("Failed to create an event file descriptor because (%s)", strerror (errno));
	}

	// Create a "server" on the UNIX domain for FD passing only.
	// The client UI receives the FD, and process events on it. This is the only way we can share FDs
	// between different processes. Note that the client only has to do it once for each event source, and does
	// *NOT* need permanent connection. It is only needed for passing the FD. At the time this was written, FD_TYPE
	// is available in DBUS 1.3, but this one is not yet stable.
	sourceEventFdPasser = new FileDescriptorPasser(EVENT_SOURCE_NAMESPACE
			+ hash, eventFileDescriptorSource);
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
	close(eventFileDescriptorSource);

	shmVideoSource->remove();
	delete shmVideoSource;
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
	eventfd_write(eventFileDescriptorSource, NEW_FRAME_EVENT);
}

std::string VideoEndpoint::getDigest(const std::string& name) {
	ost::CRC32Digest digest;

	digest.putDigest((const unsigned char*) name.c_str(), name.length());

	std::ostringstream buffer;
	buffer << digest;

	return buffer.str();
}

VideoInputSource* VideoEndpoint::getVideoInputSource() {
	return videoSource;
}

std::string VideoEndpoint::getSourceDeviceShmName() {
	return shmVideoSource->getName();
}

bool VideoEndpoint::hasShm(const std::string& shm)
{
	if (shmVideoSource->getName() == shm) {
		return true;
	}

	// Search in all RtpSessionRecord, trying to find the shm
	SocketAddressToRtpSessionRecordIterator it = std::find_if(
			socketAddressToRtpSessionRecord.begin(),
			socketAddressToRtpSessionRecord.end(), HasSameShmName(shm));
	if (it != socketAddressToRtpSessionRecord.end()) {
		return true;
	}

	return false;
}

sfl::VideoFormat VideoEndpoint::getShmVideoFormat(const std::string& shm)
		throw (NoSuchShmException) {
	if (shmVideoSource->getName() == shm) {
		return videoSource->getOutputFormat();
	}

	// Search in all RtpSessionRecord, trying to find the shm
	SocketAddressToRtpSessionRecordIterator it = std::find_if(
			socketAddressToRtpSessionRecord.begin(),
			socketAddressToRtpSessionRecord.end(), HasSameShmName(shm));
	if (it == socketAddressToRtpSessionRecord.end()) {
		throw NoSuchShmException(std::string(
				"Cannot find shared memory segment \"") + shm + std::string(
				"\" within any session."));
	}

	return ((*it).second)->getVideoRtpSession()->getVideoOutputFormat();
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

std::string VideoEndpoint::getFdPasserName() throw(NoSuchShmException) {
	return sourceEventFdPasser->getAbstractNamespace();
}

std::string VideoEndpoint::getFdPasserName(const std::string& shm) throw(NoSuchShmException)
{
	if (shmVideoSource->getName() == shm) {
		return sourceEventFdPasser->getAbstractNamespace();
	}

	// Search in all RtpSessionRecord, trying to find the shm
	SocketAddressToRtpSessionRecordIterator it = std::find_if(
			socketAddressToRtpSessionRecord.begin(),
			socketAddressToRtpSessionRecord.end(), HasSameShmName(shm));
	if (it == socketAddressToRtpSessionRecord.end()) {
		throw NoSuchShmException(std::string(
				"Cannot find shared memory segment \"") + shm + std::string(
				"\" within any session."));
	}


	return ((*it).second)->getFileDescriptorPasser()->getAbstractNamespace();
}

bool VideoEndpoint::isCapturing() {
	return capturing;
}

bool VideoEndpoint::isDisposable() {
	// Disposable if not capturing nor has active RTP sessions.
	return (!capturing && socketAddressToRtpSessionRecord.size() == 0);
}

void VideoEndpoint::sendInAllRtpSession(const VideoFrame* frame) {
	SocketAddressToRtpSessionRecordIterator it;
	for (it = socketAddressToRtpSessionRecord.begin(); it
			!= socketAddressToRtpSessionRecord.end(); it++) {
		((*it).second)->getVideoRtpSession()->sendPayloaded(frame);
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

void VideoEndpoint::removeRtpSession(const sfl::InetSocketAddress& address) {
	SocketAddressToRtpSessionRecordIterator socketIt =
			socketAddressToRtpSessionRecord.find(address);
	if (socketIt == socketAddressToRtpSessionRecord.end()) {
		return;
	}

	// Remove from the map
	delete (*socketIt).second;
	socketAddressToRtpSessionRecord.erase(socketIt);
}

void VideoEndpoint::createRtpSession(const sfl::InetSocketAddress& address) {
	SocketAddressToRtpSessionRecordIterator socketIt =
			socketAddressToRtpSessionRecord.find(address);
	if (socketIt != socketAddressToRtpSessionRecord.end()) {
		// TODO throw UnavailableSomethingException
		_warn("Some RTP session bound to socket on %s was found.", address.toString().c_str());
		return;
	}

	_debug("No RTP session bound to socket on %s could be found, creating new one ...", address.toString().c_str());

	ost::InetHostAddress localAddress = address.getAddress();
	sfl::VideoRtpSession* rtpSession = new sfl::VideoRtpSession(
			localAddress, address.getPort());

	// Set input format
	rtpSession->setVideoInputFormat(videoSource->getOutputFormat());

	// Create a shared memory segment for video
	std::stringstream ss;
	ss << rtpSession->getLocalSSRC();
	std::string suffix = ss.str();

	SharedMemoryPosix* shmRtpStream = new SharedMemoryPosix("/sflphone-shm-"
			+ suffix, false);

	// Open a new event file descriptor
	int fd = eventfd(0, 0);
	if (fd < 0) {
		// TODO Think of something more/better.
		_debug ("Failed to create an event file descriptor because (%s) (%s:%d)", strerror (errno), __FILE__, __LINE__);
	}

	RtpStreamDecoderObserver* observer = new RtpStreamDecoderObserver(fd,
			shmRtpStream, this);

	// Instantiate a decoder observer that will write into the SHM
	rtpSession->addObserver(observer);

	// Create a "server" on the UNIX domain for FD passing only.
	// The client UI receives the FD, and process events on it. This is the only way we can share FDs
	// between different processes. Note that the client only has to do it once for each event source, and does
	// *NOT* need permanent connection. It is only needed for passing the FD. At the time this was written, FD_TYPE
	// is available in DBUS 1.3, but this one is not yet stable.
	FileDescriptorPasser* passer = new FileDescriptorPasser(
			EVENT_STREAM_NAMESPACE + suffix, fd);
	passer->start();
	while (!passer->isReady()) {
		usleep(BUSY_WAIT_TIME);
	}

	socketAddressToRtpSessionRecord.insert(
			SocketAddressToRtpSessionRecordEntry(address, new RtpSessionRecord(this,
					shmRtpStream, passer, rtpSession, observer)));
}

std::string VideoEndpoint::startRtpSession(const InetSocketAddress& localAddress,
		std::vector<const sfl::VideoCodec*> negotiatedCodecs) {
	// Find the RTP session identified by "localAddress"
	SocketAddressToRtpSessionRecordIterator socketIt =
			socketAddressToRtpSessionRecord.find(localAddress);
	if (socketIt == socketAddressToRtpSessionRecord.end()) {
		// TODO throw UnavailableSomethingException
	}
	VideoRtpSession* rtpSession =
			((*socketIt).second)->getVideoRtpSession();

	// Register the session codecs
	std::vector<const sfl::VideoCodec*>::iterator codecIt;
	for (codecIt = negotiatedCodecs.begin(); codecIt != negotiatedCodecs.end(); codecIt++) {
		// Note that there should be no risk of conflicting payload types as this is taken care in the SdpMedia object.
		rtpSession->addSessionCodec((*codecIt)->getPayloadType(), (*codecIt));
	}

	// Start sending/receiving RTP packets
	rtpSession->start();

	return ((*socketIt).second)->getSharedMemoryPosix()->getName();
}

void VideoEndpoint::addDestination(const InetSocketAddress& localAddress,
		const InetSocketAddress& destinationAddress) {
	_info("Adding destination %s to RTP session bound on %s", destinationAddress.toString().c_str(), localAddress.toString().c_str());

	SocketAddressToRtpSessionRecordIterator socketIt =
			socketAddressToRtpSessionRecord.find(localAddress);
	if (socketIt == socketAddressToRtpSessionRecord.end()) {
		// TODO throw UnavailableSomethingException
		_error("Could not find RTP session for local address %s (%s:%d)", localAddress.toString().c_str(), __FILE__, __LINE__);
	}

	ost::InetHostAddress address = destinationAddress.getAddress();
	VideoRtpSession* rtpSession =
			((*socketIt).second)->getVideoRtpSession();
	rtpSession->addDestination(address, destinationAddress.getPort());
}

}
