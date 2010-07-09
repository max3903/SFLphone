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

#include "video/encoder/GstEncoder.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"

namespace sfl {

GstEncoder::GstEncoder(VideoInputSource& source,
		unsigned maxFrameQueued) throw (VideoDecodingException,
		MissingPluginException) :
	VideoEncoder(source) {
	init(source, maxFrameQueued);
}

GstEncoder::GstEncoder(VideoInputSource& source)
		throw (VideoDecodingException, MissingPluginException) :
	VideoEncoder(source) {
	init(source, MAX_FRAME_QUEUED);
}

void GstEncoder::init(VideoInputSource& source, unsigned maxFrameQueued)
		throw (VideoDecodingException, MissingPluginException) {
	gst_init(0, NULL);

	// Create a new pipeline
	Pipeline pipeline(std::string("sfl_") + getMimeSubtype() + std::string("_encoding"));
	pipeline.setPrefix("sfl_encoder_");

	GstElement* ffmpegcolorspace = pipeline.addElement("ffmpegcolorspace");
	GstElement* videoscale = pipeline.addElement("videoscale", ffmpegcolorspace);

	// Ask the derived child to take care of building the encoding portion of the pipeline itself. A knowledge that we
	// can't have at this point in the object hierarchy (template design pattern).
	buildFilter(pipeline);

	// Add an injectable endpoint
	VideoFormat format = getVideoInputSource()->getOutputFormat();

	GstCaps* sourceCaps = gst_caps_new_simple(
			"video/x-raw-rgb", // FIXME Hardcoded !
			"format", GST_TYPE_FOURCC, GST_STR_FOURCC(
					format.getFourcc().c_str()), "width", G_TYPE_INT,
			format.getWidth(),
			"height",
			G_TYPE_INT,
			format.getHeight(),
			"bpp",
			G_TYPE_INT,
			32, // FIXME Hardcoded !,
			"depth",
			G_TYPE_INT,
			32, // FIXME Hardcoded !
			"endianness", G_TYPE_INT, 4321, "red_mask", G_TYPE_INT, 65280,
			"green_mask", G_TYPE_INT, 16711680, "blue_mask", G_TYPE_INT,
			-16777216, "framerate", GST_TYPE_FRACTION,
			format.getPreferredFrameRate().getNumerator(),
			format.getPreferredFrameRate().getDenominator(), NULL);

	injectableEnd = new InjectablePipeline(pipeline, sourceCaps, maxFrameQueued);  // FIXME Do some calculation

	// Add a retrievable endpoint
	retrievableEnd = new RetrievablePipeline(pipeline);
	outputObserver = new PipelineEventObserver(this);
	retrievableEnd->addObserver(outputObserver);

	// Connect both endpoints to the graph.
	injectableEnd->setSink(ffmpegcolorspace);
	retrievableEnd->setSource(getTail());
}

GstEncoder::~GstEncoder() {
	deactivate();

	delete retrievableEnd;
	delete injectableEnd;
}

void GstEncoder::encode(const VideoFrame* frame)
		throw (VideoEncodingException) {
	GstBuffer* buffer = gst_buffer_new();
	GST_BUFFER_SIZE(buffer) = frame->getSize();
	GST_BUFFER_DATA(buffer) = (guint8*) frame->getFrame();

	_info("Encoding frame of raw size %d", GST_BUFFER_SIZE(buffer));

	injectableEnd->inject(buffer);
}

void GstEncoder::activate() {
	VideoEncoder::activate();

	_info("Activating encoder");

	retrievableEnd->start();
}

void GstEncoder::deactivate() {
	VideoEncoder::deactivate();

	_info("Deactivating encoder");

	clearObservers();

	retrievableEnd->removeObserver(outputObserver);
	retrievableEnd->stop();
}

}
