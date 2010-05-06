#include "VideoInputSourceGst.h"

#include <vector>
#include <string>

#include <gst/interfaces/propertyprobe.h>
#include <gst/app/gstappsink.h>

#include "../logger.h"

namespace sfl 
{
	
	VideoInputSourceGst::VideoInputSourceGst() :
		pipeline(NULL)
	{
	}

	VideoInputSourceGst::~VideoInputSourceGst()
	{
	}
	
	/**
	 * @Override
	 */
	void VideoInputSourceGst::open(int width, int height, int fps) throw(VideoDeviceIOException)
	{
		// Build the GST graph based on the chosen device.
		gchar* command = NULL;
		command = g_strdup_printf(
			"%s ! appsink max_buffers=2 drop=true caps=video/x-raw-yuv"
		    ",format=(fourcc)I420"
		    ",width=%d"
		    ",height=%d"
		    ",framerate=(fraction)%d/1 name=%s",
		    getDevice()->getDescription().c_str(),
			width, height, fps, APPSINK_NAME.c_str());
		
		GError* error = NULL;   
		pipeline = gst_parse_launch (command, &error);
		
		if (error != NULL) {
			g_error_free(error);
			g_free(command);
			throw new VideoDeviceIOException("while opening device " + getDevice()->getName()); 	
		}

		// Start the pipeline
		(void)gst_element_set_state (pipeline, GST_STATE_PLAYING);
		
		// Make sure that it is indeed playing
		GstState actualState;
		(void)gst_element_get_state (pipeline, &actualState, NULL, GST_SECOND);

    	if (actualState != GST_STATE_PLAYING) {
      		gst_element_set_state (pipeline, GST_STATE_NULL);
      		gst_object_unref (GST_OBJECT (pipeline));
      		pipeline = NULL;
      		
      		g_free (command);
			throw new VideoDeviceIOException("while opening device " + getDevice()->getName()); 	
    	} else {
			// notify observers for the opening event
    	}

 		g_free (command);
	}
	
	/**
	 * @Override
	 */
	void VideoInputSourceGst::close() throw(VideoDeviceIOException)
	{
		if (pipeline != NULL) {
			// Notify observers that the device is now closed.
    		g_object_unref (pipeline);
	    	pipeline = NULL;
  		}
	}
			
	/**
	 * @Override
	 */
	std::vector<VideoDevice*> VideoInputSourceGst::enumerateDevices(void)
	{
		std::vector<VideoDevice*> detectedDevices;

		try {
			std::vector<VideoDevice*> v4l2Devices = getV4l2Devices();
			std::vector<VideoDevice*> dv1394Devices = getDv1394();
			std::vector<VideoDevice*> ximageDevices = getXimageSource();		
			std::vector<VideoDevice*> videoTestSourceDevices = getVideoTestSource();
		
			detectedDevices.insert(detectedDevices.end(), v4l2Devices.begin(), v4l2Devices.end());
			detectedDevices.insert(detectedDevices.end(), dv1394Devices.begin(), dv1394Devices.end());
			detectedDevices.insert(detectedDevices.end(), ximageDevices.begin(), ximageDevices.end());
			detectedDevices.insert(detectedDevices.end(), videoTestSourceDevices.begin(), videoTestSourceDevices.end());
		} catch (MissingGstPluginException e) {
			// TODO We might want to pop something up to the user in the GUI.
			_warn((std::string("A plugin is missing : ") + e.what()).c_str());
		}
		
		return detectedDevices;
	}
	
	/**
	 * @Override
	 */
	void VideoInputSourceGst::run()
	{
		while(testCancel() == false) {
			grabFrame();
			
			notifyAllFrameObserver(getCurrentFrame());
			
			yield();
		}
	}
		
	/**
	 * @Override
	 */
	void VideoInputSourceGst::grabFrame() throw(VideoDeviceIOException)
	{		
		// Retreive the bin
		GstElement* sink = NULL;	
		sink = gst_bin_get_by_name (GST_BIN (pipeline), APPSINK_NAME.c_str());
		if (sink == NULL) {
			throw new VideoDeviceIOException("While grabing frame on " + getDevice()->getName()); 	
		}
	
		// Get the raw data
		GstBuffer* buffer = NULL;		
		buffer = gst_app_sink_pull_buffer(GST_APP_SINK (sink));
		if (buffer == NULL) {
			throw new VideoDeviceIOException("While grabing frame on " + getDevice()->getName()); 	
		}
		g_object_unref (sink);
		
		setCurrentFrame(buffer);  		
  		gst_buffer_unref (buffer);
 	}
 		
	void VideoInputSourceGst::ensurePluginAvailability(std::vector<std::string>& plugins) throw(MissingGstPluginException)
	{
		std::vector<std::string>::iterator it;
		for (it = plugins.begin(); it < plugins.end(); it++) {
			GstElement* element = NULL;
	
			element = gst_element_factory_make((*it).c_str(), ((*it) + std::string("presencetest")).c_str());
			if (element == NULL) {
				throw new MissingGstPluginException((*it) + std::string(" gstreamer pluging is missing."));
			} else {
	    		gst_object_unref(element);
			}
		}
	}
	
