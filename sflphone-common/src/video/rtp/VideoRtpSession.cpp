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
#include "VideoRtpSession.h"
#include "video/decoder/VideoDecoder.h"
#include "util/QueuedBuffer.h"

#include <stdint.h>
#include <stdlib.h>

namespace sfl {
VideoRtpSession::VideoRtpSession(ost::InetMcastAddress& ima, ost::tpport_t port) :
	ost::RTPSession(ima, port) {
	init();
}

VideoRtpSession::VideoRtpSession(ost::InetHostAddress& ia, ost::tpport_t port) :
	ost::RTPSession(ia, port) {
	init();
}

VideoRtpSession::~VideoRtpSession()
{
	delete decoder;
	delete workingBuffer;
}

void VideoRtpSession::init() {
	decoder = NULL;
	workingBuffer = new QueuedBuffer<uint8_t>(WORKING_BUFFER_SIZE);
}

void VideoRtpSession::setDecoder(VideoDecoder& decoder) {
	this->decoder = decoder.clone();
}

void VideoRtpSession::queue(const ost::AppDataUnit* adu) {
	dataQueue.push(adu);
}

void VideoRtpSession::flush() {
	while(!dataQueue.empty()) {
		const ost::AppDataUnit* adu = dataQueue.front();
		workingBuffer->put(adu->getData(), adu->getSize()); // TODO Catch exception.
		dataQueue.pop();
		delete adu;
	}

	decoder->decode(workingBuffer->getBuffer(), workingBuffer->getSize());

	workingBuffer->reset();
	workingBuffer->clear();
}

/**
 * To send a video frame on a
 * rtp socket, we have to fragment it into multi packets. FFmpeg handles
 * this part by slicing each packet into 500 bytes chunks. The last packet
 * of a serie has its rtp markbit set to 1. We read and copy packets data
 * till we get the markbit. Once we have it, we pass the whole buffer to
 * the decoder,
 */
void VideoRtpSession::listen() {
	setSchedulingTimeout( SCHEDULING_TIMEOUT);
	setExpireTimeout( EXPIRE_TIMEOUT);

	setPayloadFormat(ost::DynamicPayloadFormat((ost::PayloadType) 98, 9000)); // FIXME this is specific to h264.

	startRunning();
	for (;;) {
		const ost::AppDataUnit* adu;
		while ((adu = getData(getFirstTimestamp()))) {
			if (adu->isMarked()) {
				// We should have a full frame by now
				flush();
			} else {
				queue(adu);
			}
		}

		yield();
	}
}
}
