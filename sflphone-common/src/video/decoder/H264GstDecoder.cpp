/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#include "H264GstDecoder.h"
#include "logger.h"

#include <gst/video/video.h>
#include <gst/app/gstappsrc.h>

#include <stdint.h>
#include <string.h>

namespace sfl {

 gboolean H264GstDecoder::onGstreamerBusMessage(GstBus *bus, GstMessage *msg,
		gpointer data) {
	GMainLoop* loop = (GMainLoop*) data;

	switch (GST_MESSAGE_TYPE(msg)) {
	case GST_MESSAGE_EOS:
		 _error("End-of-stream");
		g_main_loop_quit(loop);
		break;
	case GST_MESSAGE_ERROR: {
		gchar *debug = NULL;
		GError *err = NULL;

		gst_message_parse_error(msg, &err, &debug);

		_error("Caught Gstreamer error: %s", err->message);

		g_error_free(err);

		if (debug) {
			_debug("Debug details : %s", debug);
			g_free(debug);
		}

		g_main_loop_quit(loop);
		break;
	}
	default:
		break;
	}

	return TRUE;
}

 gboolean H264GstDecoder::onReadDataFromSource(gpointer object) {
	H264GstDecoder* self = (H264GstDecoder*) object;

	// Dequeue some encoded NAL unit
	if (self->nalUnits.empty()) {
		_warn("Decoding queue is empty.");
		return TRUE;
	}

	Buffer<uint8_t> encodedFrame = self->nalUnits.front();
	self->nalUnits.pop();

	// Put in a GstBuffer
	GstBuffer* buffer = gst_buffer_new_and_alloc(encodedFrame.getSize());
	gst_buffer_set_data(buffer, encodedFrame.getBuffer(), encodedFrame.getSize());

	// Push the buffer down to the decoder
	GstFlowReturn ret;
	g_signal_emit_by_name(self->appsrc, "push-buffer", buffer, &ret);
	gst_buffer_unref(buffer);
	if (ret != GST_FLOW_OK) {
		// Some error, stop sending data
		_error("Some error occured while pushing data to the decoder.");
		return FALSE;
	}

	return TRUE;
}

 void H264GstDecoder::onStartFeed(GstElement * playbin, guint size,
		gpointer object) {
	H264GstDecoder* self = (H264GstDecoder*) object;

	if (self->sourceid == 0) {
		_debug("start feeding");
		self->sourceid = g_idle_add((GSourceFunc) onReadDataFromSource, object);
	}
}

 void H264GstDecoder::onStopFeed(GstElement * playbin, gpointer object) {
	H264GstDecoder* self = (H264GstDecoder*) object;

	if (self->sourceid != 0) {
		_debug("Stop feeding.");
		g_source_remove(self->sourceid);
		self->sourceid = 0;
	}
}

 void H264GstDecoder::onAppSrcInit(GObject * object, GObject * orig,
		GParamSpec * pspec, gpointer data) {
	H264GstDecoder* self = (H264GstDecoder*) data;

	// Get a handle to the appsrc
	g_object_get(orig, pspec->name, &self->appsrc, NULL);

	// configure the appsrc, we will push data into the appsrc from the mainloop.
	g_signal_connect(self->appsrc, "need-data", G_CALLBACK(onStartFeed), object);
	g_signal_connect(self->appsrc, "enough-data", G_CALLBACK(onStopFeed), object);
}

GstFlowReturn H264GstDecoder::onNewBuffer(GstAppSink* sink, gpointer data) {
	H264GstDecoder* self = (H264GstDecoder*) data;

	self->dispatchEvent();

	return GST_FLOW_OK;
}

void H264GstDecoder::dispatchEvent() {
	GstBuffer * buffer = NULL;
	buffer = gst_app_sink_pull_buffer(GST_APP_SINK(appsink));
	if (buffer == NULL) {
		_error("The buffer pulled from appsink is NULL");
		return; //TODO : do something
	}

	// Notify
	Buffer<uint8> obsBuffer(GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer));
	notifyAll(obsBuffer);

