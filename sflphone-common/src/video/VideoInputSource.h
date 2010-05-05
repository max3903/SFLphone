#ifndef ABSTRACTVIDEOINPUTSOURCE_H_
#define ABSTRACTVIDEOINPUTSOURCE_H_

#include <vector> 
#include <string> 

namespace sfl 
{
	
/**
* Video source types that might be supported.
*/	
enum VideoSourceType { V4L, V4L2, DV1394, XIMAGE, IMAGE, TEST, NONE };

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
		virtual std::vector<VideoDevice> enumerateDevices(void) = 0;
		
		/**
		 * @param device The device to use.
		 */
		inline void setDevice(const std::string& device) { currentDevice = device; }
		
		/**
		 * @return the current device that is being used. 
		 */
		inline std::string getDevice() { return currentDevice; }
					 			
	private:
		std::string currentDevice; 
};

}
#endif /*ABSTRACTVIDEOINPUTSOURCE_H_*/
