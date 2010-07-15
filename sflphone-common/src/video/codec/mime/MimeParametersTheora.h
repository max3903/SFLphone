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

#ifndef __SFL_MIME_PARAMETERS_THEORA_H__
#define __SFL_MIME_PARAMETERS_THEORA_H__

#include "MimeParameters.h"

namespace sfl {
/**
 *
 * MIME media type name: video
 *
 * MIME subtype: theora
 */
class MimeParametersTheora : public virtual MimeParameters {
public:
	/**
	 * @Override
	 */
	std::string getMimeType()  { return "video"; }

	/**
	 * @Override
	 */
	std::string getMimeSubtype() { return "theora"; }

	/**
	 * @Override
	 */
	const ost::PayloadFormat& getPayloadFormat() {
		static ost::DynamicPayloadFormat format((ost::PayloadType) 96, 90000);
		return format;
	}
	/**
	 * Determines the chroma subsampling format.
	 */
	void setSampling(const std::string& value) { setParameter("sampling", value); }
	std::string getSampling() { return getParameter("sampling"); }

	/**
	 * Determines the number of pixels per line.  This is an
     * integer between 1 and 1048561 and MUST be in multiples of 16.
	 */
	void setWidth(const std::string& value) { setParameter("width", value); }
	std::string getWidth() { return getParameter("width"); }

	/**
	 * Determines the number of lines per frame encoded.  This is
     * an integer between 1 and 1048561 and MUST be in multiples of 16.
	 */
	void setHeight(const std::string& value) { setParameter("height", value); }
	std::string getHeight() { return getParameter("height"); }

	/**
	 * Indicates the delivery methods in use, the
     * possible values are: inline, in_band, out_band/specific_name
     * Where "specific_name" is the name of the out of band delivery method.
	 */
	void setDeliveryMethod(const std::string& value) { setParameter("delivery-method", value); }
	std::string getDeliveryMethod() { return getParameter("delivery-method"); }

	/**
	 * The base16 [11] (hexadecimal) representation of the Packed Headers (Section 3.2.1).
	 */
	void setConfiguration(const std::string& value) { setParameter("configuration", value); }
	std::string getConfiguration() { return getParameter("configuration"); }

	/**
	 * The URI of the configuration headers in case of
     * out of band transmission.  In the form of
     * "protocol://path/to/resource/".  Depending on the specific
     * method the single ident packets could be retrieved by their
     * number or aggregated in a single stream, aggregates MAY be
     * compressed using gzip [12] or bzip2 [14] and an sha1 [13]
     * checksum MAY be provided in the form of
     * "protocol://path/to/resource/aggregated.bz2!sha1hash"
	 */
	void setConfigurationUri(const std::string& value) { setParameter("configuration-uri", value); }
	std::string getConfigurationUri() { return getParameter("configuration-uri"); }

protected:
	MimeParametersTheora() {};
	inline virtual ~MimeParametersTheora() {}
};
}

#endif
