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

void VideoRtpSession::registerCodec(const std::string& mime,
		VideoEncoder& encoder, VideoDecoder& decoder) {
	registerEncoder(mime, encoder);
	registerDecoder(mime, decoder);
}

void VideoRtpSession::unregisterCodec(const std::string& mime) {
	unregisterEncoder(mime);
	unregisterDecoder(mime);
}

void VideoRtpSession::registerDecoder(const std::string& mime,
		VideoDecoder& decoder) {
	decoderTable.insert(std::pair<std::string, VideoDecoder*>(mime, &decoder));

	_debug("Decoder %s registered", mime.c_str());
}

void VideoRtpSession::unregisterDecoder(const std::string mime) {
	DecoderTableIterator it = decoderTable.find(mime);
	decoderTable.erase(it);

	_debug("Decoder %s unregistered", mime.c_str());
}

void VideoRtpSession::registerEncoder(const std::string& mime,
		VideoEncoder& encoder) {
	encoderTable.insert(std::pair<std::string, VideoEncoder*>(mime, &encoder));

	_debug("Encoder %s registered", mime.c_str());
}

void VideoRtpSession::unregisterEncoder(const std::string mime) {
	EncoderTableIterator it = encoderTable.find(mime);
	encoderTable.erase(it);

	_debug("Encoder %s unregistered", mime.c_str());
}

/**
 * Take the raw, unencrypted, RTP packets and send to the decoder.
 */
VideoDecoder* VideoRtpSession::getDecoder(const std::string& mime)
		throw (MissingPluginException) {
	// Find a decoder for the mimetype
	DecoderTableIterator it = decoderTable.find(mime);
	if (it == decoderTable.end()) {
		std::string msg = std::string("Decoder \"") + mime + std::string(
				"\"is not a registered decoder.");
		throw MissingPluginException(msg);
	}

	return (*it).second;
}

VideoEncoder* VideoRtpSession::getEncoder(const std::string& mime)
		throw (MissingPluginException) {
	// Find a decoder for the mimetype
	EncoderTableIterator it = encoderTable.find(mime);
	if (it == encoderTable.end()) {
		std::string msg = std::string("Encoder \"") + mime + std::string(
				"\"is not a registered encoder.");
		throw MissingPluginException(msg);
	}

	return (*it).second;
}

bool VideoRtpSession::onRTPPacketRecv(ost::IncomingRTPPkt& packet) {
	// Make sure that a decoder has been configured
	if (decoder != NULL) {
		unsigned char* rawData =
				const_cast<unsigned char*> (packet.getRawPacket()); // FIXME Dangerous, but needed.
		uint32 rawSize = packet.getRawPacketSize();

		ManagedBuffer<uint8> buffer(rawData, rawSize);
		decoder->decode(buffer);
	}

	// TODO decide if an exception should be thrown.
	// This special case means that we decided to add a destination,
	// that the remote peer decided to send some data, but yet, no codec was configured prior to that.
	// It would happen if configureFromSdp is not executed yet, or is being executed. Thus the problem
	// would be between init() and configureFromSdp(). Maybe startRunning() should be called elsewhere.

	return true;
}

void VideoRtpSession::replaceCodec(VideoEncoder* encoderNew,
		VideoDecoder* decoderNew) throw (MissingPluginException) {
	// Deactivate the previous codec
	if (encoder != NULL) {
		encoder->deactivate();
	}
	//decoder->deactivate();

	// Set the current codec
	encoder = encoderNew;
	decoder = decoderNew;

	// Register as observer from the new one
	encoder->addObserver(encoderObserver);
	// decoder->addObserver(this);

	encoder->activate();
	// decoder->activate();

}

void VideoRtpSession::configureFromSdp(const RtpMap& rtpmap, const Fmtp& fmtp)
		throw (MissingPluginException) {
	_info("Configuring current video codec from SDP");

	// Replace codec
	replaceCodec(getEncoder(rtpmap.getCodecName()), getDecoder(
			rtpmap.getCodecName()));

	// Configure additional information.
	clockRate = rtpmap.getClockRate();
	payloadType = rtpmap.getPayloadType();

	// Set the new payload type
	setPayloadFormat(ost::DynamicPayloadFormat(payloadType, clockRate));
}

void VideoRtpSession::listen() {
	// TODO make this happen in a separate thread.
//	while (!testCancel()) {
//		const ost::AppDataUnit* adu;
//		while ((adu = getData(getFirstTimestamp()))) {
//		}
//		yield();
//	}
}

void VideoRtpSession::init() {
	// Fixed encoder for any video encoder type
	encoderObserver = new EncoderObserver(this);
	encoder = NULL;
	decoder = NULL;

	// The default scheduling timeout to use when no data packets are waiting to be sent.
	setSchedulingTimeout(SCHEDULING_TIMEOUT);

	// Set the "expired" timer for expiring packets pending in the send
	// queue which have gone unsent and are already "too late" to be sent now.
	setExpireTimeout(EXPIRE_TIMEOUT);

	// Set the SDES information
	// TODO Set the other items
	ost::defaultApplication().setSDESItem(ost::SDESItemTypeTOOL,
			"Video Endpoint");

	// Start the socket thread
	startRunning();
}

void VideoRtpSession::notify(VideoFrameDecodedObserver* observer, ManagedBuffer<
		uint8_t>& data) {
	observer->onNewFrameDecoded(data);
}

VideoRtpSession::VideoRtpSession(ost::InetMcastAddress& ima, ost::tpport_t port) :
	ost::RTPSession(ima, port) {
	init();
}

VideoRtpSession::VideoRtpSession(ost::InetHostAddress& ia, ost::tpport_t port) :
	ost::RTPSession(ia, port) {
	init();
}

VideoRtpSession::~VideoRtpSession() {
	encoder->removeObserver(encoderObserver);
	delete encoderObserver;
	_warn("VideoRtpSession has terminated");
}

}
