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

#include "H264Decoder.h"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

namespace sfl {

H264Decoder::H264Decoder(const VideoFormat& encodingFormat,
		const VideoFormat& decodingFormat) throw(VideoDecodingException):
	VideoDecoder(encodingFormat, decodingFormat) {
	// Init fields.
	init();

	// Allocate FFMPEG context
	context = avcodec_alloc_context();
	context->width = encodingFormat.getWidth();
	;
	context->height = encodingFormat.getHeight();
	;
	context->idct_algo = FF_IDCT_AUTO;
	//context->dsp_mask = (FF_MM_MMX|FF_MM_MMXEXT|FF_MM_SSE|FF_MM_SSE2);

	// Allocate FFMPEG frame for decoded frames
	decodedFrame = avcodec_alloc_frame();

	// Open the decoder
	decoder = avcodec_find_decoder(CODEC_ID_H264);
	if (decoder == NULL) {
		// Throw MissingCodecException
	}

	if (avcodec_open(context, decoder) < 0) {
		throw VideoDecodingException("Failed to open decoder");
	}

	// Allocate image convert context
	convertContext = sws_getContext(context->width,
			context->height, context->pix_fmt,
			decodingFormat.getWidth(), decodingFormat.getHeight(),
			(PixelFormat) decodingFormat.getColorSpace(), SWS_BICUBIC, NULL, NULL, NULL);

	if (convertContext == NULL) {
		// TODO Something.
	}

	// Allocate FFMPEG frame for converted frames
	convertedFrame = avcodec_alloc_frame();
}

H264Decoder::~H264Decoder() {
	av_free(decodedFrame);
	av_free(convertedFrame);
}

H264Decoder::H264Decoder(const H264Decoder& other) :
	VideoDecoder(other.getEncondingFormat(), other.getDecodingFormat()) {
	init();
}

void H264Decoder::init() {
	context = NULL;
	decodedFrame = NULL;
	decoder = NULL;
}

uint8_t* H264Decoder::getConvertedPacked() throw (VideoDecodingException) {

	int ret = sws_scale(convertContext, decodedFrame->data,
			decodedFrame->linesize, getDecodingFormat().getWidth(),
			getDecodingFormat().getHeight(), convertedFrame->data,
			convertedFrame->linesize);

	if (ret < 0) {
		throw VideoDecodingException("Failed to scale video.");
	}

	return convertedFrame->data[0];
}

uint8_t** H264Decoder::getRawData() const {
	return decodedFrame->data;
}

int H264Decoder::decode(const uint8_t* frame, size_t size)
		throw (VideoDecodingException) {
	int nbytes;
	int pictureFinished = 0;

	// TODO Add thread safety if needed when writing to outputFrame
	nbytes = avcodec_decode_video(context, decodedFrame, &pictureFinished,
			frame, size);

	if (pictureFinished) {
		return nbytes;
	} else {
		throw VideoDecodingException("Failed to decode to H264.");
	}
}

VideoDecoder* H264Decoder::clone() {
	return new H264Decoder(*this);
}

}
