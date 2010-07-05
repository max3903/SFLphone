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

#include <map>
#include <ccrtp/rtp.h>

#include "video/decoder/VideoDecoder.h"
#include "video/encoder/VideoEncoder.h"

#include "util/pattern/Observer.h"
#include "util/pattern/AbstractObservable.h"

#include "sip/sdp/Fmtp.h"
#include "sip/sdp/RtpMap.h"

namespace sfl {

/**
 * Forward declarations.
 */
class VideoDecoder;
template <class T>
class BufferBuilder;

/**
 * Interface for VideoRtpSession types.
 */
class VideoRtpSession : public AbstractObservable<Buffer<uint8_t>&, VideoFrameDecodedObserver>, public ost::RTPSession {
public:
	/**
	 * @param mutiCastAddress Local multicast network address
	 * @param port Local transport port (where incoming packets are expected)
	 */
	VideoRtpSession(ost::InetMcastAddress& multiCastAddress, ost::tpport_t port);

	/**
	 * @param unicastAddress Local unicast network address
	 * @param port Local transport port (where incoming packets are expected)
	 */
	VideoRtpSession(ost::InetHostAddress& unicastAddress, ost::tpport_t port);

	~VideoRtpSession();

	/**
	 * Change the configuration of the object based on the information contained in
	 * the SDP "a=rtpmap" and "a=fmtp" attributes.
	 * @param rtpmap The Rtmap object object holding the information of a "a=rtpmap" line of SDP.
	 * @param fmtp The Fmtp object holding the information of a "a=fmtp" line of SDP for some corresponding "a=rtpmap".
	 * @throw MissingPluginException if either the decoder, the encoder, or both are missing.
	 */
	void configureFromSdp(const RtpMap& rtpmap, const Fmtp& fmtp) throw(MissingPluginException);

	/**
	 * Register an encoder + decoder pair.
	 * @param mime The mime type corresponding to the codec.
	 * @param encoder The encoder object that will be fed with video frames and produce RTP packets stream.
	 * @param decoder The decoder object that will take the raw RTP packets and produce a video stream.
	 * @see sfl#VideoRtpSession#registerEncoder
	 * @see sfl#VideoRtpSession#registerDecoder
	 */
	void registerCodec(const std::string& mime, VideoEncoder& encoder, VideoDecoder& decoder);

	/**
	 * @param mime The mime type corresponding to the decoding unit to remove.
	 * @see sfl#VideoRtpSession#unregisterEncoder
	 * @see sfl#VideoRtpSession#unregisterDecoder
	 */
	void unregisterCodec(const std::string& mime);

	/**
	 * Register a given decoder for a given MIME media type.
	 * TODO integrate with the plugin manager.
	 * @precondition The given decoding unit corresponding to the given MIME type must not be present in the table.
	 * @postcondition The decoding unit corresponding to the MIME type will be used if appropriate.
	 */
	void registerDecoder(const std::string& mime, VideoDecoder& decoder);

	/**
	 * @param mime The mime type corresponding to the decoding unit to remove.
	 */
	void unregisterDecoder(const std::string mime);

	/**
	 * Register a given encoder for a given MIME media type.
	 * TODO integrate with the plugin manager.
	 * @precondition The given encoding unit corresponding to the given MIME type must not be present in the table.
	 * @postcondition The encoding unit corresponding to the MIME type will be used if appropriate.
	 */
	void registerEncoder(const std::string& mime, VideoEncoder& decoder);

	/**
	 * @param mime The mime type corresponding to the encoding unit to remove.
	 */
	void unregisterEncoder(const std::string mime);

	void listen();

	static const int SCHEDULING_TIMEOUT = 10000;
	static const int EXPIRE_TIMEOUT = 1000000;

protected:
	/**
	 * Simple dispatch for the VideoFrameDecodedObserver type.
	 * @Override
	 */
	void notify(VideoFrameDecodedObserver* observer, Buffer<uint8_t>& data);

	// FIXME Should not have to override if not needed.
	void notify(VideoFrameDecodedObserver* observer, const std::string& name, Buffer<uint8_t>& data) {};

	/**
	 * @Override
	 */
	bool onRTPPacketRecv(ost::IncomingRTPPkt& packet);

private:
	/**
	 * Helper method to avoid code duplications with different constructors.
	 */
	void init();

	/**
	 * @param mime The mime type corresponding to the decoder.
	 * @return A registered decoder for the given mime type.
	 */
	VideoDecoder* getDecoder(const std::string& mime) throw(MissingPluginException);

	/**
	 * @param mime The mime type corresponding to the encoder.
	 * @return A registered encoder for the given mime type.
	 */
	VideoEncoder* getEncoder(const std::string& mime) throw(MissingPluginException);

	/**
	 * Replace the old codec with the new one.
	 * @param encoderNew The new encoder to use instead of the current one.
	 * @param decoderNew The new decoder to use instead of the current one.
	 */
	void replaceCodec(VideoEncoder* encoderNew, VideoDecoder* decoderNew) throw(MissingPluginException);

	/**
	 * Observer for NAL units produced by the encoder.
	 */
	class EncoderObserver : public VideoFrameEncodedObserver {
	public:
		EncoderObserver(VideoRtpSession* session) : parent(session) {}
		VideoRtpSession* parent;
		void onNewFrameEncoded(std::pair<uint32, Buffer<uint8> >& data) {
			_debug("Sending NAL unit over RTP");
			_debug("Size %d", (data.second).getSize());
			parent->putData(data.first /* timestamp */, (data.second).getBuffer() /* payload */, (data.second).getSize() /* payload size */);
		}
	};
	EncoderObserver* encoderObserver;

	unsigned clockRate;
	ost::RTPSession* session;
	ost::PayloadType payloadType;

	VideoDecoder* decoder;
	std::map<std::string, VideoDecoder*> decoderTable;
	typedef std::map<std::string, VideoDecoder*>::iterator DecoderTableIterator;

	VideoEncoder* encoder;
	std::map<std::string, VideoEncoder*> encoderTable;
	typedef std::map<std::string, VideoEncoder*>::iterator EncoderTableIterator;
};
}
#endif
