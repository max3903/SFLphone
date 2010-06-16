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
#ifndef __SFL_VIDEO_RTP_SESSION_H__
#define __SFL_VIDEO_RTP_SESSION_H__

#include <queue>
#include <ccrtp/rtp.h>

#include "util/Observer.h"
#include "util/AbstractObservable.h"

namespace sfl {

/**
 * Forward declarations.
 */
class VideoDecoder;
template <class T>
class QueuedBuffer;

/**
 * Asynchronous notification on frame decoding.
 */
class VideoFrameDecodedObserver : public Observer {
public:
	/**
	 * @param frame The new frame that was depayloaded and decoded.
	 */
	void onNewFrameDecoded(uint8_t* frame);
};

/**
 * Interface for VideoRtpSession types.
 */
class VideoRtpSession : public ost::RTPSession, public AbstractObservable<uint8_t*, VideoFrameDecodedObserver> {
public:
	/**
	 * @param mutiCastAddress A multicast address.
	 * @param port The port to work on.
	 */
	VideoRtpSession(ost::InetMcastAddress& multiCastAddress, ost::tpport_t port);

	/**
	 * @param mutiCastAddress An inet address.
	 * @param port The port to work on.
	 */
	VideoRtpSession(ost::InetHostAddress& ia, ost::tpport_t port);

	~VideoRtpSession();

	/**
	 * @param decoder The video decoder to use.
	 * @postcondition The object will be copied (avoiding to leak control outside of class).
	 */
	void setDecoder(VideoDecoder& decoder);

	/**
	 * This methods starts listening for rtp packets, calling the
	 * observer when a frame is fully available.
	 */
	void listen();

	static const int SCHEDULING_TIMEOUT = 10000;
	static const int EXPIRE_TIMEOUT = 1000000;

protected:
	/**
	 * @Override
	 */
	void notify(VideoFrameDecodedObserver* observer, uint8_t* data);

private:
	/**
	 * Used in constructor.
	 */
	void init();

	/**
	 * Enqueue slice of data.
	 * @param The whole adu object. Will be freed when flushed.
	 */
	void queue(const ost::AppDataUnit* adu);

	/**
	 * Encode queued data. Note that this could be made asynchronous. Flushing the data would be non-blocking, spawning a separate
	 * thread for decoding.
	 */
	void flush();

	VideoDecoder* decoder;
	QueuedBuffer<uint8_t>* workingBuffer;
	std::queue<const ost::AppDataUnit*> dataQueue;

	static const int WORKING_BUFFER_SIZE = 100000;
};
}
#endif
