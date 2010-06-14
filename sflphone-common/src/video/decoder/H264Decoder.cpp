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
#include <libavcodec/avcodec.h>

namespace sfl {

H264Decoder::H264Decoder(const FrameFormat& format) :
	VideoDecoder(format){
	// Init fields.
	init();

	// Allocate FFMPEG context
	context = avcodec_alloc_context();
	context->width = format.getWidth();
	;
	context->height = format.getHeight();
	;
	context->idct_algo = FF_IDCT_AUTO;
	//context->dsp_mask = (FF_MM_MMX|FF_MM_MMXEXT|FF_MM_SSE|FF_MM_SSE2);

	// Allocate FFMPEG frame
	outputFrame = avcodec_alloc_frame();

	// Open the decoder
	decoder = avcodec_find_decoder(CODEC_ID_H264);
	if (decoder == NULL) {
		// Throw MissingCodecException
	}

	if (avcodec_open(context, decoder) < 0) {
		// throw exception
	}
}

H264Decoder::H264Decoder(const H264Decoder& other) : VideoDecoder(other.getFrameFormat())
{ init(); }

void H264Decoder::init()
{
	context = NULL;
	outputFrame = NULL;
	decoder = NULL;
}

void H264Decoder::decode(const uint8_t* frame, size_t size) {
	int nbytes;
	int pictureFinished = 0;

	nbytes = avcodec_decode_video(context, outputFrame, &pictureFinished,
			frame, size);

	if (pictureFinished) {
		return; // FIXME device on returning nbytes or not.
	}
}

VideoDecoder* H264Decoder::clone() {
	return new H264Decoder(*this);
}

}
