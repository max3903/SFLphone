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
#ifndef __SFL_FMTP_H__
#define __SFL_FMTP_H__

#include "logger.h"

#include <cc++/tokenizer.h>

#include <string>

namespace sfl {

/**
 * Unmutable class for holding the attributes of a a=fmtp SDP line.
 */
class Fmtp {
public:
	/**
	 * @param payloadType The static or dynamic payload type corresponding to some a=rtpmap line.
	 * @params params Codec specific parameters.
	 */
	Fmtp(const std::string& payloadType, const std::string& params) {
		this->payloadType = payloadType;
		this->params = params;
	}

	/**
	 * @return Codec specific parameters.
	 */
    std::string getParams() const
    {
        return params;
    }


	/**
	 * Split the params of a a=fmtp line into its individual parameter, separated by ";" tokens.
	 * Note that RFC4566 indicates no assumption about how this piece of data should be formatted.
	 * @return The param portion of the a=fmtp line, splitted into individual parameters in a "property-name:property-value" mapping.
	 */
	std::map<std::string, std::string> getParamParsed() const {
		std::map<std::string, std::string> properties;

		_debug("Running tokenizer on \"%s\"", params.c_str());

		ost::StringTokenizer paramsTokenizer(params.c_str(), ";", false, true);
		ost::StringTokenizer::iterator it;
		for (it = paramsTokenizer.begin(); it != paramsTokenizer.end(); ++it) {
			std::string param(*it);
			_debug("Found MIME parameter \"%s\"", param.c_str());
			size_t pos = param.find("=");
			std::string value = param.substr(pos+1); // FIXME Too naive !
			std::string property = param.substr(0, pos); // FIXME Too naive !

			properties.insert(std::pair<std::string, std::string>(property, value));
		}

		return properties;
	}

    /**
     * @return The static or dynamic payload type corresponding to some a=rtpmap line.
     */
    std::string getPayloadType() const
    {
        return payloadType;
    }

private:
	std::string payloadType;
	std::string params;
};

}

#endif
