#include "VideoInputSourceGst.h"
#include "GstVideoDevice.h"
#include "logger.h"

#include <vector>
#include <string>

#include <gst/interfaces/propertyprobe.h>
#include <gst/app/gstappsink.h>

namespace sfl {

const char* VideoInputSourceGst::APPSINK_NAME = "sflphone_sink";
const char* VideoInputSourceGst::APPSINK_MIMETYPE = "video/x-raw-rgb";

VideoInputSourceGst::VideoInputSourceGst() :
	pipeline(NULL), pipelineRunning(false) {
	gst_init(0, NULL); // FIXME this is throwing a GLib-GObject-CRITICAL, but we want to keep it that way (null).
}

VideoInputSourceGst::~VideoInputSourceGst() {

}

GstFlowReturn VideoInputSourceGst::onNewBuffer(GstAppSink * sink, gpointer data) {
	VideoInputSourceGst * gst = (VideoInputSourceGst *) data;
	gst->dispatchEvent();
	return GST_FLOW_OK;
}

/**
 * @Override
 */
void VideoInputSourceGst::open(VideoDevice device)
		throw (VideoDeviceIOException) {

	GstVideoDevice* gstDevice = static_cast<GstVideoDevice*> (&device);

	_debug("Pipeline for device %s: %s",  gstDevice->getName().c_str(), gstDevice->getGstPipeline().c_str());

	// Build the GST graph based on the chosen device.
	gchar* command = NULL;
	command = g_strdup_printf("%s ! appsink max_buffers=2 drop=true caps=%s"
		",bpp=(int)%d"
		",depth=(int)%d"
		",width=%d"
		",height=%d"
		",framerate=(fraction)%d/%d name=%s",
			gstDevice->getGstPipeline().c_str(), APPSINK_MIMETYPE,
			APPSINK_BPP,
			APPSINK_DEPTH,
			gstDevice->getPreferredWidth(), // TODO offer the ability to videoscale the output
			gstDevice->getPreferredHeight(),
			gstDevice->getPreferredFrameRateNumerator(),
			gstDevice->getPreferredFrameRateDenominator(), APPSINK_NAME);

	_debug("Opening gstreamer with pipeline : %s", command);

	GError* error = NULL;
	pipeline = gst_parse_launch(command, &error);

	if (error != NULL) {
		g_error_free(error);
		g_free(command);
		pipelineRunning = false;
		throw VideoDeviceIOException("while opening device "
				+ gstDevice->getName());
	}

	// Set internal callbacks
	GstElement * element;
	element = gst_bin_get_by_name(GST_BIN(pipeline), APPSINK_NAME);

	GstAppSinkCallbacks callbacks;
	callbacks.eos = NULL;
	callbacks.new_preroll = NULL;
	callbacks.new_buffer = VideoInputSourceGst::onNewBuffer;
	callbacks.new_buffer_list = NULL;
	gst_app_sink_set_callbacks(GST_APP_SINK(element), &callbacks, this, NULL);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	// Make sure that we are in the correct state
	GstState state;
	gst_element_get_state(pipeline, &state, NULL, GST_SECOND);

	if (state != GST_STATE_PLAYING) {
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(GST_OBJECT(pipeline));
		g_free(command);

		pipelineRunning = false;
		throw VideoDeviceIOException(
				"Failed to start the video capture pipeline.");
	}

	g_free(command);
	pipelineRunning = true;

	// These are the width and height at the sink. They might be different from the source. For eg: 320x240 displayed in 1024x800
	setDevice(device);
	setScaledWidth(gstDevice->getPreferredWidth());
	setScaledHeight(gstDevice->getPreferredHeight());
	setReformattedDepth(APPSINK_DEPTH);
}

/**
 * @Override
 */
void VideoInputSourceGst::dispatchEvent() {
	// Pull the frame.
	GstElement * sink = NULL;
	sink = gst_bin_get_by_name(GST_BIN(pipeline), APPSINK_NAME);

	if (sink == NULL) {
		_error("Sink is NULL");
		return; //TODO : do something
	}

	GstBuffer * buffer = NULL;
	buffer = gst_app_sink_pull_buffer(GST_APP_SINK(sink));

	if (buffer == NULL) {
		_error("GST buffer is NULL");
		return; //TODO : do something
	}

	// Notify observers with the new frame
	setCurrentFrame(GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer));
	notifyAllFrameObserver();

