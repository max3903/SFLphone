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

#include "GstDecoder.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"
#include "util/gstreamer/VideoFormatToGstCaps.h"

namespace sfl {

GstDecoder::GstDecoder() throw (VideoDecodingException, MissingPluginException) :
	VideoDecoder(), injectableEnd(NULL), retrievableEnd(NULL) {
}

void GstDecoder::setParameter(const std::string& name, const std::string& value) {
	if (injectableEnd == NULL) {
		_debug ("GstDecoder: Pushing parameter in list since injectableEnd is null");
		parameters.push_back(std::pair<std::string, std::string>(name, value));
	} else {
		injectableEnd->setField(name, value);
	}
}

std::string GstDecoder::getParameter(const std::string& name) {
	return injectableEnd->getField(name);
}

void GstDecoder::init() throw (VideoDecodingException, MissingPluginException) {
	gst_init(0, NULL);

	// Create the encoding portion of the pipeline
	// TODO catch exceptions
	Pipeline pipeline(std::string("sfl_") + getMimeSubtype() + std::string(
			"_decoding"));
	pipeline.setPrefix("sfl_decoder_");

	// Ask the derived child to take care of building the encoding portion of the pipeline itself. A knowledge that we
	// can't have at this point in the object hierarchy (template design pattern).
	buildFilter(pipeline);

	//    pipeline.addTee ("main_tee", getTail());
	//    GstPad* debugBranch = pipeline.branch ("main_tee");
	//    GstElement* previous = pipeline.addElement ("queue", debugBranch);
	//    previous = pipeline.addElement ("ffmpegcolorspace", previous);
	//    previous = pipeline.addElement ("deinterlace", previous);
	//    pipeline.addElement ("autovideosink", previous);
	//    GstPad* outputBranch = pipeline.branch ("main_tee");
	GstElement* previous = pipeline.addElement("ffmpegcolorspace", getTail());
	GstElement* capsfilter = pipeline.addElement("capsfilter");

	currentCaps = gst_caps_new_simple("video/x-raw-rgb",
			"bpp", G_TYPE_INT, 32, "depth",
			G_TYPE_INT, 32, NULL);

	// Enforce our caps on the output buffers
	g_object_set(G_OBJECT(capsfilter), "caps", currentCaps, NULL);

	pipeline.link(previous, capsfilter);

	// Add injectable endpoint
	std::ostringstream caps;
	caps << "application/x-rtp," << "media=(string)video,"
			<< "encoding-name=(string)" << getMimeSubtype() << ","
			<< "clock-rate=(int)" << getClockRate() << "," << "payload=(int)"
			<< (int) getPayloadType();

	GstCaps* sourceCaps = gst_caps_from_string((caps.str()).c_str());
	_debug ("GstDecoder: Setting caps %s on decoder source", caps.str().c_str());

	injectableEnd = new InjectablePipeline(pipeline, sourceCaps);

	// If there are any optional parameters that were added before activate()
	// was called, apply them now.
	std::list<std::pair<std::string, std::string> >::iterator it;
	_debug ("GstDecoder: Unwinding parameters ...");

	for (it = parameters.begin(); it != parameters.end(); it++) {
		_debug("Setting \"%s\" to \"%s\"", ((*it).first).c_str(), ((*it).second).c_str());
		injectableEnd->setField((*it).first, (*it).second);
	}

	// Add retrievable endpoint
	retrievableEnd = new RetrievablePipeline(pipeline);

	outputObserver = new PipelineEventObserver(this);
	retrievableEnd->addObserver(outputObserver);

	// Connect both endpoints to the graph.
	injectableEnd->setSink(getHead());
	retrievableEnd->setSource(capsfilter);
}

GstDecoder::~GstDecoder() {
	deactivate();

	delete retrievableEnd;
	delete injectableEnd;
}

void GstDecoder::decode(Buffer<uint8>& data) throw (VideoDecodingException) {
	//_debug ("Decoding RTP packet ...");

	// Convert the raw decrypted packet to a GstBuffer that can be sent downstream in the pipeline.
	// TODO Figure out if we can avoid copying the data.
	GstBuffer* buffer = gst_rtp_buffer_new_copy_data(data.getBuffer(),
			data.getSize());

	// This function takes ownership of the buffer.
	injectableEnd->inject(buffer);
}

void GstDecoder::setOutputFormat(VideoFormat& format) {

	_debug("GstDecoder: Setting output format on decoder to %s", format.toString().c_str());
	VideoFormatToGstCaps convert;
	retrievableEnd->setCaps(convert(format));
}

VideoFormat GstDecoder::getOutputFormat() {
	// Extract the colospace, width and height from caps
	_debug ("GstDecoder: caps are %" GST_PTR_FORMAT, currentCaps);

	GstVideoFormat format;
	int width, height;
	gboolean ret = gst_video_format_parse_caps(currentCaps, &format, &width, &height);
	if (ret == false) {
		throw VideoDecodingException("Failed to parse caps");
	}

	int numerator, denominator;
	ret = gst_video_parse_caps_framerate(currentCaps, &numerator, &denominator);
	if (ret == false) {
		throw VideoDecodingException("Failed to parse caps");
	}

	guint32 fourcc = VideoFormatToGstCaps::fourccFromGstVideoFormat(format);

	VideoFormat videoFormat;
	videoFormat.setFourcc(fourcc);
	videoFormat.setWidth(width);
	videoFormat.setHeight(height);
	videoFormat.setMimetype(std::string("video/x-raw-rgb"));
	videoFormat.setFramerate(numerator, denominator);

	_debug("Fourcc format is %d (%s)", fourcc, videoFormat.getFourcc().c_str());

	return videoFormat;
}

void GstDecoder::activate() {
	_info ("GstDecoder: Activating decoder");

	init();

	// Does not matter whether we call start() on injectable or retrievable endpoints.
	injectableEnd->start();
}

void GstDecoder::deactivate() {
	_info ("GstDecoder: Deactivating gstreamer decoder");

	// Clear our own observers
	clearObservers();

	// Unsubscribe this object from appsink events.
	retrievableEnd->removeObserver(outputObserver);

	// Does not matter whether we call stop() on pipeline directly,
	// injectable or retrievable endpoints.
	retrievableEnd->stop();

	// Clear the parameter set
	parameters.clear();

    gst_caps_unref(currentCaps);
}

}