	gst_buffer_unref(buffer);
}

void H264GstDecoder::start() throw (VideoDecodingException) {
	GstStateChangeReturn ret = gst_element_set_state(pipeline,
			GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print("Failed to start up pipeline!\n");

		// Check if there is an error message with details on the bus
		std::string errorMessage = "An error occured while starting decoding. ";
		GstMessage *msg = gst_bus_poll(bus, GST_MESSAGE_ERROR,
				MAX_BUS_POOL_WAIT);
		if (msg) {
			GError *err = NULL;
			gst_message_parse_error(msg, &err, NULL);

			errorMessage = errorMessage + std::string(err->message);
			g_error_free(err);
			gst_message_unref(msg);
		}

		throw VideoDecodingException(errorMessage);
	}

	g_main_loop_run(loop);
}

void H264GstDecoder::stop() throw (VideoDecodingException) {
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}

void H264GstDecoder::decode(Buffer<uint8_t>& buffer)
		throw (VideoDecodingException) {
	nalUnits.push(buffer);
}

Dimension H264GstDecoder::getDimension() const {
	int x, y;
	if (GstPad* pad = gst_element_get_pad(appsink, "sfl_appsink")) {
		gst_video_get_size(GST_PAD(pad), &x, &y);
		gst_object_unref(GST_OBJECT(pad));
	} else {
		throw GstException("Failed to get dimension from  pad.");
	}

	return Dimension(x, y);
}

