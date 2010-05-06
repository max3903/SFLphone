/*
 *  Copyright (C) 2006-2010 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *                                                                              
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                                
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                              
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef ABSTRACTVIDEOINPUTSOURCE_H_
#define ABSTRACTVIDEOINPUTSOURCE_H_

#include <vector> 
#include <string> 
#include <memory>
#include <stdexcept>
#include <stdint.h>

#include <cc++/thread.h>
#include <cc++/exception.h>

#include <gst/gst.h>

namespace sfl 
{
	
/**
* Video source types that might be supported.
*/	
enum VideoSourceType { V4L, V4L2, DV1394, XIMAGE, IMAGE, TEST, NONE };

/**
 * This exception is thrown when an IO operation fails for a given video device.
 */
class VideoDeviceIOException : public ost::IOException {
	public:
		VideoDeviceIOException(const std::string& msg) : ost::IOException(msg) {}
};
 	
/**
 * Representation of a given gstreamer video device.
 */
class VideoDevice
{
	public:
		/**
		 * @param type One of the available source type (eg. V4L2)
		 * @param name The representative, and unique name of this device.
		 * @param description A description, additional info.
		 */
		VideoDevice(VideoSourceType type, std::string name, std::string description) {
			this->type = type;
			this->name = name;
			this->description = description;
		}
		
		/**
		 * @return The video source type (eg. V4L2)
		 */
		VideoSourceType getType() { return type; }
		/**
		 * @return The representative, and unique name of this device.
		 */
		std::string getName() { return name; }
		/**
		 * @return A description, additional info.
		 */			
		std::string getDescription() { return description; } 
		
	private:
		VideoSourceType type;
		std::string name;
		std::string description;
};
	
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
 * Base abstract class for asynchronous or synchronous access to a video capture device.
 */
class VideoInputSource : public ost::Thread
{
	public:			
		/**
		 * @return a vector containing the name of all the video devices available from this source.
		 */
		virtual std::vector<std::auto_ptr<VideoDevice> > enumerateDevices(void) = 0;
		
		/**
		 * Open the specified video device. Frame grabbing will be started on request, either via start(), 
		 * or getFrame() (in the synchronous case).
		 * @param widht The desired width.
		 * @param height The desired height.
		 * @param fps The desired frame rate.
		 */
		virtual void open(int width, int height, int fps) throw(VideoDeviceIOException) = 0;
		
		/**
		 * Close the currently opened device.
		 * @precondition The device should have been opened prior to that call.
		 */
		virtual void close() throw(VideoDeviceIOException) = 0;
		
		/**
		 * Grab a frame from the specified capture device.
		 * @precondition The device should have been opened prior to that call.
		 * @postcondition getCurrentFrame() will return the frame that was captured.
		 */
		virtual void grabFrame() throw(VideoDeviceIOException) = 0;
		
		/**
		 * Reminder : Must override this method such that frames are grabbed within this method.
		 * Once the thread is started, run() will get called.
		 */
		virtual void run(void) = 0;		
		
		/**
		 * @param device The device to use.
		 */
		inline void setDevice(VideoDevice* device) { currentDevice = device; }
		
		/**
		 * Destructor.
		 */
		virtual ~VideoInputSource();
		
		VideoInputSource();
		
		/**
		 * @return the current device that is being used. 
		 */
		inline VideoDevice* getDevice() { return currentDevice; }
		
		/**
		 * Register a new video frame observer.
		 * The observer will get called when a frame becomes available.
		 * @param observer The video frame observer.
		 */
		void addVideoFrameObserver(VideoFrameObserver* observer);
		
	   /**
		* @return The current frame.
		*/
		uint8_t* getCurrentFrame();		
				
	protected:
		/**
		 * Call every observers with the given frame as an argument.
		 */
		void notifyAllFrameObserver(const uint8_t* frame);
		
		/**
		 * @param the current frame. 
		 */
		void setCurrentFrame(GstBuffer* frame);	
						 			
	private:
		ost::Mutex frameMutex;	
		uint8_t * currentFrame;		
		VideoDevice* currentDevice;		
		std::vector<VideoFrameObserver*> videoFrameObservers;
};

}
#endif /*ABSTRACTVIDEOINPUTSOURCE_H_*/