	std::vector<VideoDevice*> VideoInputSourceGst::getVideoTestSource() throw(MissingGstPluginException)
	{
		GstElement* element = NULL;
		element = gst_element_factory_make ("videotestsrc", "videotestsrcpresencetest");
	
		std::vector<VideoDevice*> detectedDevices;
		if (element != NULL) {
			detectedDevices.push_back(new VideoDevice(TEST, "videotestsrc", "videotestsrc"));
	    	gst_object_unref (GST_OBJECT (element));
	  	}
	  	
	  	return detectedDevices;
	}
	
	std::vector<VideoDevice*> VideoInputSourceGst::getXimageSource() throw(MissingGstPluginException)
	{
		std::vector<std::string> neededPlugins;
		neededPlugins.push_back("ximagesrc");
		neededPlugins.push_back("videoscale");
		neededPlugins.push_back("ffmpegcolorspace");
		ensurePluginAvailability(neededPlugins);

    	std::string description = "ximagesrc ! videoscale ! ffmpegcolorspace";
  		std::vector<VideoDevice*> detectedDevices;
  		detectedDevices.push_back(new VideoDevice(XIMAGE, "ximagesrc", description));
  		
  		return detectedDevices;
	}
	
	std::vector<VideoDevice*> VideoInputSourceGst::getV4l2Devices() throw(MissingGstPluginException)
	{
		std::vector<std::string> neededPlugins;
		neededPlugins.push_back("videoscale");
		neededPlugins.push_back("ffmpegcolorspace");
		
		// Will throw at that point if a plugin is missing. We might want to handle this more gracefully.
		ensurePluginAvailability(neededPlugins);
		
		// Retreive the list of devices and their details.
		std::vector<VideoDevice*> detectedDevices;
		GstElement* element = NULL;
		element = gst_element_factory_make ("v4l2src", "v4l2srcpresencetest");	
	  	if (element == NULL) {
			throw new MissingGstPluginException("Missing v4l2src plugin.");	  		
	  	} else {
	    	GstPropertyProbe* probe = NULL;
	    	const GParamSpec* pspec = NULL;
	    	GValueArray* array = NULL;
	
	    	probe = GST_PROPERTY_PROBE (element);
	    	pspec = gst_property_probe_get_property(probe, "device");
			array = gst_property_probe_probe_and_get_values(probe, pspec);
				
			if (array != NULL) {
			
	  			for (guint index = 0; index < array->n_values; index++) {
					GValue* device = NULL;
					gchar* name = NULL;
					gchar* descr = NULL;
	
					device = g_value_array_get_nth (array, index);
					g_object_set_property (G_OBJECT (element), "device", device);
	
					g_object_get (G_OBJECT (element), "device-name", &name, NULL);
					
					descr = g_strdup_printf ("v4l2src device=%s ! videoscale ! ffmpegcolorspace", g_value_get_string (device));
					
					// Add to vector
					VideoDevice * gstDevice = new VideoDevice(V4L2, name, descr);
					detectedDevices.push_back(gstDevice);
					
		  			g_free (name);
					g_free (descr);
	      		}
			}
	
	      g_value_array_free (array);
	      gst_element_set_state (element, GST_STATE_NULL);
	  	}
	  
		gst_object_unref (GST_OBJECT (element));
		
		return detectedDevices;
	}
	
	std::vector<VideoDevice*> VideoInputSourceGst::getDv1394() throw(MissingGstPluginException)
	{
	
		std::vector<std::string> neededPlugins;
		neededPlugins.push_back("videoscale");
		neededPlugins.push_back("decodebin");
		neededPlugins.push_back("ffmpegcolorspace");
		
		// Will throw at that point if a plugin is missing. We might want to handle this more gracefully.
		ensurePluginAvailability(neededPlugins);

		// Retreive the list of devices and their details.
		std::vector<VideoDevice*> detectedDevices;
		GstElement* element = NULL;			
		element = gst_element_factory_make ("dv1394src", "dv1394srcpresencetest");
	
	  	if (element == NULL) {
			throw new MissingGstPluginException("Missing dv1394src plugin.");	  		
	  	} else {
    		GstPropertyProbe* probe = NULL;	  		
			const GParamSpec* pspec = NULL;
		    GValueArray* array = NULL;
		
		    probe = GST_PROPERTY_PROBE (element);
		    pspec = gst_property_probe_get_property (probe, "guid");
		    array = gst_property_probe_probe_and_get_values (probe, pspec);
		
		    if (array != NULL) {
				for(guint index = 0; index < array->n_values; index++) {		
					GValue* guid = NULL;
					gchar* name = NULL;
					gchar* descr = NULL;
					guid = g_value_array_get_nth (array, index);
					g_object_set_property (G_OBJECT (element), "guid", guid);
		
					g_object_get (G_OBJECT (element), "device-name", &name, NULL);
					descr = g_strdup_printf ("dv1394src guid=%Ld"
						 " ! decodebin"
						 " ! videoscale"
						 " ! ffmpegcolorspace",
						 (long long int) g_value_get_uint64 (guid));
						 
					if (name != 0) {
						VideoDevice * gstDevice = new VideoDevice(DV1394, name, descr);
						detectedDevices.push_back(gstDevice);
						g_free (name);
					}
			
					g_free (descr);
		    	}
		
		      g_value_array_free (array);
		      gst_element_set_state (element, GST_STATE_NULL);
			}
		}
		 
		if (element != NULL) {
			gst_object_unref (GST_OBJECT (element));
		}
		
		return detectedDevices;
	}

}