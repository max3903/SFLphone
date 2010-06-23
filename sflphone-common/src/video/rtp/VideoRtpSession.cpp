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
#include <stdint.h>
#include <stdlib.h>

namespace sfl {
VideoRtpSession::VideoRtpSession(ost::InetMcastAddress& ima, ost::tpport_t port)
{
	session = new ost::RTPSession(ima, port);
	init();
}

VideoRtpSession::VideoRtpSession(ost::InetHostAddress& ia, ost::tpport_t port)
{
	session = new ost::RTPSession(ia, port);
	init();
}

VideoRtpSession::~VideoRtpSession()
{
	terminate();
	_warn("VideoRtpSession has terminated");
}

void VideoRtpSession::init()
{
	setCancel(cancelDeferred);
}

void VideoRtpSession::configureFromSdp(const RtpMap& rtpmap, const Fmtp& fmtp) {
	// Find a decoder for the mimetype
	DecoderTableIterator it = decoderTable.find(rtpmap.getCodecName());
	if (it == decoderTable.end()) {
		std::string msg = std::string("Codec \"") + rtpmap.getCodecName()
				+ std::string("\"is not a registered codec.");
		throw MissingPluginException(msg);
	}
	depayloader = (*it).second;

	// Configure additional information.
	clockRate = rtpmap.getClockRate();
	payloadType = rtpmap.getPayloadType();
}

void VideoRtpSession::registerDecoder(const std::string& mime,
		VideoDepayloader& depayloader) {
	decoderTable.insert(std::pair<std::string, VideoDepayloader*>(mime,
			&depayloader));
}

void VideoRtpSession::registerDecoder(const std::string& mime,
		VideoDepayloader& depayloader, VideoDecoder& decoder) {
	depayloader.setDecoder(decoder);
	registerDecoder(mime, depayloader);
}

void VideoRtpSession::unregisterDecoder(const std::string mime) {
	DecoderTableIterator it = decoderTable.find(mime);
	decoderTable.erase(it);
}

/**
 * To send a video frame on a
 * rtp socket, we have to fragment it into multi packets. FFmpeg handles
 * this part by slicing each packet into 500 bytes chunks. The last packet
 * of a serie has its rtp markbit set to 1. We read and copy packets data
 * till we get the markbit. Once we have it, we pass the whole buffer to
 * the decoder.
 */
void VideoRtpSession::run() {
	session->setPayloadFormat(ost::DynamicPayloadFormat(payloadType, clockRate)); // FIXME this is specific to h264.
	session->setSchedulingTimeout(SCHEDULING_TIMEOUT);
	session->setExpireTimeout(EXPIRE_TIMEOUT);
	session->startRunning();

	while (!testCancel()) {
		const ost::AppDataUnit* adu;
		while ((adu = session->getData(session->getFirstTimestamp()))) {
			depayloader->process(adu);
		}

		yield();
	}
}

void VideoRtpSession::notify(VideoFrameDecodedObserver* observer, Buffer<uint8_t>& data) {
	observer->onNewFrameDecoded(data);
}

}
