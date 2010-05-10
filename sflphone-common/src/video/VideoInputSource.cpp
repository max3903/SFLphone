#include "VideoInputSource.h"
#include <algorithm>

namespace sfl {
VideoInputSource::VideoInputSource() :
	frameMutex(), currentFrame(NULL), currentDevice(NULL), width(0), height(0) {
}

VideoInputSource::~VideoInputSource() {
	delete currentFrame;
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

void VideoInputSource::setCurrentFrame(const uint8_t* frame, size_t size) {
	frameMutex.enterMutex();
	delete currentFrame;
	currentFrame = new VideoFrame(frame, size, getHeight(), getWidth());
	frameMutex.leaveMutex();
}

VideoFrame * VideoInputSource::getCurrentFrame() {
	return currentFrame;
}

}
