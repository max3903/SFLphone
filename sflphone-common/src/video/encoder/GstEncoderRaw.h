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

#ifndef __SFL_GST_ENCODER_RAW_H__
#define __SFL_GST_ENCODER_RAW_H__

#include "GstEncoder.h"

#include <string>

namespace sfl {

class GstEncoderRaw: public GstEncoder {
public:
	GstEncoderRaw(VideoInputSource& source) throw (VideoDecodingException,
			MissingPluginException);

	GstEncoderRaw(VideoInputSource& source, unsigned maxFrameQueued)
			throw (VideoDecodingException, MissingPluginException);

	void setChromaSubsamplingFormat(const std::string& value);

	void setWidth(const std::string& value);

	void setHeight(const std::string& value);

	void setDepth(const std::string& value);

	void setColorimetry(const std::string& value);

	/**
	 * @Override
	 */
	std::string getCodecName();

protected:

	/**
	 * @Override
	 */
	GstElement* getTail();

	/**
	 * @Override
	 */
	void setProperty(int index, const std::string& value);

	/**
	 * @Override
	 */
	void buildEncodingFilter(Pipeline& pipeline, GstElement* previous)
			throw (VideoDecodingException, MissingPluginException);

private:

	void init() throw (VideoDecodingException, MissingPluginException);

	GstElement* previous;
};

}

#endif
