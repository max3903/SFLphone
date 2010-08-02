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

#ifndef __SFL_VIDEO_CODEC_H__
#define __SFL_VIDEO_CODEC_H__

#include "Codec.h"
#include "video/VideoPlugin.h"
#include "encoder/VideoEncoder.h"
#include "decoder/VideoDecoder.h"
#include "video/source/VideoInputSource.h"

#include "util/memory/Buffer.h"

namespace sfl {

/**
 * A VideoCodec is a pair that consists of a video encoder and a video decoder.
 * A VideoCodec object offers a complete set of properties that corresponds to the
 * set of parameters defined in its RFC (SDP params).
 *
 * Hence, encoding and decoding params must be the same for the two elements.
 *
 * A lot of the public methods made available below are just meant to hide delegates.
 */

class VideoCodec : public virtual Codec, public VideoPlugin {
public:
	VideoCodec() {};
	virtual ~VideoCodec() {};

	virtual void encode(const VideoFrame* frame) throw(VideoEncodingException) = 0;

	virtual void decode(Buffer<uint8>& data) throw (VideoDecodingException) = 0;

	virtual void setEncoderVideoSource(VideoInputSource* source) = 0;

	virtual void addVideoFrameEncodedObserver(VideoFrameEncodedObserver& observer) = 0;

	virtual void addVideoFrameDecodedObserver(VideoFrameDecodedObserver& observer) = 0;
};

}

#endif
