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
#include "util/Buffer.h"
#include "logger.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>

#include <stdint.h>

namespace sfl {

gboolean H264GstDecoder::gstreamer_bus_callback(GstBus *bus, GstMessage *msg,
		gpointer data) {
	GMainLoop *loop = data;

	switch (GST_MESSAGE_TYPE(msg)) {
	case GST_MESSAGE_EOS:
		ERROR << "End-of-stream" << std::endl;
		g_main_loop_quit(loop);
		break;
	case GST_MESSAGE_ERROR: {
		gchar *debug = NULL;
		GError *err = NULL;

		gst_message_parse_error(msg, &err, &debug);

		ERROR << "Caught Gstreamer error";
		ERROR << err->message << std::endl;

		g_error_free(err);

		if (debug) {
			DEBUG << "Debug details :";
			DEBUG << debug << std::endl;
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

gboolean H264GstDecoder::read_data(gpointer object) {
	H264GstDecoder* self = (H264GstDecoder*) object;

	// Dequeue some encoded NAL unit
	if (self->nalUnits.empty()) {
		WARNING << "Decoding queue is empty." << std::endl;
		return TRUE;
	}
	Buffer<uint8_t> encodedFrame = self->nalUnits.pop();

	// Put in a GstBuffer
	GstBuffer* buffer = gst_buffer_new();
	GST_BUFFER_DATA(buffer) = encodedFrame.getBuffer();
	GST_BUFFER_SIZE(buffer) = encodedFrame.getSize();

	// Push the buffer down to the decoder
	GstFlowReturn ret;
	g_signal_emit_by_name(self->appsrc, "push-buffer", buffer, &ret);
	gst_buffer_unref(buffer);
	if (ret != GST_FLOW_OK) {
		// Some error, stop sending data
		return FALSE;
	}

	return TRUE;
}

void H264GstDecoder::start_feed(GstElement * playbin, guint size, gpointer object) {
	H264GstDecoder* self = (H264GstDecoder*) object;

	if (self->sourceid == 0) {
		GST_DEBUG("start feeding");
		self->sourceid = g_idle_add((GSourceFunc) read_data, object);
	}
}

void H264GstDecoder::stop_feed(GstElement * playbin, gpointer object) {
	H264GstDecoder* self = (H264GstDecoder*) object;

	if (self->sourceid != 0) {
		GST_DEBUG("Stop feeding.");
		g_source_remove(self->sourceid);
		self->sourceid = 0;
	}
}

void H264GstDecoder::init_appsrc_callback(GObject * object, GObject * orig,
		GParamSpec * pspec, gpointer object) {
	H264GstDecoder* self = (H264GstDecoder*) object;

	// Get a handle to the appsrc
	g_object_get(orig, pspec->name, &self->appsrc, NULL);

	// configure the appsrc, we will push data into the appsrc from the mainloop.
	g_signal_connect(self->appsrc, "need-data", G_CALLBACK(start_feed), self);
	g_signal_connect(self->appsrc, "enough-data", G_CALLBACK(stop_feed), self);
}

GstFlowReturn H264GstDecoder::new_buffer(GstAppSink * sink, gpointer object) {
	H264GstDecoder* self = (H264GstDecoder*) object;

	self->dispatchEvent();

	return GST_FLOW_OK;
}

void H264GstDecoder::dispatchEvent() {
	GstBuffer * buffer = NULL;
	buffer = gst_app_sink_pull_buffer(GST_APP_SINK(appsink));
	if (buffer == NULL) {
		ERROR << "The buffer pulled from appsink is NULL" << std::endl;
		return; //TODO : do something
	}

	// Notify
	notifyAll(Buffer<uint8>(GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer)));

	gst_buffer_unref(buffer);
}

H264GstDecoder::H264GstDecoder(const VideoFormat& encodingFormat,
		const VideoFormat& decodingFormat) throw (VideoDecodingException,
		MissingPluginException) :
	VideoDecoder(encodingFormat, decodingFormat) {

	gst_init(0, NULL);

	gstApp = new GstApp();

	// Create main loop
	gstApp->loop = g_main_loop_new(NULL, FALSE);

	// Create pipeline
	gstApp->pipeline = gst_pipeline_new("sfl_h264_decoding");

	// Watch for messages on the pipeline's bus
	gstApp->bus = gst_pipeline_get_bus(GST_PIPELINE(gstApp->pipeline));
	gst_bus_add_watch(gstApp->bus, gstreamer_bus_callback, gstApp->loop);
	gst_object_unref(gstApp->bus);

	// Create the appsrc, in which we will inject depayloaded but still encoded frames.
	GstElement* appsrc = gst_element_factory_make("appsrc", "appsrc");
	if (gstApp->appsrc == NULL) {
		throw MissingGstPluginException("Plugin appsrc could not be found. "
			"Check your install (you need gst-plugins-base). "
			"Run gst-inspect to get the list of available plugins");
	}

	// Create the H264 decoder
	gstApp->decoder = gst_element_factory_make("ffdec_h264", "decoder");
	if (gstApp->decoder == NULL) {
		throw MissingGstPluginException(
				"Plugin ffdec_h264 could not be found. "
					"Check your install (you need gstreamer-ffmpeg). "
					"Run gst-inspect to get the list of available plugins");
	}

	// Create the appsink where decoded frames will be retrieved.
	gstApp->appsink = gst_element_factory_make("appsink", "appsink");
	if (gstApp->appsink == NULL) {
		throw MissingGstPluginException("Plugin appsink could not be found. "
			"Check your install (you need gst-plugins-base). "
			"Run gst-inspect to get the list of available plugins");
	}

	// Configure callbacks for the appsink
	GstAppSinkCallbacks callbacks;
	callbacks.eos = NULL;
	callbacks.new_preroll = NULL;
	callbacks.new_buffer = new_buffer;
	callbacks.new_buffer_list = NULL;
	gst_app_sink_set_callbacks(GST_APP_SINK(gstApp->appsink), &callbacks, this, NULL);

	// Add everything to the pipeline.
	gst_bin_add_many(GST_BIN(gstApp->pipeline), gstApp->appsink, gstApp->decoder,
			NULL);

	// Link all elements together
	if (gst_element_link_many(gstApp->appsink, gstApp->decoder, NULL) == FALSE) {
		throw VideoDecodingException("Failed to link one or more elements.");
	}

	// Connect signal. Called when the appsrc is created.
	g_signal_connect(GST_BIN(gstApp->pipeline), "deep-notify::source",
			(GCallback) init_appsrc_callback, this);

	start();
}

void H264GstDecoder::~H264Decoder() {
	stop();
}

void H264GstDecoder::start() throw (VideoDecodingException) {
	GstStateChangeReturn ret = gst_element_set_state(gstApp->pipeline,
			GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print("Failed to start up pipeline!\n");

		// Check if there is an error message with details on the bus
		std::string errorMessage = "An error occured while starting decoding. ";
		GstMessage *msg = gst_bus_poll(gstApp->bus, GST_MESSAGE_ERROR,
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

	g_main_loop_run(gstApp->loop);
}

void H264GstDecoder::stop() throw (VideoDecodingException) {
	gst_element_set_state(gstApp->pipeline, GST_STATE_NULL);
	gst_object_unref(gstApp->pipeline);
}

int H264GstDecoder::decode(Buffer<uint8_t>& buffer) throw (VideoDecodingException) {
	nalUnits.push(buffer);
}

}
