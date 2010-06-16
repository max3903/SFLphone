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
#ifndef __SFL_H264_DECODER_H__
#define __SFL_H264_DECODER_H__

#include "VideoDecoder.h"
#include "video/FrameFormat.h"

// Forward declaration
class AVCodecContext;
class AVFrame;
class AVCodec;
struct SwsContext;

namespace sfl
{
class H264Decoder : public VideoDecoder
{
public:
	H264Decoder(const VideoFormat& encodingFormat, const VideoFormat& decodingFormat) throw(VideoDecodingException);
	~H264Decoder();

	/**
	 * Copy constructor.
	 */
	H264Decoder(const H264Decoder& decoder);

	/**
	 * @Override
	 */
	int decode(const uint8_t* frame, size_t size) throw(VideoDecodingException);

	/**
	 * @Override
	 */
	VideoDecoder* clone();

	/**
	 * @Override
	 */
	uint8_t** getRawData() const;

	/**
	 * @Override
	 */
	uint8_t* getConvertedPacked() throw(VideoDecodingException);

private:
	void init();

	AVCodecContext* context;
	AVFrame* decodedFrame;
	AVFrame* convertedFrame;
	AVCodec* decoder;

	struct SwsContext* convertContext;
};

}

#endif
