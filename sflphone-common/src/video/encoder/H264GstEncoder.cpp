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

#include "video/encoder/H264GstEncoder.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"

namespace sfl {

H264GstEncoder::H264GstEncoder(VideoInputSource& source)
		throw (VideoDecodingException, MissingPluginException) :
	VideoEncoder(source), injectableEnd(NULL), retrievableEnd(NULL) {

	gst_init(0, NULL);

	// Create pipeline
	GstElement* gstPipeline = gst_pipeline_new("sfl_h264_encoding");

	// Set caps
	GstCaps* sourceCaps = gst_caps_new_simple("application/x-rtp", "media",
			G_TYPE_STRING, "video", "clock-rate", G_TYPE_INT, 90000,
			"encoding-name", G_TYPE_STRING, "H264", "payload", G_TYPE_INT, 96,
			NULL);

	// Create the encoding portion
	GstElement* colorspace = gst_element_factory_make("ffmpegcolorspace",
			"sfl_encoder_csp");
	GstElement* videoscale = gst_element_factory_make("videoscale",
			"sfl_encoder_scale");
	GstElement* encoder =
			gst_element_factory_make("x264enc", "sfl_encoder_enc");
	GstElement* payloader = gst_element_factory_make("rtph264pay",
			"sfl_encoder_pay");

	g_object_set(G_OBJECT(encoder), "byte-stream", TRUE, NULL);
	g_object_set(G_OBJECT(encoder), "bitrate", 300, NULL);

	// Add the elements in bin
	gst_bin_add_many(GST_BIN(gstPipeline), colorspace, videoscale, encoder,
			payloader, NULL);

	// Link elements together
	if (gst_element_link_many(colorspace, videoscale, encoder, payloader, NULL)
			== FALSE) {
		throw VideoDecodingException("Failed to link one or more elements.");
	}

	// Create an injectableEnd and retrievableEnd pipeline
	Pipeline pipeline(gstPipeline);
	injectableEnd = new InjectablePipeline(pipeline, colorspace, sourceCaps);
	retrievableEnd = new RetrievablePipeline(pipeline, payloader);

	outputObserver = new PipelineEventObserver(this);
	retrievableEnd->addObserver(outputObserver);
}

H264GstEncoder::~H264GstEncoder() {
	deactivate();

	delete retrievableEnd;
	delete injectableEnd;
}

void H264GstEncoder::encode(const VideoFrame* frame)
		throw (VideoEncodingException) {
	GstBuffer* buffer = gst_buffer_new();
	GST_BUFFER_SIZE(buffer) = frame->getSize();
	GST_BUFFER_DATA(buffer) = (guint8*) frame->getFrame();

	injectableEnd->inject(buffer);
}

void H264GstEncoder::activate() {
	VideoEncoder::activate();

	_info("Activating h264 encoder");

	retrievableEnd->start();
}

void H264GstEncoder::deactivate() {
	VideoEncoder::deactivate();

	_info("Deactivating h264 encoder");

	clearObservers();
	retrievableEnd->removeObserver(outputObserver);
	retrievableEnd->stop();
}

}
