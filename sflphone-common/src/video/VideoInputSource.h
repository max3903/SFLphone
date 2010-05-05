#ifndef ABSTRACTVIDEOINPUTSOURCE_H_
#define ABSTRACTVIDEOINPUTSOURCE_H_

#include <vector> 
#include <string> 
#include <stdexcept>
#include <stdint.h>

namespace sfl 
{
	
/**
* Video source types that might be supported.
*/	
enum VideoSourceType { V4L, V4L2, DV1394, XIMAGE, IMAGE, TEST, NONE };

/**
 * This exception is thrown when an IO operation fails for a given video device.
 */
class VideoDeviceIOException : public std::runtime_error {
	public:
		VideoDeviceIOException(const std::string& msg) : std::runtime_error(msg) {}
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
 * Base abstract class for every asynchronous or synchronous video input source type.
 */
class VideoInputSource
{
	public:			
		/**
		 * @return a vector containing the name of all the video devices available from this source.
		 */
		virtual std::vector<VideoDevice*> enumerateDevices(void) = 0;
		
		/**
		 * Open the specified video device. Frame grabbing will be started on request, either via start(), 
		 * or getFrame() (in the synchronous case).
		 * @param widht The desired width.
		 * @param height The desired height.
		 * @param fps The desired frame rate.
		 */
		virtual void open(int width, int height, int fps) = 0;
		
		/**
		 * Close the currently opened device.
		 * @precondition The device should have been opened prior to that call.
		 */
		virtual void close() = 0;
		
		/**
		 * Grab a frame from the specified capture device.
		 * @precondition The device should have been opened prior to that call.
		 * @postcondition getCurrentFrame() will return the frame that was captured.
		 */
		virtual void grabFrame() = 0;
		
		/**
		 * @param device The device to use.
		 */
		inline void setDevice(VideoDevice* device) { currentDevice = device; }
		
		/**
		 * @return the current device that is being used. 
		 */
		inline VideoDevice* getDevice() { return currentDevice; }
					 			
	private:
		VideoDevice* currentDevice; 
		uint8_t * currentFrame;
};

}
#endif /*ABSTRACTVIDEOINPUTSOURCE_H_*/
