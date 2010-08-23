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
#include "VideoRtpSessionSimple.h"
#include "video/codec/VideoCodecNull.h"

namespace sfl
{

void VideoRtpSessionSimple::setActiveCodec(ost::PayloadType pt)
{
	SessionCodecIterator it = sessionsCodecs.find(pt);
	if (it == sessionsCodecs.end()) {
		// TODO throw MissingPluginException
		_error("No video codec could be found with payload type %d (%s:%d)", __FILE__, __LINE__);
	}

	sfl::VideoCodec* codec = (*it).second;

    activeCodec->deactivate();
    activeCodec = codec;

    // Set the payload format in ccRTP from the information contained in the VideoCodec.
    ost::DynamicPayloadFormat format (activeCodec->getPayloadType(), activeCodec->getClockRate());
    setPayloadFormat (format);

    // Register as a VideoFrameEncodedObserver so that encoded frames produced in the VideoCodec,
    // starting from the VideoSource, get dispatched internally and sent over the network immediately.
    activeCodec->addVideoFrameEncodedObserver ( (*encoderObserver));

    activeCodec->activate();
}

void VideoRtpSessionSimple::addSessionCodec(ost::PayloadType payloadType, const sfl::VideoCodec* codec)
{
	VideoCodec* videoCodec = dynamic_cast<VideoCodec*>(codec->clone());
	sessionsCodecs.insert (SessionCodecEntry(payloadType, videoCodec));
	_info ("Codec \"%s\" (payload number %d) added for this session", videoCodec->getMimeSubtype().c_str(), payloadType);

    // If this is the first codec that we add, set as default.
    if (sessionsCodecs.size() == 1) {
        setActiveCodec(payloadType);
    }
}

void VideoRtpSessionSimple::setVideoInputFormat (const VideoFormat& format)
{
	currentVideoFormat = format;
}

void VideoRtpSessionSimple::start()
{
    // Start the socket thread
    startRunning();
}

void VideoRtpSessionSimple::sendPayloaded(const VideoFrame* frame)
{
	activeCodec->encode(frame);
}

void VideoRtpSessionSimple::init()
{
    // Fixed encoder for any video encoder type
    encoderObserver = new EncoderObserver (this);
    activeCodec = new VideoCodecNull();

    // The default scheduling timeout to use when no data packets are waiting to be sent.
    setSchedulingTimeout (SCHEDULING_TIMEOUT);

    // Set the "expired" timer for expiring packets pending in the send
    // queue which have gone unsent and are already "too late" to be sent now.
    setExpireTimeout (EXPIRE_TIMEOUT);

    // Set the SDES information
    // TODO Set the other items
    ost::defaultApplication().setSDESItem (ost::SDESItemTypeTOOL,
                                           "SFLPhone Video Endpoint");

    _debug ("VideoRtpSessionSimple initialized.");
}

bool VideoRtpSessionSimple::onRTPPacketRecv (ost::IncomingRTPPkt& packet)
{
    // Make sure that a decoder has been configured for this payload type
    if (getCurrentPayloadType() != packet.getPayloadType()) {
        _warn ("New payload type detected during RTP session. Switching to new codec with payload type %d", packet.getPayloadType());
        //setActiveCodec(packet.getPayloadType());
    }

    // Extract RTP packet
    unsigned char* rawData = const_cast<unsigned char*> (packet.getRawPacket()); // FIXME Dangerous, but needed.
    uint32 rawSize = packet.getRawPacketSize();

    // Send to the depayloader/decoder
    Buffer<uint8> buffer (rawData, rawSize); // TODO Should not be managed
    activeCodec->decode (buffer);

    return true;
}

VideoRtpSessionSimple::VideoRtpSessionSimple (ost::InetMcastAddress& ima,
        ost::tpport_t port) :
        ost::RTPSession (ima, port)
{
    init();
}

VideoRtpSessionSimple::VideoRtpSessionSimple (ost::InetHostAddress& ia,
        ost::tpport_t port) :
        ost::RTPSession (ia, port)
{
    init();
}

VideoRtpSessionSimple::~VideoRtpSessionSimple()
{
    // TODO codec->removeObserver(encoderObserver);
	// TODO free the codec list.
    _warn ("VideoRtpSessionSimple has terminated");
}

}
