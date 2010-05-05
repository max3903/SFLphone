#ifndef GSTVIDEOINPUTSOURCEASYNCHRONOUS_H_
#define GSTVIDEOINPUTSOURCEASYNCHRONOUS_H_

#include <vector>
#include <string>
#include <stdexcept> 

#include <gst/gstelement.h>

#include "VideoInputSourceAsynchronous.h"

namespace sfl 
{
	/**
	 * This exception is thrown when an expected plugin is missing from gstreamer.
	 */
	class MissingGstPluginException : public std::runtime_error {
 		public:
   			MissingGstPluginException(const std::string& msg) : std::runtime_error(msg) {}
 	};
 
 	/**
 	 * Representation of a given gstreamer video device.
 	 */
 	class GstVideoDetectedDevice : public VideoDevice
	{
		public:
		   /**
			* @param type One of the available source type (eg. V4L2)
			* @param name The representative, and unique name of this device.
		 	* @param description A description, additional info.
		 	*/
			GstVideoDetectedDevice(VideoSourceType type, std::string name, std::string description) : VideoDevice(type, name, description) {} 			
	};
		
 	/**
 	 * This class captures video frames asynchronously via Gstreamer.
 	 */
	class GstVideoInputSourceAsynchronous : public VideoInputSourceAsynchronous
	{
		public:
			GstVideoInputSourceAsynchronous();
			virtual ~GstVideoInputSourceAsynchronous();
			
			/**
	 		 * @Override
	 		 */
			std::vector<VideoDevice*> enumerateDevices(void);		
			/**
	 		 * @Override
	 		 */
			void open(int width, int height, int fps) throw(VideoDeviceIOException);
			/**
	 		 * @Override
	 		 */
			void close();
			
			static std::string APPSINK_NAME;
		private:
			std::vector<GstVideoDetectedDevice*> getXimageSource() throw(MissingGstPluginException);		
			std::vector<GstVideoDetectedDevice*> getVideoTestSource() throw(MissingGstPluginException);
			std::vector<GstVideoDetectedDevice*> getV4l2Devices() throw(MissingGstPluginException);
			std::vector<GstVideoDetectedDevice*> getDv1394() throw(MissingGstPluginException);
			
			/**
			 * Make sure that all the plugins are available in gstreamer.
			 * @param plugins A vector of strings, containing the names of the plugins.
			 */
			void ensurePluginAvailability(std::vector<std::string>& plugins) throw(MissingGstPluginException);
			
    		GstElement * pipeline;
	};
	
	std::string GstVideoInputSourceAsynchronous::APPSINK_NAME = std::string("sflphone_sink");

}
#endif /*GSTVIDEOINPUTSOURCEASYNCHRONOUS_H_*/
