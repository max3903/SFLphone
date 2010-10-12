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
#ifndef __SFL_VIDEO_ENCODER_H__
#define __SFL_VIDEO_ENCODER_H__

#include "video/VideoPlugin.h"
#include "video/VideoExceptions.h"
#include "video/source/VideoFrame.h"
#include "video/source/VideoFormat.h"

#include "video/codec/mime/MimeParameters.h"

#include "util/pattern/AbstractObservable.h"
#include "util/memory/Buffer.h"

namespace sfl {

/**
 * The client that wants to get access to encoded frame must
 * implement this interface.
 */
class VideoFrameEncodedObserver: public Observer {
public:
	/**
	 * @param frame The new frame, or NAL unit, that was encoded. The implementer will
	 * then use this data and send it over the network.
	 */
	virtual void onNewFrameEncoded(std::pair<uint32, Buffer<uint8> >& data) = 0;
};

/**
 * Abstract base class for every video encoder.
 */
class VideoEncoder: public virtual MimeParameters,
		public VideoPlugin,
		public AbstractObservable<std::pair<uint32, Buffer<uint8> >&,
				VideoFrameEncodedObserver> {
public:
	/**
	 * Constructor for VideoEncoder type, taking no video format parameter.
	 * @see sfl#VideoEncoder#setSource;
	 */
	VideoEncoder() throw (VideoEncodingException, MissingPluginException) {}

	/**
	 * @param format The video format in which the frames are expected to be received.
	 * @see sfl#VideoEncoder#setSource;
	 */
	VideoEncoder(const VideoFormat& format) throw (VideoEncodingException, MissingPluginException) :
		format(format) {}

	virtual inline ~VideoEncoder() {};

	/**
	 * @param format The video format in which the frames are expected to be received.
	 */
	void setVideoInputFormat(const VideoFormat& format) {
		this->format = format;
	}

	/**
	 * @return The video format in which the frames are expected to be received.
	 */
	VideoFormat getVideoInputFormat() {
		return format;
	}

	/**
	 * Concrete encoders must provide an implementation of this method in order to
	 * encode the given frame.
	 * @param frame The video frame to be encoded, then sent (but not in this object) through the
	 * RTP stack.
	 */
	virtual void
			encode(const VideoFrame* frame) throw (VideoEncodingException) = 0;

	typedef int IsDerivedFromVideoEncoder;

	/**
	 * @Override
	 */
	virtual void activate() {} ;

	/**
	 * @Override
	 */
	virtual void deactivate() {};

private:
	/**
	 * Simple dispatch for the VideoFrameDecodedObserver type.
	 * @Override
	 */
	void notify(VideoFrameEncodedObserver* observer, std::pair<uint32, Buffer<
			uint8> >& data) {
		observer->onNewFrameEncoded(data);
	}

	// FIXME Should not need to do that.
	void notify(VideoFrameEncodedObserver* observer, const std::string& name,
			std::pair<uint32, Buffer<uint8> >& data) {
	}
	;

	VideoFormat format;
};

}

#endif
