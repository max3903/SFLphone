#ifndef VIDEOINPUTSOURCEGST_H_
#define VIDEOINPUTSOURCEGST_H_

#include <vector>
#include <string>
#include <memory>
#include <stdexcept> 
#include <stdint.h>
#include <gst/gstelement.h>

#include "VideoInputSource.h"

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
 	 * This class captures video frames asynchronously via Gstreamer.
 	 */
	class VideoInputSourceGst : public VideoInputSource
	{
		public:
			VideoInputSourceGst();
			virtual ~VideoInputSourceGst();
			
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
			void close() throw(VideoDeviceIOException);
			/**
	 		 * @Override
	 		 */
			void run(void);
			/**
	 		 * @Override
	 		 */
	 		void grabFrame() throw(VideoDeviceIOException);
	 		 
			static std::string APPSINK_NAME;
				
		private:
			std::vector<VideoDevice*> getXimageSource() throw(MissingGstPluginException);		
			std::vector<VideoDevice*> getVideoTestSource() throw(MissingGstPluginException);
			std::vector<VideoDevice*> getV4l2Devices() throw(MissingGstPluginException);
			std::vector<VideoDevice*> getDv1394() throw(MissingGstPluginException);
			
			/**
			 * Make sure that all the plugins are available in gstreamer.
			 * @param plugins A vector of strings, containing the names of the plugins.
			 */
			void ensurePluginAvailability(std::vector<std::string>& plugins) throw(MissingGstPluginException);
			
    		GstElement * pipeline;
	};
}
#endif /*VIDEOINPUTSOURCEGST_H_*/
