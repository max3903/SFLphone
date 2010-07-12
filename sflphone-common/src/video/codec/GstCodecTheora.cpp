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

#include "GstCodecTheora.h"
#include "logger.h"

namespace sfl {

std::string GstCodecTheora::getMimeSubtype() {
	return "H264";
}

void GstCodecTheora::setChromaSubsamplingFormat(const std::string& value) {
	_warn("max-mbps property not taken into account");
}

void GstCodecTheora::setDeliveryMethod(const std::string& value) {
	_warn("sampling property not taken into account");
}

void GstCodecTheora::setConfiguration(const std::string& value) {
	_warn("configuration property not taken into account");
}

void GstCodecTheora::setConfigurationUri(const std::string& value) {
	_warn("configuration-uri property not taken into account");
}

void GstCodecTheora::setWidth(const std::string& value) {

}
void GstCodecTheora::setHeight(const std::string& value) {

}

enum {
	SAMPLING, WIDTH, HEIGHT, DELIVERY_METHOD, CONFIGURATION, CONFIGURATION_URI
};

void GstCodecTheora::setProperty(int index, const std::string& value) {
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
	case DELIVERY_METHOD:
		setDeliveryMethod(value);
		break;
	case CONFIGURATION:
		setConfiguration(value);
		break;
	case CONFIGURATION_URI:
		setConfigurationUri(value);
		break;
	default:
		break;
	};
}

void GstCodecTheora::init() throw (VideoDecodingException,
		MissingPluginException) {

	// sampling: Determines the chroma subsampling format.
	installProperty("sampling", SAMPLING);

	// width: Determines the number of pixels per line.  This is an integer
	// between 1 and 1048561 and MUST be in multiples of 16.
	installProperty("width", WIDTH);

	// height: Determines the number of lines per frame encoded.  This is
	// an integer between 1 and 1048561 and MUST be in multiples of 16.
	installProperty("height", HEIGHT);

	// delivery-method: indicates the delivery methods in use, the
	// possible values are: inline, in_band, out_band/specific_name
	// Where "specific_name" is the name of the out of band delivery method.
	installProperty("delivery-method", DELIVERY_METHOD);

	// configuration: the base16 [11] (hexadecimal) representation of the Packed Headers (Section 3.2.1).
	installProperty("configuration", CONFIGURATION);

	// configuration-uri: the URI of the configuration headers in case of
	// out of band transmission. In the form of
	// "protocol://path/to/resource/". Depending on the specific
	// method the single ident packets could be retrived by their
	// number or aggregated in a single stream, aggregates MAY be
	// compressed using gzip [12] or bzip2 [14] and an sha1 [13]
	// checksum MAY be provided in the form of
	// "protocol://path/to/resource/aggregated.bz2!sha1hash"

	installProperty("configuration-uri", CONFIGURATION_URI);
}

GstCodecTheora::GstCodecTheora() :
	AbstractVideoCodec<GstEncoderTheora, GstDecoderTheora> () {
	init();
}

}
