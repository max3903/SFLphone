#include "VideoInputSource.h"
	
namespace sfl
{
	
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
		// TODO make this thread safe.
		memcpy (currentFrame, GST_BUFFER_DATA (currentFrame), GST_BUFFER_SIZE (currentFrame));
	}
	
	uint8_t * VideoInputSource::getCurrentFrame()
	{	
		// TODO make this thread safe.
		return currentFrame;
	}	
	
}