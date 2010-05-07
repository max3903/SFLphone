#ifndef VIDEOINPUTSOURCEGST_H_
#define VIDEOINPUTSOURCEGST_H_

#include <vector>
#include <string>
#include <memory>
#include <stdexcept> 
#include <stdint.h>

#include <gst/gstelement.h>
#include <gst/app/gstappsink.h>

#include "VideoInputSource.h"

namespace sfl {
/**
 * This exception is thrown when an expected plugin is missing from gstreamer.
 */
class MissingGstPluginException: public std::runtime_error {
public:
	MissingGstPluginException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * This class captures video frames asynchronously via Gstreamer.
 */
class VideoInputSourceGst: public VideoInputSource {
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
	void open(int width, int height, int fps) throw (VideoDeviceIOException);
	/**
	 * @Override
	 */
	void close() throw (VideoDeviceIOException);

	/**
	 * @Override
	 */
	void grabFrame() throw (VideoDeviceIOException);

	/**
	 * Check if we are capturing frames.
	 * @return true If the pipeline is running.
	 */
	bool isRunning() { return pipelineRunning; }

	static std::string APPSINK_NAME;

private:
	/**
	 * Make sure that all the plugins are available in gstreamer.
	 * @param plugins A vector of strings, containing the names of the plugins.
	 */
	void ensurePluginAvailability(std::vector<std::string>& plugins)
			throw (MissingGstPluginException);
	/**
	 * Dispatch a frame event to the registered observers.
	 * This will pull the frame from the queue, and push it
	 * to each observer.
	 */
	void dispatchEvent();

	/**
	 * Callback for appsink in gstreamer.
	 * This is handled internally. When a new frame is available,
	 * this method is called, then dispath the event to the
	 * registered observers.
	 * @param sink The appsink.
	 * @param data A pointer on the source object.
	 */
	static GstFlowReturn onNewBuffer(GstAppSink * sink, gpointer data);

	std::vector<VideoDevice*> getXimageSource()
			throw (MissingGstPluginException);
	std::vector<VideoDevice*> getVideoTestSource()
			throw (MissingGstPluginException);
	std::vector<VideoDevice*> getV4l2Devices()
			throw (MissingGstPluginException);
	std::vector<VideoDevice*> getDv1394() throw (MissingGstPluginException);

	GstElement * pipeline;
	bool pipelineRunning;
};
}
#endif /*VIDEOINPUTSOURCEGST_H_*/
