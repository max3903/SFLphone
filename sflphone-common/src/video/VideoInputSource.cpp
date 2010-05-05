#include "VideoInputSource.h"
	
namespace sfl
{
	VideoInputSource::VideoInputSource() : 
		frameMutex(),
		currentFrame(NULL),
		currentDevice(NULL)
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
	
	void VideoInputSource::setCurrentFrame(GstBuffer * currentFrame)
	{
		frameMutex.enterMutex();
			memcpy (currentFrame, GST_BUFFER_DATA (currentFrame), GST_BUFFER_SIZE (currentFrame));
		frameMutex.leaveMutex();
	}
	
	uint8_t * VideoInputSource::getCurrentFrame()
	{	
		return currentFrame;	
	}	
	
}