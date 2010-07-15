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

GstDecoder::GstDecoder()
		throw (VideoDecodingException, MissingPluginException) :
	VideoDecoder() {
}

GstDecoder::GstDecoder(VideoFormat& format)
		throw (VideoDecodingException, MissingPluginException) :
	VideoDecoder(), outputVideoFormat(format) {
}

void GstDecoder::setParameter(const std::string& name, const std::string& value)
{
	injectableEnd->setField(name, value);
}

std::string GstDecoder::getParameter(const std::string& name)
{
	return injectableEnd->getField(name);
}

void GstDecoder::init()
		throw (VideoDecodingException, MissingPluginException) {
	gst_init(0, NULL);

	// Create the encoding portion of the pipeline
	// TODO catch exceptions
	Pipeline pipeline(std::string("sfl_") + getMimeSubtype() + std::string("_decoding"));
	pipeline.setPrefix("sfl_decoder_");

	// Ask the derived child to take care of building the encoding portion of the pipeline itself. A knowledge that we
	// can't have at this point in the object hierarchy (template design pattern).
	buildFilter(pipeline);

	pipeline.addTee("main_tee", getTail());

	GstPad* debugBranch = pipeline.branch("main_tee");
	GstElement* previous = pipeline.addElement("queue", debugBranch);
	previous = pipeline.addElement("ffmpegcolorspace", previous);
	previous = pipeline.addElement("deinterlace", previous);
	pipeline.addElement("autovideosink", previous);

	GstPad* outputBranch = pipeline.branch("main_tee");
	previous = pipeline.addElement("queue", outputBranch);
	previous = pipeline.addElement("ffmpegcolorspace", previous);
	previous = pipeline.addElement("deinterlace", previous);
	GstElement* videoscale = pipeline.addElement("videoscale", previous);

	// Add injectable endpoint
	std::ostringstream caps;
	caps << "application/x-rtp,"
		<< "media=(string)video,"
		<< "encoding-name=(string)" << getMimeSubtype() << ","
		<< "clock-rate=(int)" << getPayloadFormat().getRTPClockRate() << ","
		<< "payload=(int)" << getPayloadFormat().getPayloadType();

	GstCaps* sourceCaps = gst_caps_from_string((caps.str()).c_str());
	_debug("Setting caps %s on decoder source", caps.str().c_str());

	injectableEnd = new InjectablePipeline(pipeline, sourceCaps);

	// Add retrievable endpoint
	VideoFormatToGstCaps convert;
	retrievableEnd = new RetrievablePipeline(pipeline, convert(outputVideoFormat));
	outputObserver = new PipelineEventObserver(this);
	retrievableEnd->addObserver(outputObserver);

	// Connect both endpoints to the graph.
	injectableEnd->setSink(getHead());
	retrievableEnd->setSource(videoscale);
}

GstDecoder::~GstDecoder() {
	deactivate();

	delete retrievableEnd;
	delete injectableEnd;
}

void GstDecoder::decode(Buffer<uint8>& data)
		throw (VideoDecodingException) {
	_debug("Decoding RTP packet ...");

	// Convert the raw decrypted packet to a GstBuffer that can be sent downstream in the pipeline.
	// TODO Figure out if we can avoid copying the data.
	GstBuffer* buffer = gst_rtp_buffer_new_copy_data(data.getBuffer(),
			data.getSize());

	// This function takes ownership of the buffer.
	injectableEnd->inject(buffer);
}

void GstDecoder::setOutputFormat(VideoFormat& format)
{
	VideoFormatToGstCaps convert;
	retrievableEnd->setCaps(convert(format));
}

void GstDecoder::activate() {
	_info("Activating decoder");

	init();

	// Does not matter whether we call start() on injectable or retrievable endpoints.
	injectableEnd->start();
}

void GstDecoder::deactivate() {
	_info("Deactivating gstreamer decoder");

	// Clear our own observers
	clearObservers();

	// Unsubscribe this object from appsink events.
	retrievableEnd->removeObserver(outputObserver);

	// Does not matter whether we call stop() on injectable or retrievable endpoints.
	retrievableEnd->stop();
}

}
