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

#include "string.h"

namespace sfl {
GstEncoder::GstEncoder() :
	VideoEncoder() {
}

GstEncoder::GstEncoder(const VideoFormat& format)
		throw (VideoEncodingException, MissingPluginException) :
	VideoEncoder(format), maxFrameQueued(MAX_FRAME_QUEUED),
			injectableEnd(NULL), retrievableEnd(NULL), bufferCaps(NULL) {
}

GstEncoder::GstEncoder(const VideoFormat& format, unsigned maxFrameQueued)
		throw (VideoDecodingException, MissingPluginException) :
	VideoEncoder(format), maxFrameQueued(maxFrameQueued), injectableEnd(NULL),
			retrievableEnd(NULL), bufferCaps(NULL) {
}

void GstEncoder::setVideoInputFormat(const VideoFormat& format) {
	VideoEncoder::setVideoInputFormat(format);

	// Configure the caps at the source
	if (injectableEnd != NULL) {
		configureSource();
	}
}

void GstEncoder::setParameter(const std::string& name, const std::string& value) {
	if (injectableEnd == NULL) {
		// Wait for the source to be set, creating the caps at the same time.
		userSpecifiedParameters.push_back(std::pair<std::string, std::string>(
				name, value));
	} else {
		// Apply the parameter immediately to the caps.
		injectableEnd->setField(name, value);
	}
}

std::string GstEncoder::getParameter(const std::string& name) {
	ParameterIterator it = parameters.find(name);
	if (it != parameters.end()) {
		return (*it).second;
	}

	return std::string("");
}

void GstEncoder::setBufferCaps(GstCaps* caps) {
	bufferCaps = caps;
}

GstCaps* GstEncoder::getBufferCaps() {
	return bufferCaps;
}

void GstEncoder::addParameter(const std::string& name, const std::string& value) {
	std::pair<ParameterIterator, bool> ret = parameters.insert(ParameterEntry(
			name, value));
	if (ret.second == false) {
		// Update the parameter with the most recent value
		parameters.erase(ret.first);

		parameters.insert(ParameterEntry(name, value));
	}
}

gboolean GstEncoder::extractParameter(GQuark field_id, const GValue* value,
		gpointer user_data) {
	GstEncoder* self = (GstEncoder*) user_data;

	GType type = G_VALUE_TYPE(value);
	const gchar* field_name = g_quark_to_string(field_id);

	if (type == G_TYPE_STRING) {
		self->addParameter(field_name, g_value_get_string(value));
	}

	return TRUE;
}

void GstEncoder::generateSdpParameters() {
	_debug("Generating SDP by injecting frames ...");
	// Set the selector to point on the videotestsrc
	selectVideoTestSrc(true);

	// Block until a buffer becomes available
	_debug("Blocking until a buffer becomes available ...");
	GstBuffer* buffer = retrievableEnd->getBuffer();
	if (buffer == NULL) {
		_error ("Got NULL buffer");
	}

	// Save a copy of the caps
	setBufferCaps(gst_buffer_get_caps(buffer));

	// Extract the SDP parameters from the caps
	GstStructure* structure = gst_caps_get_structure(getBufferCaps(), 0);

	gst_structure_foreach(structure, extractParameter, (gpointer) this);

	// Set the selector to point on the appsrc
	selectVideoTestSrc(false);
}

static void selector_blocked(GstPad * pad, gboolean blocked, gpointer user_data) {
	/* no nothing */
	_debug ("blocked callback, blocked: %d", blocked);
}

void GstEncoder::selectVideoTestSrc(bool selected) {
	if (selected) {
		_debug("Selecting videotestsrc on input-selector ...");

		//gst_pad_set_blocked (appsrcPad, TRUE);
		//gst_pad_set_blocked (videotestsrcPad, FALSE);
		if (retrievableEnd->isPlaying()) {
			gst_pad_set_blocked_async(videotestsrcPad, FALSE, selector_blocked,
					NULL);
		}

		g_object_set(G_OBJECT(inputselector), "active-pad", videotestsrcPad,
				NULL);

		//	    g_signal_emit_by_name(G_OBJECT (inputselector), "block", NULL);
		//	    g_signal_emit_by_name(G_OBJECT (inputselector), "switch", videotestsrcPad, -1, -1, NULL);
	} else {
		_debug("Selecting encoding source on input-selector ...");

		if (retrievableEnd->isPlaying()) {
			gst_pad_set_blocked_async(videotestsrcPad, TRUE, selector_blocked,
					NULL);
		}

		g_object_set(G_OBJECT(inputselector), "active-pad", appsrcPad, NULL);
	}
}

void GstEncoder::configureSource() {
	// Create the new caps for this video source
	VideoFormat format = getVideoInputFormat();
	std::ostringstream caps;
	caps << "video/x-raw-rgb" << ",format=(fourcc)"
			<< format.getFourcc().c_str() << ",height=(int)"
			<< format.getHeight() << ",width=(int)" << format.getWidth()
			<< ",bpp=(int)" << 32 << ",depth=(int)" << 32
			<< ",endianness=(int)" << 4321 << ",red_mask=(int)" << 65280
			<< ",green_mask=(int)" << 16711680 << ",blue_mask=(int)"
			<< -16777216 << ",framerate=(fraction)"
			<< format.getPreferredFrameRate().getNumerator() << "/"
			<< format.getPreferredFrameRate().getDenominator();

	GstCaps* sourceCaps = gst_caps_from_string((caps.str()).c_str());
	_debug ("Setting caps %s on encoder source", caps.str().c_str());

	// Set the new maximum size on the input queue
	injectableEnd->setMaxQueueSize(10 /** Frames */* format.getWidth()
			* format.getHeight() * 32); // FIXME Hardcoding !

	// Append the optional parameters (if any)
	std::list<std::pair<std::string, std::string> >::iterator it;

	for (it = userSpecifiedParameters.begin(); it
			!= userSpecifiedParameters.end(); it++) {
		GValue gstValue;
		memset(&gstValue, 0, sizeof(GValue));
		g_value_init(&gstValue, G_TYPE_STRING);
		g_value_set_string(&gstValue, ((*it).second).c_str());

		gst_caps_set_value(sourceCaps, (*it).first.c_str(), &gstValue);
	}

	// Set the caps on the appsrc
	injectableEnd->setCaps(sourceCaps);

	// Set the caps on the videotestsrc
	g_object_set(G_OBJECT(capsFilterVideoTestSrc), "caps", sourceCaps, NULL);
}

void GstEncoder::init() throw (VideoDecodingException, MissingPluginException) {
	gst_init(0, NULL);

	// Create a new pipeline
	Pipeline pipeline(std::string("sfl_") + getMimeSubtype() + std::string(
			"_encoding"));
	pipeline.setPrefix("sfl_encoder_");

	// Create a videotestsrc, used to find SDP parameters
	videotestsrc = pipeline.addElement("videotestsrc");
	g_object_set(G_OBJECT(videotestsrc), "pattern", 2, NULL);
	g_object_set(G_OBJECT(videotestsrc), "is-live", TRUE, NULL);
	g_object_set(G_OBJECT(videotestsrc), "do-timestamp", TRUE, NULL);

	capsFilterVideoTestSrc = pipeline.addElement("capsfilter", videotestsrc);

	// Create an input selector element, to switch from videotestsrc to the appsrc on request
	inputselector = pipeline.addElement("input-selector");

	// Link the appsrc to some input of the input selector
	appsrcPad = gst_element_get_request_pad(inputselector, "sink%d");

	// Link the video test src to some input of the input selector
	videotestsrcPad = gst_element_get_request_pad(inputselector, "sink%d");

	pipeline.link(capsFilterVideoTestSrc, videotestsrcPad);

	// Create a color space converter element. Used downstream, after the input selector.
	GstElement* ffmpegcolorspace = pipeline.addElement("ffmpegcolorspace",
			inputselector);
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

	injectableEnd->setSink(appsrcPad);
	retrievableEnd->setSource(getTail());

	// Configure the source with the input format
	configureSource();
}

GstEncoder::~GstEncoder() {
	gst_object_unref(videotestsrcPad);
	gst_object_unref(appsrcPad);

	deactivate();

	delete retrievableEnd;
	delete injectableEnd;
}

void free_encoded_buffer(gpointer mem) {
	_debug("Automatically Freeing buffer");
	g_free(mem);
}

void GstEncoder::encode(const VideoFrame* frame) throw (VideoEncodingException) {
	GstBuffer* buffer = gst_buffer_new();
	GST_BUFFER_SIZE(buffer) = frame->getSize();
	GST_BUFFER_MALLOCDATA(buffer) = (guint8*) g_malloc(frame->getSize());
	GST_BUFFER_DATA(buffer) = GST_BUFFER_MALLOCDATA(buffer);
	GST_BUFFER_FREE_FUNC(buffer) = free_encoded_buffer;

	// Copy the actual data into the buffer
	memcpy(GST_BUFFER_DATA(buffer), frame->getFrame(), frame->getSize());

	// This function takes ownership of the buffer.
	injectableEnd->inject(buffer);

	_debug("Buffer %d injected", frame->getSize());
}

void GstEncoder::activate() {
	VideoEncoder::activate();
	_info ("Activating encoder");

	init();

	retrievableEnd->start();
	selectVideoTestSrc(false);
	_info ("Pipeline started.");
}

void GstEncoder::deactivate() {
	VideoEncoder::deactivate();
	_info ("Deactivating encoder");

	clearObservers();

	retrievableEnd->removeObserver(outputObserver);
	retrievableEnd->stop();
}

}
