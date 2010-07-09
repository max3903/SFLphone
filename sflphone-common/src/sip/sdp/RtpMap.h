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
#ifndef __SFL_RTP_MAP_H__
#define __SFL_RTP_MAP_H__

#include <cc++/tokenizer.h>

#include <string>
#include <stdlib.h>

namespace sfl {
/**
 * Unmutable class for holding the attributes of a a=rtpmap SDP line.
 */
class RtpMap {
public:
	/**
	 * @param payloadType The static or dynamic payload type. For example : 98
	 * @param codec The codec name. Eg: H264
	 * @param clockRate The clock rate for this codec. Eg: 9000
	 * @param param Optional parameters for this codec.
	 */
	RtpMap(const std::string& payloadType, const std::string& codec,
			unsigned clockRate, const std::string param) {
		this->payloadType = atoi(payloadType.c_str());
		this->codec = codec;
		this->clockRate = clockRate;
		this->param = param;
	}

	/**
	 * @return The clock rate for this codec.
	 */
	unsigned getClockRate() const {
		return clockRate;
	}

	/**
	 * @return The codec name.
	 */
	std::string getCodecName() const {
		return codec;
	}

	/**
	 * @return Optional parameters for this codec.
	 */
	std::string getParam() const {
		return param;
	}

	/**
	 * Split the params of a a=fmtp line into its individual parameter, separated by ";" tokens.
	 * Note that RFC4566 indicates no assumption about how this piece of data should be formatted.
	 * @return The param portion of the a=fmtp line, splitted into individual parameters in a "property-name:property-value" mapping.
	 */
	std::map<std::string, std::string> getParamParsed() const {
		std::map<std::string, std::string> properties;

		ost::StringTokenizer paramsTokenizer(param.c_str(), ";", false, true /* Trim wspace */);
		ost::StringTokenizer::iterator it;
		for (it = paramsTokenizer.begin(); it != paramsTokenizer.end(); ++it) {
			std::string param(*it);
			size_t pos = param.find("=");
			std::string property = param.substr(pos+1); // FIXME Too naive !
			std::string value = param.substr(0, pos); // FIXME Too naive !

			properties.insert(std::pair<std::string, std::string>(property, value));
		}

		return properties;
	}

	/**
	 * @return The static or dynamic payload type.
	 */
	unsigned getPayloadType() const {
		return payloadType;
	}

private:
	unsigned payloadType;
	std::string codec;
	unsigned clockRate;
	std::string param;
};
}
#endif
