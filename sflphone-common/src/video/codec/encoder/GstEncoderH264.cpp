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

#include "GstEncoderH264.h"
#include <iostream>

namespace sfl {

void GstEncoderH264::buildFilter(Pipeline& pipeline)
		throw (MissingPluginException) {

	x264enc = pipeline.addElement("x264enc");

	// Generate byte stream format of NALU
	//g_object_set(G_OBJECT(x264enc), "byte-stream", TRUE, NULL);

	// Enable automatic multithreading
	g_object_set(G_OBJECT(x264enc), "threads", 0, NULL);

	// If "cabac" entropy coding is enabled or "bframes"
	// are allowed, then Main Profile is in effect, and
	// otherwise Baseline profile applies.
	g_object_set(G_OBJECT(x264enc), "cabac", FALSE, NULL);

	// Set default bitrate
	g_object_set(G_OBJECT(x264enc), "bitrate", 768, NULL);

	rtph264pay = pipeline.addElement("rtph264pay", x264enc);

}

void GstEncoderH264::setVideoInputFormat(const VideoFormat& format) {
	GstEncoder::setVideoInputFormat(format);

	generateSdpParameters();

	// Now a few hacks and tricks to get the "profile-level-id"
	// Get the src pad on the x264enc element, that contains the "codec_data" field
	GstPad* srcPad = gst_element_get_static_pad(GST_ELEMENT(x264enc), "src");
	if (srcPad == NULL) {
		return;
	}

	// Get the caps on the src pad
	GstCaps* caps = gst_pad_get_caps(srcPad);
	if (!caps) {
		return;
	}

	// Get the first structure in the caps
	GstStructure* structure = gst_caps_get_structure(caps, 0);
	if (!structure) {
		return;
	}
	_debug ("Structure 0 on src pad of x264enc is %" GST_PTR_FORMAT, structure);

	// Get the codec_data field
	const GValue* value = gst_structure_get_value(structure, "codec_data");
	if (!value) {
		return;
	}
	GstBuffer* buffer = gst_value_get_buffer(value);
	guint8* data = GST_BUFFER_DATA(buffer);
	guint size = GST_BUFFER_SIZE(buffer);

	// Create the base 16 representation of the profile-level-id
	char profileBase16[7];
	snprintf(profileBase16, 7, "%06X", (data[1] << 16) | (data[2] << 8) | data[3]);
	_debug("profile-level-id %s", profileBase16);

	// Keep the parameter internally
	addParameter("profile-level-id", profileBase16);

	gst_caps_unref(caps);
	gst_object_unref(srcPad);
}

std::string GstEncoderH264::getParameter(const std::string& name) {
	// The "packetization-mode" SDP format property is known as
	// "scan-mode" in Gstreamer.
	if (name == "packetization-mode") {
		int mode = 0;
		g_object_get(rtph264pay, "scan-mode", &mode, NULL);
		std::stringstream ss;
		ss << mode;
		return ss.str();
	}

	return GstEncoder::getParameter(name);
}

GstElement* GstEncoderH264::getHead() {
	return x264enc;
}

GstElement* GstEncoderH264::getTail() {
	return rtph264pay;
}

}
