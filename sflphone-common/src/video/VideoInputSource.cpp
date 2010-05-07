#include "VideoInputSource.h"
	
namespace sfl
{
	VideoInputSource::VideoInputSource() : 
		frameMutex(),
		currentFrame(NULL),
		currentFrameSize(0),
		currentDevice(NULL)
	{
	}
	
	VideoInputSource::~VideoInputSource()
	{
	}
	
	void VideoInputSource::addVideoFrameObserver(VideoFrameObserver* observer)
	{
		videoFrameObservers.push_back(observer);
	}
	
	void VideoInputSource::notifyAllFrameObserver(const uint8_t* frame)
	{
  		std::vector<VideoFrameObserver*>::iterator it;
  		for (it = videoFrameObservers.begin(); it < videoFrameObservers.end(); it++ ) {
  		 	(*it)->onNewFrame(frame);
  		}
	}
	
	void VideoInputSource::notifyAllFrameObserver()
	{
  		std::vector<VideoFrameObserver*>::iterator it;
  		for (it = videoFrameObservers.begin(); it < videoFrameObservers.end(); it++ ) {
  		 	(*it)->onNewFrame(currentFrame);
  		}
	}

	void VideoInputSource::setCurrentFrame(const uint8_t* frame, size_t size)
	{
		frameMutex.enterMutex();
			if (currentFrameSize != size) {
				free(currentFrame);
				currentFrame = (uint8_t *) malloc(size);
			}

			memcpy (currentFrame, frame, size);
		frameMutex.leaveMutex();
	}

	uint8_t * VideoInputSource::getCurrentFrame()
	{
		return currentFrame;
	}	

}
