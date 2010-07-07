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

#include "video/decoder/H264GstDecoder.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"
#include "util/gstreamer/VideoFormatToGstCaps.h"

namespace sfl {

H264GstDecoder::H264GstDecoder()
		throw (VideoDecodingException, MissingPluginException) :
	VideoDecoder() {
	VideoFormat defaultFormat; // See VideoFormat for the default values that are set. Should be 320x240, RGB, 30/1
	init(defaultFormat);
}

H264GstDecoder::H264GstDecoder(VideoFormat& format)
		throw (VideoDecodingException, MissingPluginException) :
	VideoDecoder() {
	init(format);
}

/**
 * Initialize the Gstreamer pipeline.
 *
 * The appsrc element merely does what udpsrc would do. The only
 * reason for not using it is because we want to keep the security
 * features that we already have with ccRTP.
 *
 * |appsrc| -> |rtph264depay| -> |h264parse| -> |ffdec_h264| ---
 *
 * ----> |ffmpegcolorspace| -> |deinterlace| -> |videoscale| -> |queue| -> |autovideosink| (debug branch)
 */
void H264GstDecoder::init(VideoFormat& format)
		throw (VideoDecodingException, MissingPluginException) {
	gst_init(0, NULL);

	// Create the encoding portion of the pipeline
	// TODO catch exceptions
	Pipeline pipeline("sfl_h264_decoding");
	pipeline.setPrefix("sfl_decoder_");

	GstElement* rtph264depay = pipeline.addElement("rtph264depay");
	GstElement* previous = pipeline.addElement("h264parse", rtph264depay);
	previous = pipeline.addElement("ffdec_h264", previous);

	pipeline.addTee("main_tee", previous);

	GstPad* debugBranch = pipeline.branch("main_tee");
	previous = pipeline.addElement("queue", debugBranch);
	previous = pipeline.addElement("ffmpegcolorspace", previous);
	previous = pipeline.addElement("deinterlace", previous);
	pipeline.addElement("autovideosink", previous);

	GstPad* outputBranch = pipeline.branch("main_tee");
	previous = pipeline.addElement("queue", outputBranch);
	previous = pipeline.addElement("ffmpegcolorspace", previous);
	previous = pipeline.addElement("deinterlace", previous);
	GstElement* videoscale = pipeline.addElement("videoscale", previous);

	// Add injectable endpoint
	GstCaps* sourceCaps = gst_caps_from_string(
							"application/x-rtp,"
							"media=(string)video,"
							"encoding-name=(string)H264,"
							"clock-rate=(int)90000,"
							"payload=(int)96");
	injectableEnd = new InjectablePipeline(pipeline, sourceCaps);

	// Add retrievable endpoint
	retrievableEnd = new RetrievablePipeline(pipeline, VideoFormatToGstCaps(format));
	outputObserver = new PipelineEventObserver(this);
	retrievableEnd->addObserver(outputObserver);

	// Connect both endpoints to the graph.
	injectableEnd->setSink(rtph264depay);
	retrievableEnd->setSource(videoscale);
}

H264GstDecoder::~H264GstDecoder() {
	deactivate();

	delete retrievableEnd;
	delete injectableEnd;
}

void H264GstDecoder::decode(ManagedBuffer<uint8>& data)
		throw (VideoDecodingException) {
	_debug("Decoding RTP packet ...");

	// Convert the raw decrypted packet to a GstBuffer that can be sent downstream in the pipeline.
	// TODO Figure out if we can avoid copying the data.
	GstBuffer* buffer = gst_rtp_buffer_new_copy_data(data.getBuffer(),
			data.getSize());

	// This function takes ownership of the buffer.
	injectableEnd->inject(buffer);
}

void H264GstDecoder::setOutputFormat(VideoFormat& format)
{
	retrievableEnd->setCaps(VideoFormatToGstCaps(format));
}

void H264GstDecoder::activate() {
	_info("Activating h264 decoder");

	// Does not matter whether we call start() on injectable or retrievable endpoints.
	injectableEnd->start();
}

void H264GstDecoder::deactivate() {
	_info("Deactivating h264 decoder");

	// Clear our own observers
	clearObservers();

	// Unsubscribe this object from appsink events.
	retrievableEnd->removeObserver(outputObserver);

	// Does not matter whether we call stop() on injectable or retrievable endpoints.
	retrievableEnd->stop();
}

}
