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

#ifndef __SFL_DECODER_H__
#define __SFL_DECODER_H__

#include "video/VideoFrame.h"
#include "video/FrameFormat.h"

#include <stdexcept>

namespace sfl {
/**
 * This exception is thrown when a video frame cannot be decoded.
 */
class VideoDecodingException: public std::runtime_error {
public:
	VideoDecodingException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * This exception is thrown when a video codec cannot be found.
 * TODO This kind of exception is general and would benefit from being moved
 * outside of this file into a class-neutral and separated file. It could
 * also be made a supertype of MissingGstPluginException.
 */
class MissingPluginException: public std::runtime_error {
public:
	MissingPluginException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * Abstract base class for every video encoder.
 */
class VideoDecoder {
public:
	/**
	 * @param encodingFormat The source format.
	 * @param decodingFormat The output format.
	 * @throw VideoDecodingException if an error occurs while opening the video decoder.
	 */
	VideoDecoder(const VideoFormat& encodingFormat, const VideoFormat& decodingFormat) throw(VideoDecodingException, MissingPluginException);

	/**
	 * @return encodingFormat The source format.
	 */
	const VideoFormat& getEncondingFormat() const;

	/**
	 * @return decodingFormat The output format.
	 */
	const VideoFormat& getDecodingFormat() const;

	/**
	 * @param frame The video frame to encode.
	 * @param size The frame size.
	 * @param return The number of bytes decoded.
	 * @throw VideoDecodingException if the frame cannot be decoded.
	 */
	virtual int decode(const uint8_t* frame, size_t size)
			throw (VideoDecodingException) = 0;

	/**
	 * @return A pointer to a new copy of the current object.
	 */
	virtual VideoDecoder* clone() = 0;

	/**
	 * @return The raw data after decoding.
	 */
	virtual uint8_t** getRawData() const = 0;

	/**
	 * @precondition The requested output format must be of type "packed". You can ensure this by using the sfl::IsPlanar predicate.
	 * @return The raw data converted to the format specified in the constructor.
	 * @see sfl#VideoDecoder#VideoDecoder
	 * @see sfl#VideoDecoder#getRawData
	 * @see sfl#IsPlanar
	 */
	virtual uint8_t* getConvertedPacked() throw(VideoDecodingException) = 0;

private:
	VideoFormat encodedFormat;
	VideoFormat decodedFormat;
};

}

#endif /*DECODER_H_*/
