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

#ifndef __SFL_VIDEO_DECODER_H__
#define __SFL_VIDEO_DECODER_H__

#include "video/VideoFrame.h"
#include "video/FrameFormat.h"
#include "video/VideoExceptions.h"

#include "util/AbstractObservable.h"
#include "util/Dimension.h"
#include "util/Buffer.h"

#include <stdexcept>

namespace sfl {

/**
 * The client that wants to get access to decoded frame must
 * implement this interface.
 */
class VideoFrameDecodedObserver : public Observer {
public:
	/**
	 * @param frame The new frame that was decoded.
	 */
	virtual void onNewFrameDecoded(Buffer<uint8_t>& data) = 0;
};

/**
 * Abstract base class for every video encoder.
 */
class VideoDecoder : public AbstractObservable<Buffer<uint8_t>&, VideoFrameDecodedObserver>{
public:
	/**
	 * Construct a video decoder with no rescaling nor colorspace transformation.
	 * @see sfl#VideoDecoder#getDimension
	 * @see sfl#VideoDecoder#getFrameFormat
	 * #see sfl#VideoDecoder#setOutputFormat
	 */
	VideoDecoder() throw(VideoDecodingException, MissingPluginException) {};

	/**
	 * @param decodingFormat The output format.
	 * @throw VideoDecodingException if an error occurs while opening the video decoder.
	 */
	VideoDecoder(const VideoFormat& decodingFormat) throw(VideoDecodingException, MissingPluginException) {};

	virtual ~VideoDecoder() {};

	/**
	 * @param buffer A buffer containing the depayloaded data.
	 * @throw VideoDecodingException if the frame cannot be decoded.
	 */
	virtual void decode(Buffer<uint8>& data)
			throw (VideoDecodingException) = 0;

	/**
	 * @param decodingFormat The desired output format.
	 * @postcondition The new output video format will be applied immediately.
	 * @throw VideoDecodingException if the output format can't be applied.
	 */
	virtual void setOutputFormat(const VideoFormat& decodingFormat) throw (VideoDecodingException) = 0;

	/**
	 * @return decodingFormat The specified output format.
	 */
	virtual VideoFormat getOutputFormat() const = 0;

	/**
	 * @return The output Dimension of the current video stream.
	 */
	virtual Dimension getDimension() const = 0;

	/**
	 * @return A string containing the FOURCC representation of the frame format that is being used.
	 */
	virtual std::string getFourcc() const = 0;

protected:
	/**
	 * Simple dispatch for the VideoFrameDecodedObserver type.
	 * @Override
	 */
	void notify(VideoFrameDecodedObserver* observer, Buffer<uint8_t>& data) {
		observer->onNewFrameDecoded(data);
	}
};

}

#endif /*DECODER_H_*/
