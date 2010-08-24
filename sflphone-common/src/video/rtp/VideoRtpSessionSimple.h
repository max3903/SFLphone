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

#include "video/codec/VideoCodec.h"

#include "sip/sdp/Fmtp.h"
#include "sip/sdp/RtpMap.h"

namespace sfl
{

/**
 * Objects of this class listen on the network for incoming RTP packets, and decode them with some
 * registered video decoder. Similarly, frames from a video source are encoded and set with the RTP stack.
 *
 * Implementers will want to be notified when video frames get decoded. Therefore, a VideoFrameDecodedObserver
 * can be registered (via the hidden decoder delegate) on the session object.
 */
class VideoRtpSessionSimple: public ost::RTPSession, public AbstractObservable<Buffer<uint8_t>&, VideoFrameDecodedObserver>
{
    public:
        /**
         * @param mutiCastAddress Local multicast network address
         * @param port Local transport port (where incoming packets are expected)
         */
        VideoRtpSessionSimple (ost::InetMcastAddress& multiCastAddress,
                               ost::tpport_t port);

        /**
         * @param unicastAddress Local unicast network address
         * @param port Local transport port (where incoming packets are expected)
         */
        VideoRtpSessionSimple (ost::InetHostAddress& unicastAddress,
                               ost::tpport_t port);

        /**
         * Add a codec to be used in the RTP session. Normally, only one codec will be chosen. However, it
         * might happen that the application chooses to switch between codecs dynamically, and thus answer
         * back with more that one codec. This method allows for that kind of behavior. If a payload type different
         * from the current one is detected in the RTP stream, a lookup table will be read in order to find a codec that
         * corresponds to the payload type.
         *
         * Multiple video codecs might be accepted from the answerer. Therefore, dynamic configuration based on
         * the RTP stream must be performed.
         *
         * @param payloadType The payload type from SDP referring to this codec.
         *
         * @param codec A codec instance, pre-configured from the "a=fmtp" SDP line.
         *
		 * @postcondition The first codec registered (temporally) is set as the selected codec. Subsequent calls to this method
         * will only prepare ("cue") other possible session codecs. A new codec clone will be created from the given one.
         */
        void addSessionCodec(ost::PayloadType payloadType, const sfl::VideoCodec* codec);

        /**
         * @param source The video source that will provide raw frames to the encoder, producing RTP packets that get sent in this session.
         */
        void setVideoInputFormat (const VideoFormat& format);

        /**
         * Start sending and receiving in this session.
         * @Override ost#Thread#start
         */
        void start();

        /**
         * Encode (asynchronously) then send the given video frame.
         * @param frame The video frame to send.
         * @precondition A video codec should have been added to the session codec.
         */
        void sendPayloaded(const VideoFrame* frame);

        ~VideoRtpSessionSimple();

        static const int SCHEDULING_TIMEOUT = 10000;

        static const int EXPIRE_TIMEOUT = 1000000;

    protected:

        /**
         * @Override
         */
        bool onRTPPacketRecv (ost::IncomingRTPPkt& packet);

        /**
         * @param pt The payload type identifying the codec to use.
         * @precondition A video codec instance must exists for the given payload type and have been added via the
         * VideoRtpSessionSimple#addSessionCodec() method
         */
        void setActiveCodec(ost::PayloadType pt);

    private:
        /**
         * Helper method to avoid code duplications with different constructors.
         */
        void init();

        /**
         * Encoder observer. Take the payloaded packets, and send to our RTP stack.
         */
        class EncoderObserver: public VideoFrameEncodedObserver
        {
            public:
                EncoderObserver (VideoRtpSessionSimple* session) :
                        parent (session) {
                }

            	/**
            	 * @Override
            	 */
                void onNewFrameEncoded (std::pair<uint32, Buffer<uint8> >& data) {
                    //_debug ("Sending NAL unit of size %d over RTP", (data.second).getSize());
                    parent->sendImmediate (data.first /* timestamp */,
                                           (data.second).getBuffer() /* payload */,
                                           (data.second).getSize() /* payload size */);
                }

            private:
                VideoRtpSessionSimple* parent;
        };
        EncoderObserver* encoderObserver;

        /**
         * Decoder observer, only meant to hide the observable mechanism at the decoder leve
         * and make codec switching easier.
         */
        class DecoderObserver : public VideoFrameDecodedObserver
        {
			public:
        	DecoderObserver (VideoRtpSessionSimple* session) :
                    parent (session) {}

        	/**
        	 * @Override
        	 */
        	inline void onNewFrameDecoded (Buffer<uint8_t>& data) {
        		parent->notifyAll(data);
        	}

			private:
            VideoRtpSessionSimple* parent;
        };
        DecoderObserver* decoderObserver;

        typedef std::map<ost::PayloadType, VideoCodec*>::iterator SessionCodecIterator;
        typedef std::pair<ost::PayloadType, VideoCodec*> SessionCodecEntry;
        std::map<ost::PayloadType, VideoCodec*> sessionsCodecs;

        /**
         * The main active codec being used.
         */
        VideoCodec* activeCodec;

        /**
         * The video format in which the video frames are expected to be received.
         */
        VideoFormat currentVideoFormat;

        /**
         * @Override
         */
        inline void notify (VideoFrameDecodedObserver* observer, Buffer<uint8_t>& data) {
        	observer->onNewFrameDecoded(data);
        }

        /**
         * @Override
         */
        void notify (VideoFrameDecodedObserver* observer, const std::string& name, Buffer<uint8_t>& data) {}
};
}
#endif