	gst_buffer_unref(buffer);
	g_object_unref(sink);
}

void VideoInputSourceGst::grabFrame() throw (VideoDeviceIOException) {
	// Retreive the bin
	GstElement* sink = NULL;
	sink = gst_bin_get_by_name(GST_BIN(pipeline), APPSINK_NAME);
	if (sink == NULL) {
		throw VideoDeviceIOException("While grabbing frame on "
				+ currentDevice->getName());
	}

	// Get the raw data
	GstBuffer* buffer = NULL;
	buffer = gst_app_sink_pull_buffer(GST_APP_SINK(sink));
	if (buffer == NULL) {
		throw VideoDeviceIOException("While grabbing frame on "
				+ currentDevice->getName());
	}
	g_object_unref(sink);

	setCurrentFrame(GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer));
	gst_buffer_unref(buffer);
}

void VideoInputSourceGst::close() throw (VideoDeviceIOException) {
	if (!isRunning()) {
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(GST_OBJECT(pipeline));
		pipelineRunning = false;
	}
}

/**
 * @Override
 */
/**
 * @Override
 */
std::vector<FrameFormat> VideoInputSourceGst::getWebcamCapabilities(
		VideoSourceType type, const std::string& device) throw (GstException) {
	std::vector<FrameFormat> formats;
	char *pipeline_desc;
	GstElement * pipeline;
	GError *error;
	GstStateChangeReturn ret;
	GstMessage *msg = NULL;
	GstBus *bus;

	pipeline_desc = g_strdup_printf("%s name=source device=%s ! fakesink",
			videoTypeToString(type).c_str(), device.c_str());
	error = NULL;
	pipeline = gst_parse_launch(pipeline_desc, &error);
	if ((pipeline != NULL) && (error == NULL)) {
		/* Start the pipeline and wait for max. 10 seconds for it to start up */
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
		ret = gst_element_get_state(pipeline, NULL, NULL,
				DEVICE_DETECT_MAX_WAIT * GST_SECOND);

		// Check if any error messages were posted on the bus
		bus = gst_element_get_bus(pipeline);
		msg = gst_bus_poll(bus, GST_MESSAGE_ERROR, 0);
		gst_object_unref(bus);

		// Get the capabilities for this device
		if ((msg == NULL) && (ret == GST_STATE_CHANGE_SUCCESS)) {
			GstElement *src;
			GstPad *pad;
			char *name;
			GstCaps *caps;

			gst_element_set_state(pipeline, GST_STATE_PAUSED);

			src = gst_bin_get_by_name(GST_BIN(pipeline), "source");

			g_object_get(G_OBJECT(src), "device-name", &name, NULL);
			if (name == NULL) {
				name = "Unknown";
			}
			_debug("Device: %s (%s)", name, device.c_str());

			pad = gst_element_get_pad(src, "src");
			caps = gst_pad_get_caps(pad);
			gst_object_unref(pad);

			formats = getSupportedFormats(caps);

			gst_caps_unref(caps);
		}
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
	}

	if (error) {
		gchar *message_str;
		gst_message_parse_error(msg, &error, &message_str);
		std::string errorDescription = message_str;
		g_error_free(error);
		g_free(pipeline_desc);
		throw GstException(errorDescription);
	}

	g_free(pipeline_desc);

	return formats;
}

