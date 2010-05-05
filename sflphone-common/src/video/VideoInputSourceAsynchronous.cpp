#include "VideoInputSourceAsynchronous.h"

namespace sfl 
{	
	void VideoInputSourceAsynchronous::addVideoFrameObserver(VideoFrameObserver* observer)
	{
		videoFrameObservers.push_back(observer);
	}
	
	void VideoInputSourceAsynchronous::notifyAllFrameObserver(const uint8_t* frame)
	{
  		std::vector<VideoFrameObserver*>::iterator it;
  		for (it = videoFrameObservers.begin(); it < videoFrameObservers.end(); it++ ) {
  		 	(*it)->onNewFrame(frame);
  		}
	}
}
