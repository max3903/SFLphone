#ifndef VIDEOINPUTSOURCEASYNCHRONOUS_H_
#define VIDEOINPUTSOURCEASYNCHRONOUS_H_

#include <vector>
#include <string>
#include <stdint.h>
#include <cc++/thread.h>

#include "VideoInputSource.h"

namespace sfl
{
	/**
	 * Interface for those objects which want
	 * to be notified uppon the arrival of new
	 * frames for a specified video input source.
	 */
	class VideoFrameObserver
	{
		public:
			/**
			 * @param frame The new frame in whichever format it was requested.
			 */
			virtual void onNewFrame(const uint8_t* frame) = 0;
	};
	
	/**
	 * Base abstract class for every asynchronous video input source type.
	 */
	class VideoInputSourceAsynchronous : VideoInputSource, ost::Thread
	{
		public:				
			/**
			 * Must override this method such that frames are grabbed within this method.
			 * Once the thread is started, run() will get called.
			 */
			virtual void run(void) = 0;
			
			/**
			 * Register a new video frame observer.
			 * The observer will get called when a frame becomes available.
			 * @param observer The video frame observer.
			 */
			 void addVideoFrameObserver(VideoFrameObserver* observer);
			 
			 VideoInputSourceAsynchronous();
			 virtual ~VideoInputSourceAsynchronous();
			 
		protected:
			/**
			 * Call every observers with the given frame as an argument.
			 */
			void notifyAllFrameObserver(const uint8_t* frame);
		
		private:
			std::vector<VideoFrameObserver*> videoFrameObservers;	
	};
}

#endif /*VIDEOINPUTSOURCEASYNCHRONOUS_H_*/