std::vector<FrameFormat> VideoInputSourceGst::getSupportedFormats(GstCaps *caps) {
	std::vector<FrameFormat> detectedFormats;
	int i;
	int num_structures;

	num_structures = gst_caps_get_size(caps);
	for (i = 0; i < num_structures; i++) {
		GstStructure *structure;
		const GValue *width, *height;
		structure = gst_caps_get_structure(caps, i);

		/* only interested in raw formats; we don't want to end up using image/jpeg
		 * (or whatever else the cam may produce) since we won't be able to link
		 * that to ffmpegcolorspace or the effect plugins, which makes it rather
		 * useless (although we could plug a decoder of course) */
		if (!gst_structure_has_name(structure, "video/x-raw-yuv")
				&& !gst_structure_has_name(structure, "video/x-raw-rgb")) {
			continue;
		}

		width = gst_structure_get_value(structure, "width");
		height = gst_structure_get_value(structure, "height");

		if (G_VALUE_HOLDS_INT(width)) {
			int width, height;
			gst_structure_get_int(structure, "width", &width);
			gst_structure_get_int(structure, "height", &height);

			std::vector<FrameRate> framerates = getSupportedFramerates(
					structure);
			FrameFormat* format = new FrameFormat(gst_structure_get_name(
					structure), width, height, framerates);
			detectedFormats.push_back(*(format));
		} else if (GST_VALUE_HOLDS_INT_RANGE(width)) {
			int min_width, max_width, min_height, max_height;
			int cur_width, cur_height;

			min_width = gst_value_get_int_range_min(width);
			max_width = gst_value_get_int_range_max(width);
			min_height = gst_value_get_int_range_min(height);
			max_height = gst_value_get_int_range_max(height);

			cur_width = min_width;
			cur_height = min_height;

			/* Gstreamer will sometimes give us a range with min_xxx == max_xxx,
			 we use <= here (and not below) to make this work */
			while (cur_width <= max_width && cur_height <= max_height) {
				std::vector<FrameRate> framerates = getSupportedFramerates(
						structure);
				FrameFormat* format = new FrameFormat(gst_structure_get_name(
						structure), cur_width, cur_height, framerates);
				detectedFormats.push_back(*(format));

				cur_width *= 2;
				cur_height *= 2;
			}

			cur_width = max_width;
			cur_height = max_height;
			while (cur_width > min_width && cur_height > min_height) {
				std::vector<FrameRate> framerates = getSupportedFramerates(
						structure);
				FrameFormat* format = new FrameFormat(gst_structure_get_name(
						structure), cur_width, cur_height, framerates);
				detectedFormats.push_back(*(format));

				cur_width /= 2;
				cur_height /= 2;
			}
		} else {
			_error("GValue type %s, cannot be handled for resolution width", G_VALUE_TYPE_NAME(width));
		}
	}

	return detectedFormats;
}

std::vector<FrameRate> VideoInputSourceGst::getSupportedFramerates(
		GstStructure *structure) {
	std::vector<FrameRate> supportedFrameRates;
	const GValue *framerates;
	int i, j;

	framerates = gst_structure_get_value(structure, "framerate");
	if (GST_VALUE_HOLDS_FRACTION(framerates)) {

		FrameRate rate(gst_value_get_fraction_numerator(framerates),
				gst_value_get_fraction_denominator(framerates));
		supportedFrameRates.push_back(rate);

	} else if (GST_VALUE_HOLDS_LIST(framerates)) {

		int numberFramerates = gst_value_list_get_size(framerates);
		for (i = 0; i < numberFramerates; i++) {
			const GValue *value;
			value = gst_value_list_get_value(framerates, i);
			FrameRate rate(gst_value_get_fraction_numerator(value),
					gst_value_get_fraction_denominator(value));
			supportedFrameRates.push_back(rate);
		}

	} else if (GST_VALUE_HOLDS_FRACTION_RANGE(framerates)) {

		int numerator_min, denominator_min, numerator_max, denominator_max;
		const GValue *fraction_range_min;
		const GValue *fraction_range_max;

		fraction_range_min = gst_value_get_fraction_range_min(framerates);
		numerator_min = gst_value_get_fraction_numerator(fraction_range_min);
		denominator_min
				= gst_value_get_fraction_denominator(fraction_range_min);

		fraction_range_max = gst_value_get_fraction_range_max(framerates);
		numerator_max = gst_value_get_fraction_numerator(fraction_range_max);
		denominator_max
				= gst_value_get_fraction_denominator(fraction_range_max);

		_debug("FractionRange: %d/%d - %d/%d\n", numerator_min,
				denominator_min, numerator_max, denominator_max);

		int k = 0;
		for (i = numerator_min; i <= numerator_max; i++) {
			for (j = denominator_min; j <= denominator_max; j++) {
				FrameRate rate(i, j);
				supportedFrameRates.push_back(rate);
				k++;
			}
		}

	} else {
		_error("GValue type %s, cannot be handled for framerates",
				G_VALUE_TYPE_NAME(framerates));
	}

	return supportedFrameRates;
}

void VideoInputSourceGst::ensurePluginAvailability(
		std::vector<std::string>& plugins) throw (MissingGstPluginException) {
	std::vector<std::string>::iterator it;
	for (it = plugins.begin(); it < plugins.end(); it++) {
		GstElement* element = NULL;

		element = gst_element_factory_make((*it).c_str(), ((*it) + std::string(
				"presencetest")).c_str());
		if (element == NULL) {
			throw MissingGstPluginException((*it) + std::string(
					" gstreamer pluging is missing."));
		} else {
			gst_object_unref(element);
		}
	}
}

