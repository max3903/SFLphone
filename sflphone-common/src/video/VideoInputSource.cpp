#include "VideoInputSource.h"
#include "VideoFrame.h"

#include <algorithm>
#include "logger.h"

#include <cc++/thread.h>
#include <cc++/exception.h>

namespace sfl {
VideoInputSource::VideoInputSource() : currentDevice(NULL), currentFrame(NULL), scaledWidth(0), scaledHeight(0), reformattedDepth(0){
	frameMutex = new ost::Mutex();
}

VideoInputSource::~VideoInputSource() {
	delete currentFrame;
	delete currentDevice;
}

void VideoInputSource::open() throw (VideoDeviceIOException, NoVideoDeviceAvailableException)
{
	// If no device has been specified, handle this automatically and set it to the first available one.
	if (currentDevice == NULL) {
		std::vector<VideoDevice> devices = enumerateDevices();
		if (devices.size() == 0) {
			throw NoVideoDeviceAvailableException("No video device can be found.");
		}

		open(devices.at(0));
	} else {
		open((*currentDevice));
	}
}

void VideoInputSource::setDevice(VideoDevice device)
{
	currentDevice = new VideoDevice(device);
}

void VideoInputSource::setDevice(const std::string& device) throw(UnknownVideoDeviceException)
{
	std::vector<sfl::VideoDevice> devices = enumerateDevices();
	std::vector<sfl::VideoDevice>::iterator it;

	_debug("Searching for device (%s) to set", device.c_str());

	// Search for that name
	for (it = devices.begin(); it < devices.end(); it++) {
		_debug("Device (%s)", (*it).getName().c_str());
		if (device.compare((*it).getName()) == 0) {
			setDevice((*it));
			_debug("Device set");
			return;
		}
	}

	throw UnknownVideoDeviceException("Device (" + device + ") could not be found");
}

void VideoInputSource::setCurrentFrame(const uint8_t* frame, size_t size) {
	frameMutex->enterMutex();
	{
		delete currentFrame;
		currentFrame = new VideoFrame(frame, size, getReformattedDepth(), getScaledHeight(), getScaledWidth());
	}
	frameMutex->leaveMutex();
}

VideoFrame * VideoInputSource::getCurrentFrame() {
	return currentFrame;
}

void VideoInputSource::addVideoFrameObserver(VideoFrameObserver* observer) {
	videoFrameObservers.push_back(observer);
}

void VideoInputSource::removeVideoFrameObserver(VideoFrameObserver* observer) {
	std::vector<VideoFrameObserver*>::iterator it;
	if ((it = std::find(videoFrameObservers.begin(), videoFrameObservers.end(),
			observer)) != videoFrameObservers.end()) {
		videoFrameObservers.erase(it);
	}
}

void VideoInputSource::notifyAllFrameObserver() {
	std::vector<VideoFrameObserver*>::iterator it;
	for (it = videoFrameObservers.begin(); it < videoFrameObservers.end(); it++) {
		(*it)->onNewFrame(currentFrame);
	}
}

}
