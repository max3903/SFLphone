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
#include <gst/rtp/gstrtpbuffer.h>

#include <stdint.h>
#include <string.h>

namespace sfl {

void H264GstDecoder::dispatchEvent() {
	GstBuffer * buffer = NULL;
	buffer = gst_app_sink_pull_buffer(GST_APP_SINK(appsink));
	if (buffer == NULL) {
		_error("The buffer pulled from appsink is NULL");
		return; //TODO : do something
	}

	// Notify
	ManagedBuffer<uint8> obsBuffer(GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer));
	notifyAll(obsBuffer);

	gst_buffer_unref(buffer);
}

GstFlowReturn H264GstDecoder::onNewBuffer(GstAppSink* sink, gpointer data) {
	H264GstDecoder* self = (H264GstDecoder*) data;

	_debug("Got buffer at the output");
	self->dispatchEvent();

	return GST_FLOW_OK;
}

void H264GstDecoder::onEnoughData(GstAppSrc *src, gpointer user_data)
{
	_debug("Appsrc queue has enough data");
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

VideoFormat H264GstDecoder::getOutputFormat() const {
	GstPad* pad;
	if (!(pad = gst_element_get_pad(appsink, "sfl_appsink"))) {
		throw GstException("Failed to get  pad.");
	}

	GstCaps* caps = gst_pad_get_caps(pad);
	const gchar* mime = gst_structure_get_name(gst_caps_get_structure(
			GST_PAD_CAPS(caps), 0));

	VideoFormat format;
	format.setFourcc(getFourcc());
	format.setMimetype(std::string(mime));

	Dimension dim = getDimension();
	format.setWidth(dim.getWidth());
	format.setHeight(dim.getHeight());

	return format;
}

/**
 * Initialize the Gstreamer pipeline.
 *
 * The appsrc element merely does what udpsrc would do. The only
 * reason for not using it is because we want to keep the security
 * features that we already have with ccRTP.
 *
 * |appsrc| -> |rtph264depay| -> |h264parse| -> |ffdec_h264| ---
 *                                                                     -> |queue| -> |appsink|
 *                                                                   /
 * ----> |ffmpegcolorspace| -> |deinterlace| -> |videoscale| -> |tee|
 *                                                                   \
 *                                                                     -> |queue| -> |autovideosink| (debug branch)
 */
void H264GstDecoder::stop() throw (VideoDecodingException) {
	gst_app_src_end_of_stream(GST_APP_SRC(appsrc));
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}

void H264GstDecoder::start() throw (VideoDecodingException) {
	GstStateChangeReturn ret = gst_element_set_state(pipeline,
			GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print("Failed to start up pipeline!\n");

		// Check if there is an error message with details on the bus
		std::string errorMessage = "An error occured while starting decoding. ";
		GstBus* bus = gst_element_get_bus(pipeline);
		GstMessage *msg = gst_bus_poll(bus, GST_MESSAGE_ERROR,
				MAX_BUS_POOL_WAIT);
		gst_object_unref(bus);

		if (msg) {
			GError *err = NULL;
			gst_message_parse_error(msg, &err, NULL);

			errorMessage = errorMessage + std::string(err->message);
			g_error_free(err);
			gst_message_unref(msg);
		}

		throw VideoDecodingException(errorMessage);
	}

}

void H264GstDecoder::decode(ManagedBuffer<uint8>& data) throw (VideoDecodingException) {

	// Convert the raw decrypted packet to a GstBuffer that can be sent downstream in the pipeline.
	// TODO Figure out if we can avoid copying the data.
	GstBuffer* buffer = gst_rtp_buffer_new_copy_data(data.getBuffer(),
			data.getSize());

	// This function takes ownership of the buffer.
	// _debug("Pushing buffer in the queue");
	gst_app_src_push_buffer(GST_APP_SRC(appsrc), buffer);
}

void H264GstDecoder::init() {
	gst_init(0, NULL);

	// Create pipeline
	pipeline = gst_pipeline_new("sfl_h264_decoding");

	// Create the appsrc, in which we will inject (unencrypted) rtp packets
	appsrc = gst_element_factory_make("appsrc", "sfl_appsrc");
	if (appsrc == NULL) {
		throw MissingGstPluginException(
				"Plugin \"appsrc\" could not be found. "
					"Check your install (you need gst-plugins-base). "
					"Run gst-inspect to get the list of available plugins");
	}

	GstAppSrcCallbacks sourceCallbacks;
	sourceCallbacks.need_data = NULL;
	sourceCallbacks.enough_data = H264GstDecoder::onEnoughData;
	sourceCallbacks.seek_data = NULL;
	gst_app_src_set_callbacks(GST_APP_SRC(appsrc), &sourceCallbacks, this, NULL);

	gst_app_src_set_stream_type(GST_APP_SRC(appsrc), GST_APP_STREAM_TYPE_STREAM);
	gst_app_src_set_max_bytes(GST_APP_SRC(appsrc), 1000000); // 1Mb

	GstCaps* caps = gst_caps_new_simple("application/x-rtp",
	  	      "media", G_TYPE_STRING, "video",
		      "clock-rate", G_TYPE_INT, 90000,
		      "encoding-name", G_TYPE_STRING, "H264",
		      //"sprop-parameter-sets", G_TYPE_STRING, "\"Z01AFZJUC4S8v/8EavQkIAAAAwAgAAAGQeLF1AA\\=\\,aO48gAA\\=\"",
		      "payload", G_TYPE_INT, 96,
			NULL);

	gst_app_src_set_caps(GST_APP_SRC(appsrc), caps);
	gst_caps_unref (caps);

	// Create the depayloader element
	depayloader = gst_element_factory_make("rtph264depay", "sfl_depayloader");
	if (depayloader == NULL) {
		throw MissingGstPluginException(
				"Plugin \"rtph264depay\" could not be found. "
					"Check your install (you need gst-plugins-good). "
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

	// Branch the pipeline. Might become a recording branch in the future.
	tee = gst_element_factory_make("tee", "sfl_tee");
	if (tee == NULL) {
		throw MissingGstPluginException("Plugin \"tee\" could not be found. "
			"Check your install (you need gst-plugins-base). "
			"Run gst-inspect to get the list of available plugins");
	}

	// Debug window branch.
	displayQueue = gst_element_factory_make("queue", "sfl_display_queue");
	displayWindow = gst_element_factory_make("autovideosink",
			"sfl_debug_window");
	if (displayWindow == NULL) {
		throw MissingGstPluginException("Plugin \"tee\" could not be found. "
			"Check your install (you need gst-plugins-base). "
			"Run gst-inspect to get the list of available plugins");
	}

	gst_element_link(tee, displayQueue);
	gst_element_link(displayQueue, displayWindow);

	// Appsink branch. This might be replaced by v4lsink.
	appsinkQueue = gst_element_factory_make("queue", "sfl_appsink_queue");
	appsink = gst_element_factory_make("appsink", "sfl_appsink");
	if (appsink == NULL) {
		throw MissingGstPluginException(
				"Plugin \"appsink\" could not be found. "
					"Check your install (you need gst-plugins-base). "
					"Run gst-inspect to get the list of available plugins");
	}

	gst_element_link(tee, appsinkQueue);
	gst_element_link(appsinkQueue, appsink);

	// Add everything to the pipeline.
	gst_bin_add_many(GST_BIN(pipeline), appsrc, depayloader, parser, decoder,
			ffmpegcolorspace, deinterlace, videoscale, tee, displayQueue,
			appsinkQueue, displayWindow, appsink, NULL);

	// Link all elements together
//	if (gst_element_link_many(appsrc, depayloader, parser, decoder,
//			ffmpegcolorspace, deinterlace, videoscale, tee, NULL) == FALSE) {
//		throw VideoDecodingException("Failed to link one or more elements.");
//	}

	if (gst_element_link_many(appsrc, depayloader, parser, decoder,
			ffmpegcolorspace, deinterlace, videoscale, displayWindow, NULL) == FALSE) {
		throw VideoDecodingException("Failed to link one or more elements.");
	}

	// Configure callbacks for the appsink
	GstAppSinkCallbacks sinkCallbacks;
	sinkCallbacks.eos = NULL;
	sinkCallbacks.new_preroll = NULL;
	sinkCallbacks.new_buffer = H264GstDecoder::onNewBuffer;
	sinkCallbacks.new_buffer_list = NULL;
	gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &sinkCallbacks, this, NULL);
}

H264GstDecoder::H264GstDecoder() throw (VideoDecodingException,
		MissingPluginException) :
	VideoDecoder() {
	_debug("Initializing Decoder ...");
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

H264GstDecoder::~H264GstDecoder() {
	stop();
}

}