std::vector<VideoDevice*> VideoInputSourceGst::getVideoTestSource()
		throw (MissingGstPluginException) {
	GstElement* element = NULL;
	element = gst_element_factory_make("videotestsrc",
			"videotestsrcpresencetest");

	std::vector<VideoDevice*> detectedDevices;
	if (element != NULL) {
		// FIXME
		//std::vector<FrameFormat> formats = getWebcamCapabilities(TEST,
		//detectedDevices.push_back(new VideoDevice(TEST, "videotestsrc",
		//		"videotestsrc"));
		gst_object_unref(GST_OBJECT(element));
	}

	return detectedDevices;
}

std::vector<VideoDevice*> VideoInputSourceGst::getXimageSource()
		throw (MissingGstPluginException) {
	std::vector<std::string> neededPlugins;
	neededPlugins.push_back("ximagesrc");
	neededPlugins.push_back("videoscale");
	neededPlugins.push_back("ffmpegcolorspace");
	ensurePluginAvailability(neededPlugins);

	std::string pipelineDescription =
			"ximagesrc ! videoscale ! ffmpegcolorspace";
	std::vector<VideoDevice*> detectedDevices;

	// FIXME
	// std::vector<FrameFormat> formats = getWebcamCapabilities(XIMAGE,
	//		g_value_get_string(device));
	// VideoDevice * gstDevice = new GstVideoDevice(XIMAGE, formats, name,
	//		pipelineDescription);
	// detectedDevices.push_back(gstDevice);

	return detectedDevices;
}

std::vector<VideoDevice> VideoInputSourceGst::getV4l2Devices()
		throw (MissingGstPluginException) {

	std::vector<std::string> neededPlugins;
	neededPlugins.push_back("v4l2src");
	ensurePluginAvailability(neededPlugins);

	// Retreive the list of devices and their details.
	std::vector<VideoDevice> detectedDevices;
	GstElement* element = NULL;
	element = gst_element_factory_make("v4l2src", "v4l2srcpresencetest");
	if (element == NULL) {
		_error("V4L2 plugin is missing");
		throw MissingGstPluginException("Missing v4l2src plugin.");
	} else {
		GstPropertyProbe* probe = NULL;
		const GParamSpec* pspec = NULL;
		GValueArray* array = NULL;

		probe = GST_PROPERTY_PROBE(element);
		pspec = gst_property_probe_get_property(probe, "device");
		array = gst_property_probe_probe_and_get_values(probe, pspec);

		if (array != NULL) {
			for (guint index = 0; index < array->n_values; index++) {
				GValue* device = NULL;
				gchar* name = NULL;

				device = g_value_array_get_nth(array, index);
				g_object_set_property(G_OBJECT(element), "device", device);
				g_object_get(G_OBJECT(element), "device-name", &name, NULL);

				// Add to vector
				std::vector<FrameFormat> formats = getWebcamCapabilities(V4L2,
						g_value_get_string(device));
				GstVideoDevice gstDevice(V4L2, formats, g_value_get_string(
						device), name);
				detectedDevices.push_back(gstDevice);

				g_free(name);
			}
		}

		g_value_array_free(array);
		gst_element_set_state(element, GST_STATE_NULL);
	}

	gst_object_unref(GST_OBJECT(element));

	return detectedDevices;
}

std::vector<VideoDevice> VideoInputSourceGst::enumerateDevices(void) {
	std::vector<VideoDevice> detectedDevices;

	try {
		std::vector<VideoDevice> v4l2Devices = getV4l2Devices();
		//std::vector<VideoDevice*> ximageDevices = getXimageSource();
		//std::vector<VideoDevice*> videoTestSourceDevices = getVideoTestSource();

		detectedDevices.insert(detectedDevices.end(), v4l2Devices.begin(),
				v4l2Devices.end());
		//detectedDevices.insert(detectedDevices.end(), ximageDevices.begin(),
		//		ximageDevices.end());
		//detectedDevices.insert(detectedDevices.end(),
		//		videoTestSourceDevices.begin(), videoTestSourceDevices.end());
	} catch (MissingGstPluginException e) {
		// TODO We might want to pop something up to the user in the GUI.
		_warn((std::string("A plugin is missing : ") + e.what()).c_str());
	}

	return detectedDevices;
}

}
