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

#include "GstEncoder.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"

namespace sfl {
GstEncoder::GstEncoder() throw (VideoEncodingException, MissingPluginException) :
	VideoEncoder(), maxFrameQueued(MAX_FRAME_QUEUED) {
}

GstEncoder::GstEncoder(VideoInputSource& source) throw (VideoDecodingException,
		MissingPluginException) :
	VideoEncoder(source), maxFrameQueued(MAX_FRAME_QUEUED) {
}

GstEncoder::GstEncoder(VideoInputSource& source, unsigned maxFrameQueued)
		throw (VideoDecodingException, MissingPluginException) :
	VideoEncoder(source), maxFrameQueued(maxFrameQueued) {
}

void GstEncoder::setParameter(const std::string& name, const std::string& value) {
	if (injectableEnd->getCaps() == NULL) {
		// Wait for the source to be set, creating the caps at the same time.
		parameters.push_back(std::pair<std::string, std::string>(name, value));
	} else {
		// Apply the parameter immediately to the caps.
		injectableEnd->setField(name, value);
	}
}

std::string GstEncoder::getParameter(const std::string& name) {
	return injectableEnd->getField(name); // FIXME Param might not exists
}

void GstEncoder::setVideoInputSource(VideoInputSource& videoSource) {
	VideoEncoder::setVideoInputSource(videoSource);

	// Create the new caps for this video source
	VideoFormat format = videoSource.getOutputFormat();
	std::ostringstream caps;
	caps << "video/x-raw-rgb" << ",format=(fourcc)" << GST_STR_FOURCC(
			format.getFourcc().c_str()) << ",height=(int)"
			<< format.getHeight() << ",width=(int)" << format.getWidth()
			<< ",bpp=(int)" << 32 << ",depth=(int)" << 32
			<< ",endianness=(int)" << 4321 << ",red_mask=(int)" << 65280
			<< ",green_mask=(int)" << 16711680 << ",blue_mask=(int)"
			<< -16777216 << ",framerate=(framerate)"
			<< format.getPreferredFrameRate().getNumerator() << "/"
			<< format.getPreferredFrameRate().getDenominator();

	GstCaps* sourceCaps = gst_caps_from_string((caps.str()).c_str());
	_debug("Setting caps %s on encoder source", caps.str().c_str());

	// Set the new maximum size on the input queue
	injectableEnd->setMaxQueueSize(10 /** Frames */ * format.getWidth() * format.getHeight() * 32); // FIXME Hardcoding !

	// Append the optional parameters (if any)
	std::list<std::pair<std::string, std::string> >::iterator it;
	for (it = parameters.begin(); it != parameters.end(); it++) {
		GValue gstValue;
		memset(&gstValue, 0, sizeof(GValue));
		g_value_init(&gstValue, G_TYPE_STRING);
		g_value_set_string(&gstValue, ((*it).second).c_str());

		gst_caps_set_value(sourceCaps, (*it).first.c_str(), &gstValue);
	}

	injectableEnd->setCaps(sourceCaps);
}

void GstEncoder::init() throw (VideoDecodingException, MissingPluginException) {
	gst_init(0, NULL);

	// Create a new pipeline
	Pipeline pipeline(std::string("sfl_") + getMimeSubtype() + std::string(
			"_encoding"));
	pipeline.setPrefix("sfl_encoder_");

	GstElement* ffmpegcolorspace = pipeline.addElement("ffmpegcolorspace");
	GstElement* videoscale =
			pipeline.addElement("videoscale", ffmpegcolorspace);

	// Ask the derived child to take care of building the encoding portion of the pipeline itself. A knowledge that we
	// can't have at this point in the object hierarchy (template design pattern).
	buildFilter(pipeline);

	// Link the VideoScale element to the head of the filter
	pipeline.link(videoscale, getHead());

	// Add an injectable endpoint
	injectableEnd = new InjectablePipeline(pipeline);

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

void GstEncoder::encode(const VideoFrame* frame) throw (VideoEncodingException) {
	GstBuffer* buffer = gst_buffer_new();
	GST_BUFFER_SIZE(buffer) = frame->getSize();
	GST_BUFFER_DATA(buffer) = (guint8*) frame->getFrame();

	_info("Encoding frame of raw size %d", GST_BUFFER_SIZE(buffer));

	injectableEnd->inject(buffer);
}

void GstEncoder::activate() {
	VideoEncoder::activate();

	_info("Activating Gstreamer Encoder");

	init();

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