std::string H264GstDecoder::getFourcc() const {
	GstPad* pad;
	if (!(pad = gst_element_get_pad(appsink, "sfl_appsink"))) {
		throw GstException("Failed to get  pad.");
	}

	GstVideoFormat format;
	if (gst_video_format_parse_caps(GST_PAD_CAPS(pad), &format, NULL, NULL)
			!= TRUE) {
		throw GstException("Failed to get frame format from pad.");
	}

	guint32 fourcc = gst_video_format_to_fourcc(format);

	char code[4];
	sprintf(code, "%" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
	return std::string(code, 4);
}

void H264GstDecoder::setOutputFormat(const VideoFormat& outputFormat)
		throw (VideoDecodingException) {

	GstCaps* caps = gst_caps_new_simple(outputFormat.getMimetype().c_str(),
			"format", GST_TYPE_FOURCC, GST_STR_FOURCC(
					outputFormat.getFourcc().c_str()), "width", G_TYPE_INT,
			outputFormat.getWidth(), "height", G_TYPE_INT,
			outputFormat.getHeight(), NULL);

	gst_app_sink_set_caps(GST_APP_SINK(appsink), caps);
}

VideoFormat H264GstDecoder::getOutputFormat() const
{
	GstPad* pad;
	if (!(pad = gst_element_get_pad(appsink, "sfl_appsink"))) {
		throw GstException("Failed to get  pad.");
	}

	GstCaps* caps = gst_pad_get_caps(pad);
	const gchar* mime = gst_structure_get_name (gst_caps_get_structure (GST_PAD_CAPS(caps), 0));

	VideoFormat format;
	format.setFourcc(getFourcc());
	format.setMimetype(std::string(mime));

	Dimension dim = getDimension();
	format.setWidth(dim.getWidth());
	format.setHeight(dim.getHeight());

	return format;
}

void H264GstDecoder::init() {
	gst_init(0, NULL);

	// Create main loop
	loop = g_main_loop_new(NULL, FALSE);

	// Create pipeline
	pipeline = gst_pipeline_new("sfl_h264_decoding");

	// Watch for messages on the pipeline's bus
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, onGstreamerBusMessage, loop);
	gst_object_unref(bus);

	// Create the appsrc, in which we will inject depayloaded but still encoded frames.
	appsrc = gst_element_factory_make("appsrc", "sfl_appsrc");
	if (appsrc == NULL) {
		throw MissingGstPluginException(
				"Plugin \"appsrc\" could not be found. "
					"Check your install (you need gst-plugins-base). "
					"Run gst-inspect to get the list of available plugins");
	}

	// Create the H264 parser
	parser = gst_element_factory_make("h264parse", "sfl_parser");
	if (parser == NULL) {
		throw MissingGstPluginException(
				"Plugin \"h264parse\" could not be found. "
					"Check your install (you need gst-plugins-bad). "
					"Run gst-inspect to get the list of available plugins");
	}

	// Create the H264 decoder
	decoder = gst_element_factory_make("ffdec_h264", "sfl_decoder");
	if (decoder == NULL) {
		throw MissingGstPluginException(
				"Plugin \"ffdec_h264\" could not be found. "
					"Check your install (you need gstreamer-ffmpeg). "
					"Run gst-inspect to get the list of available plugins");
	}

	// Color space transform if needed.
	ffmpegcolorspace = gst_element_factory_make("ffmpegcolorspace",
			"sfl_ffmpegcolorspace");
	if (ffmpegcolorspace == NULL) {
		throw MissingGstPluginException(
				"Plugin \"ffmpegcolorspace\" could not be found. "
					"Check your install (you need gst-plugins-base). "
					"Run gst-inspect to get the list of available plugins");
	}

	deinterlace = gst_element_factory_make("deinterlace", "sfl_deinterlace");
	if (deinterlace == NULL) {
		throw MissingGstPluginException(
				"Plugin \"deinterlace\" could not be found. "
					"Check your install (you need gst-plugins-good). "
					"Run gst-inspect to get the list of available plugins");
	}

	// Video scale if neeeded
	videoscale = gst_element_factory_make("videoscale", "sfl_scale");
	if (videoscale == NULL) {
		throw MissingGstPluginException(
				"Plugin \"videoscale\" could not be found. "
					"Check your install (you need gst-plugins-base). "
					"Run gst-inspect to get the list of available plugins");
	}

	// Create the appsink where decoded frames will be retrieved.
	appsink = gst_element_factory_make("appsink", "sfl_appsink");
	if (appsink == NULL) {
		throw MissingGstPluginException(
				"Plugin \"appsink\" could not be found. "
					"Check your install (you need gst-plugins-base). "
					"Run gst-inspect to get the list of available plugins");
	}

	// Add everything to the pipeline.
	gst_bin_add_many(GST_BIN(pipeline), appsrc, parser, decoder,
			ffmpegcolorspace, deinterlace, videoscale, appsink, NULL);

	// Link all elements together
	if (gst_element_link_many(appsrc, parser, decoder, ffmpegcolorspace,
			deinterlace, videoscale, appsink, NULL) == FALSE) {
		throw VideoDecodingException("Failed to link one or more elements.");
	}

	// Configure callbacks for the appsink
	GstAppSinkCallbacks callbacks;
	callbacks.eos = NULL;
	callbacks.new_preroll = NULL;
	callbacks.new_buffer = H264GstDecoder::onNewBuffer;
	callbacks.new_buffer_list = NULL;
	gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, this, NULL);

	// Connect signal. Called when the appsrc is created.
	g_signal_connect(GST_BIN(pipeline), "deep-notify::source",
			(GCallback) onAppSrcInit, this);
}

H264GstDecoder::H264GstDecoder() throw (VideoDecodingException,
		MissingPluginException) :
	VideoDecoder() {
	init();
	start();
}

H264GstDecoder::H264GstDecoder(const VideoFormat& outputFormat)
		throw (VideoDecodingException, MissingPluginException) :
	VideoDecoder(outputFormat) {
	init();
	setOutputFormat(outputFormat);
	start();
}

H264GstDecoder::~H264GstDecoder(){
	stop();
}

}
