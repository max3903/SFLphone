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
#ifndef __SFL_VIDEO_DEPAYLOADER_H__
#define __SFL_VIDEO_DEPAYLOADER_H__

#include "video/decoder/VideoDecoder.h"
#include "util/AbstractObservable.h"
#include "util/Buffer.h"
#include <string>

/**
 * Forward declaration.
 */
namespace ost {
class AppDataUnit;
}

namespace sfl {
/**
 * This exception is thrown when a video frame cannot be decoded.
 */
class VideoDepayloadingException: public std::runtime_error {
public:
	VideoDepayloadingException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * Asynchronous notification on frame depayloading.
 */
class VideoFrameDepayloadedObserver: public Observer {
public:
	/**
	 * @param frame The new frame that was depayloaded and decoded.
	 */
	virtual void onNewDepayloadedFrame(Buffer<uint8_t>& buffer) = 0;
};

/**
 * Abstract interface for video depayloaders.
 * We decided not to use the perfectly good depayloaders in Gstreamer to favor reuse
 * of our CCRTP code base. However, we might want to switch completely at some point.
 */
class VideoDepayloader: public AbstractObservable<Buffer<uint8_t>&,
		VideoFrameDepayloadedObserver> {
public:
	/**
	 * Default constructor. With this form, no decoder is specified to receive the depayloaded data, but
	 * yet one can register observers.
	 */
	VideoDepayloader() :
		decoder(NULL) {
	}

	/**
	 * @param decoder A decoder that should receive the data after processing. This is optional.
	 */
	VideoDepayloader(VideoDecoder& decoder) {
		this->decoder = &decoder;
	}

	virtual ~VideoDepayloader() {};

	/**
	 * @param decoder A decoder that should receive the data after processing
	 * @postcondition Further calls to process() will send the data to the decoder.
	 */
	void setDecoder(VideoDecoder& decoder) {
		this->decoder = &decoder;
	}

	/**
	 * @param sdp The unparsed SDP description.
	 */
	virtual void parseSdp(const std::string& sdp) = 0;

	/**
	 * @param adu The rtp packet.
	 */
	virtual void process(const ost::AppDataUnit* adu)
			throw (VideoDepayloadingException) = 0;

	/**
	 * @param adu The rtp packet.
	 * @see sfl#VideoDepayloader#process
	 */
	inline void operator()(const ost::AppDataUnit* adu)
			throw (VideoDepayloadingException) {
		process(adu);
	}

protected:
	/**
	 * @Override
	 */
	void notify(VideoFrameDepayloadedObserver* observer, Buffer<uint8_t>& data) {
		observer->onNewDepayloadedFrame(data);
	}

	void post(Buffer<uint8_t>& data) {
		if (decoder != NULL) {
			decoder->decode(data);
		}
		notifyAll(data);
	}
	;

private:
	VideoDecoder* decoder;
};
}

#endif
