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

#include "GstEncoderRaw.h"

namespace sfl {

enum {
	SAMPLING, WIDTH, HEIGHT, DEPTH, COLORIMETRY
};

void GstEncoderRaw::setChromaSubsamplingFormat(const std::string& value) {
	// TODO
}

void GstEncoderRaw::setWidth(const std::string& value) {
	// TODO
}

void GstEncoderRaw::setHeight(const std::string& value) {
	// TODO
}

void GstEncoderRaw::setDepth(const std::string& value) {
	// TODO
}

void GstEncoderRaw::setColorimetry(const std::string& value) {
	// TODO
}

void GstEncoderRaw::setProperty(int index, const std::string& value) {
	switch (index) {
	case SAMPLING:
		setChromaSubsamplingFormat(value);
		break;
	case WIDTH:
		setWidth(value);
		break;
	case HEIGHT:
		setHeight(value);
		break;
	case COLORIMETRY:
		setColorimetry(value);
		break;
	case DEPTH:
		setDepth(value);
		break;
	default:
		break;
	};
}

void GstEncoderRaw::init() throw (VideoDecodingException,
		MissingPluginException) {

	// Determines the color (sub-)sampling mode of the video
	// stream.  Currently defined values are RGB, RGBA, BGR, BGRA,
	// YCbCr-4:4:4, YCbCr-4:2:2, YCbCr-4:2:0, and YCbCr-4:1:1.  New values
	// may be registered as described in section 6.2 of RFC 4175.
	installProperty("sampling", SAMPLING);

	// width: Determines the number of pixels per line.  This is an integer
	// between 1 and 1048561 and MUST be in multiples of 16.
	installProperty("width", WIDTH);

	// height: Determines the number of lines per frame encoded.  This is
	// an integer between 1 and 1048561 and MUST be in multiples of 16.
	installProperty("height", HEIGHT);

	// depth: Determines the number of bits per sample. This is an
	// integer with typical values including 8, 10, 12, and 16.
	installProperty("depth", DEPTH);

	// colorimetry: This parameter defines the set of colorimetric
	// specifications and other transfer characteristics for the video
	// source, by reference to an external specification. Valid values
	// and their specification are:
	//
	// BT601-5 ITU Recommendation BT.601-5 [601]
	// BT709-2 ITU Recommendation BT.709-2 [709]
	// SMPTE240M SMPTE standard 240M [240]
	installProperty("colorimetry", COLORIMETRY);

}

std::string GstEncoderRaw::getCodecName() {
	return "raw";
}

void GstEncoderRaw::buildEncodingFilter(Pipeline& pipeline,
		GstElement* previous) throw (VideoDecodingException,
		MissingPluginException) {
	previous = pipeline.addElement("rtpvrawpay", previous);
}

GstElement* GstEncoderRaw::getTail() {
	return previous;
}

GstEncoderRaw::GstEncoderRaw(VideoInputSource& source)
		throw (VideoDecodingException, MissingPluginException) :
	GstEncoder(source) {
	init();
}

GstEncoderRaw::GstEncoderRaw(VideoInputSource& source, unsigned maxFrameQueued)
		throw (VideoDecodingException, MissingPluginException) :
	GstEncoder(source, maxFrameQueued) {
	init();
}

}
