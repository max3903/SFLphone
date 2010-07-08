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

#include "video/decoder/NullDecoder.h"
#include "video/encoder/NullEncoder.h"

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

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

VideoDecoder* VideoRtpSession::getDecoder(const std::string& mime)
		throw (MissingPluginException) {
	// Find a decoder for the mimetype
	DecoderTableIterator it = decoderTable.find(mime);
	if (it == decoderTable.end()) {
		std::string msg = std::string("Decoder \"") + mime + std::string(
				"\" is not a registered decoder.");
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
				"\" is not a registered encoder.");
		throw MissingPluginException(msg);
	}

	return (*it).second;
}

void VideoRtpSession::setEncoder(VideoEncoder* enc) {
	encoder->deactivate();

	encoder = enc;

	encoder->addObserver(encoderObserver);

	encoder->activate();
}

void VideoRtpSession::setDecoder(VideoDecoder* dec) {
	decoder->deactivate();

	decoder = dec;

	decoder->activate();
}

void VideoRtpSession::setCodec(ost::PayloadType pt) throw(MissingPluginException) {
	CodecIterator it = sessionCodecMap.find(pt);
	if (it == sessionCodecMap.end()) {
		std::ostringstream msg;
		msg << "No plugin found for payload type ";
		msg << (int) pt;
		throw MissingPluginException(msg.str());
	}

	CodecConfiguration config = (*it).second;

	// Replace with the new encoder and decoder
	VideoEncoder* enc = config.getEncoder();
	setEncoder(enc);
	VideoDecoder* dec = config.getDecoder();
	setDecoder(dec);

	setActivePayloadType(pt);

	setPayloadFormat(ost::DynamicPayloadFormat(pt,
			config.getRtpmap().getClockRate()));

	// Configure the codec with the options obtained from SDP.
	std::map<std::string, std::string> props = config.getRtpmap().getParamParsed();
	std::map<std::string, std::string>::iterator itProps;
	for (itProps = props.begin(); itProps != props.end(); itProps++) {
		enc->setProperty((*itProps).first /* Prop. name */, (*itProps).second /* Prop. value */);
		dec->setProperty((*itProps).first /* Prop. name */, (*itProps).second /* Prop. value */);
	}
}

void VideoRtpSession::addSessionCodec(const RtpMap& rtpmap, const Fmtp& fmtp)
		throw (MissingPluginException) {
	_info("Configuring supported Codecs for the session from SDP");

	// Will throw at that point if an encoder/decoder can't be found.
	VideoEncoder* encoder;
	try {
		encoder = getEncoder(rtpmap.getCodecName());
	} catch (MissingPluginException e) {
		_error("The given encoder in SDP is not supported. %s", e.what());
		encoder = new NullEncoder(); // TODO use a singleton.
	};

	VideoDecoder* decoder;
	try {
		decoder = getDecoder(rtpmap.getCodecName());
	} catch (MissingPluginException e) {
		_error("The given decoder in SDP is not supported. %s", e.what());
		decoder = new NullDecoder();  // TODO use a singleton.
	};

	if (typeid(encoder) == typeid(NullEncoder) && typeid(decoder) == typeid(NullDecoder)) {
		throw MissingPluginException(std::string("Video codec \"") + rtpmap.getCodecName() + std::string("\" in SDP is not supported for both encoding and decoding."));
	}

	_debug("Inserting session codec with payload type %d", rtpmap.getPayloadType());
	sessionCodecMap.insert(CodecEntry(rtpmap.getPayloadType(),
			CodecConfiguration(encoder, decoder, rtpmap, fmtp)));

	// If this is the first codec registered, set it as default,
	// but yet support other formats if ever detected in the RTP stream.
	if (sessionCodecMap.size() == 1) {
		setCodec(rtpmap.getPayloadType());
	}
}

bool VideoRtpSession::onRTPPacketRecv(ost::IncomingRTPPkt& packet) {
	// Make sure that a decoder has been configured for this payload type
	if (getActivePayloadType() != packet.getPayloadType()) {
		_warn("New payload type detected during RTP session. Switching to new codec with payload type %d", packet.getPayloadType());
		setCodec(packet.getPayloadType());
	}

	// Extract RTP packet
	unsigned char* rawData = const_cast<unsigned char*> (packet.getRawPacket()); // FIXME Dangerous, but needed.
	uint32 rawSize = packet.getRawPacketSize();

	// Send to the depayloader/decoder
	assert(decoder);
	ManagedBuffer<uint8> buffer(rawData, rawSize);
	decoder->decode(buffer);

	return true;
}

void VideoRtpSession::init() {
	// Fixed encoder for any video encoder type
	encoderObserver = new EncoderObserver(this);

	encoder = new NullEncoder();
	decoder = new NullDecoder();

	// The default scheduling timeout to use when no data packets are waiting to be sent.
	setSchedulingTimeout(SCHEDULING_TIMEOUT);

	// Set the "expired" timer for expiring packets pending in the send
	// queue which have gone unsent and are already "too late" to be sent now.
	setExpireTimeout(EXPIRE_TIMEOUT);

	// Set the SDES information
	// TODO Set the other items
	ost::defaultApplication().setSDESItem(ost::SDESItemTypeTOOL,
			"Video Endpoint");
}

void VideoRtpSession::start() {
	// Start the socket thread
	startRunning();
}

void VideoRtpSession::notify(VideoFrameDecodedObserver* observer,
		ManagedBuffer<uint8_t>& data) {
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
