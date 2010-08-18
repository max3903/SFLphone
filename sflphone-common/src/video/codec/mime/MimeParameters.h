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

#ifndef __SFL_MIME_PARAMETERS_H__
#define __SFL_MIME_PARAMETERS_H__

/**
 * Start a new payload format definition.
 */
#define MIME_PAYLOAD_FORMAT_DEFINITION( mime, subtype, payloadType, clock ) \
	class MimeParameters##subtype: public virtual MimeParameters \
	{ \
		public: \
        inline virtual ~MimeParameters##subtype() {} \
        std::string getMimeType() const { \
            return std::string( mime ); \
        } \
        std::string getMimeSubtype() const { \
            return std::string( #subtype ); \
        } \
        uint8 getPayloadType() const { \
            return payloadType; \
        } \
        uint32 getClockRate() const { \
            return clock; \
        } \
	    MimeParameters##subtype() {

/**
 * An alias for MIME_PARAMETER_OPTIONAL
 */
#define MIME_PARAMETER(name)	\
	addOptionalParameter( name );

/**
 * Defines an optional parameter.
 */
#define MIME_PARAMETER_OPTIONAL(name) \
	addOptionalParameter( name );

/**
 * Defines a required parameter. The value of this parameter
 * should be obtained when sending the initial SDP offer.
 */
#define MIME_PARAMETER_REQUIRED(name) \
	addRequiredParameter( name )

/**
 * End a payload format definition.
 */
#define MIME_PAYLOAD_FORMAT_DEFINITION_END() \
        } \
	}; \

#include <ccrtp/rtp.h>
#include <vector>
#include <map>

namespace sfl
{

/**
 * Interface for exposing MIME parameters in SDP offer/answer model.
 */
class MimeParameters
{
    public:
        /**
         * @return The mimesubtype for this codec. Eg. : "video"
         */
        virtual std::string getMimeType() const = 0;

        /**
         * @return The mimesubtype for this codec. Eg. : "theora"
         */
        virtual std::string getMimeSubtype() const = 0;

        /**
         * @return payload type numeric identifier.
         */
        virtual uint8 getPayloadType() const = 0;

        /**
         * @return RTP clock rate in Hz.
         */
        virtual uint32 getClockRate() const = 0;

        /**
         * @param name The name that identifies the MIME parameter.
         * @param value The value this parameter should have.
         */
        virtual void setParameter (const std::string& name, const std::string& value) = 0;

        /**
         * @param name The name that identifies the MIME parameter.
         * @return The value that is set for this parameter.
         */
        virtual std::string getParameter (const std::string& name) = 0;

        /**
         * @return A string containing the codec specific parameters, formatted by default as :
         * "PARAM_LIST : PARAM_NAME = VALUE SEMI_COLON PARAM_LIST | PARAM_END
         *  PARAM_END : empty"
         */
        virtual std::string getParametersFormatted() {
        	// TODO Instead of putting everything into the same vector,
        	// enforce the required vs optional aspect. Unfilled required params. should
        	// result in exception throwing.
        	std::vector<std::string> paramList = requiredParameters;
        	std::copy(optionalParameters.begin(), optionalParameters.end(), std::back_inserter(paramList));

        	std::string output("");
        	std::vector<std::string>::iterator it;
        	int numberParamsAppended = 0;
        	for (it = paramList.begin(); it != paramList.end(); it++) {

        		std::string name = *it;
        		std::string value = getParameter(name);
        		if (value != "") {
        	   		if (numberParamsAppended != 0) {
        	        	output.append("; ");
        	        }

        	   		output.append(name + "=" + value);

        	   		numberParamsAppended += 1;
        		}
        	}

        	return output;
        }

    protected:
        /**
         * @param name The name for the required parameter to add.
         */
        void addRequiredParameter(const std::string& name) {
        	requiredParameters.push_back(name);
        }

        /**
         * @param name The name for the optional parameter to add.
         */
        void addOptionalParameter(const std::string& name) {
        	optionalParameters.push_back(name);
        }

        std::vector<std::string> requiredParameters;
        std::vector<std::string> optionalParameters;
};

}
#endif
