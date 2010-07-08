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

#include "GstEncoderJpeg2000.h"

namespace sfl {

enum {
	RATE, SAMPLING, INTERLACE, WIDTH, HEIGHT
};

void GstEncoderJpeg2000::setRate(const std::string& value) {
	// TODO
}

void GstEncoderJpeg2000::setChromaSubsamplingFormat(const std::string& value) {
	_warn("max-mbps property not taken into account");
}

void setInterlace(const std::string& value) {
	// TODO
}

void GstEncoderJpeg2000::setWidth(const std::string& value) {
	// TODO
}
void GstEncoderJpeg2000::setHeight(const std::string& value) {
	// TODO
}

void GstEncoderJpeg2000::setProperty(int index, const std::string& value) {
	switch (index) {
	case RATE:
		setRate(value);
		break;
	case SAMPLING:
		setChromaSubsamplingFormat(value);
		break;
	case INTERLACE:
		setInterlace(value);
		break;
	case WIDTH:
		setWidth(value);
		break;
	case HEIGHT:
		setHeight(value);
		break;
	default:
		break;
	};
}

void GstEncoderJpeg2000::init() throw (VideoDecodingException,
		MissingPluginException) {

	// The RTP timestamp clock rate.  The default rate is 90000,
	// but other rates MAY be specified.  Rates below 1000 Hz SHOULD NOT be used.
	installProperty("rate", RATE);

	// A list of values specifying the color space of the payload data.
	installProperty("sampling", SAMPLING);

	// interlace:  Interlace scanning.  If the payload is in interlace
	// format, the acceptable value is "1"; otherwise, the value
	// should be "0".  Each complete image forms, vertically, half the
	// display.  The tp value MUST properly specify the field the
	// image represents: odd(tp=1) or even(tp=2).  If this option is
	installProperty("interlace", INTERLACE);

	// width: Determines the number of pixels per line.  This is an integer
	// between 1 and 1048561 and MUST be in multiples of 16.
	installProperty("width", WIDTH);

	// height: Determines the number of lines per frame encoded.  This is
	// an integer between 1 and 1048561 and MUST be in multiples of 16.
	installProperty("height", HEIGHT);
}

std::string GstEncoderJpeg2000::getCodecName() {
	return "jpeg2000";
}

void GstEncoderJpeg2000::buildEncodingFilter(Pipeline& pipeline,
		GstElement* previous) throw (VideoDecodingException,
		MissingPluginException) {

	GstElement* jpegenc = pipeline.addElement("jpegenc", previous);
	rtpj2kpay = pipeline.addElement("rtpj2kpay", jpegenc);
}

GstElement* GstEncoderJpeg2000::getTail() {
	return rtpj2kpay;
}

GstEncoderJpeg2000::GstEncoderJpeg2000(VideoInputSource& source)
		throw (VideoDecodingException, MissingPluginException) :
	GstEncoder(source) {
	init();
}

GstEncoderJpeg2000::GstEncoderJpeg2000(VideoInputSource& source,
		unsigned maxFrameQueued) throw (VideoDecodingException,
		MissingPluginException) :
	GstEncoder(source, maxFrameQueued) {
	init();
}

}
