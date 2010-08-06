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
class VideoRtpSessionSimple: public ost::RTPSession
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
         * Multiple video codecs might be accepted from
         * the answerer. Therefore, dynamic configuration based on the RTP stream must be performed.
         *
         * @param rtpmap The Rtmap object object holding the information of a "a=rtpmap" line of SDP.
         *
         * @param fmtp The Fmtp object holding the information of a "a=fmtp" line of SDP for some corresponding "a=rtpmap".
         *
         * @throw MissingPluginException if either the decoder, the encoder, or both are missing.
         *
         * @postcondition The first codec registered (temporally) is set as the selected codec. Subsequent calls to this method
         * will only prepare ("cue") other possible session codecs.
         */
        void addSessionCodec (const RtpMap& rtpmap, const Fmtp& fmtp)
        throw (MissingPluginException);

        /**
         * Register an instance of some video codec.
         * @param codec The instance of some video codec to be made available for this session.
         */
        void registerCodec (VideoCodec* codec);

        /**
         * @param mime The mimetype identifying the codec to remove.
         * @postcondition The codec won't be available from that point on.
         */
        void unregisterCodec (const std::string& mime);

        /**
         * @param mime The mime subtype identifying the codec to retrieve.
         * @return The VideoCodecs corresponding to the mime type.
         */
        VideoCodec* getCodec (const std::string& mime);

        /**
         * @param source The video source that will provide raw frames to the encoder, producing RTP packets that get sent in this session.
         */
        void setVideoSource (VideoInputSource& source);

        /**
         * Start sending and receiving in this session.
         * @Override ost#Thread#start
         */
        void start();

        ~VideoRtpSessionSimple();

        static const int SCHEDULING_TIMEOUT = 10000;

        static const int EXPIRE_TIMEOUT = 1000000;

    protected:

        /**
         * @Override
         */
        bool onRTPPacketRecv (ost::IncomingRTPPkt& packet);

        /**
         * @param codec The new codec to be used.
         * @param config The configuration obtained from SDP to pass to the codec.
         * @postcondition The codec will be used for encoding and decoding during the session.
         */
        void setCodec (const RtpMap& rtpmap, const Fmtp& fmtp, VideoCodec* codec);

        /**
         * @param pt The payload type corresponding to the codec to load.
         * @precondition A codec should have been declared for the given payload type.
         * @postcondition The codec will be used for encoding and decoding during the session.
         */
        void setCodec (ost::PayloadType pt);

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
                VideoRtpSessionSimple* parent;
                void onNewFrameEncoded (std::pair<uint32, Buffer<uint8> >& data) {
                    _debug ("Sending NAL unit over RTP");
                    _debug ("Size %d", (data.second).getSize());
                    parent->sendImmediate (data.first /* timestamp */,
                                           (data.second).getBuffer() /* payload */,
                                           (data.second).getSize() /* payload size */);
                }
        };
        EncoderObserver* encoderObserver;

        /**
         * This is the table that maintains the list of video codecs that are available to us.
         * This should eventually move into some kind of PluginManager (that already exists but
         * is not yet integrated).
         *
         * Mapping : (mime_subtype => VideoCodec*)
         */
        typedef std::map<std::string, VideoCodec*>::iterator AvailableCodecIterator;
        typedef std::pair<std::string, VideoCodec*> AvailableCodecEntry;
        std::map<std::string, VideoCodec*> availableCodecs;

        /**
         * This is the table that maintains the list of video codecs that might be used during the
         * rtp session. Those are obtained after SDP negotiation. Normally, only the first one
         * would be used, but "hot swapping" the codec during the session is something admissible.
         *
         * Mapping : (payload_type => VideoCodec*) (payload type can be static or dynamic).
         */
        struct SessionCodecConfiguration {
            SessionCodecConfiguration (const RtpMap& rtpmap, const Fmtp& fmtp,
                                       VideoCodec* codec) :
                    rtpmap (rtpmap), fmtp (fmtp), codec (codec) {
            }

            const RtpMap& rtpmap;
            const Fmtp& fmtp;
            VideoCodec* codec;
        };

        typedef std::map<ost::PayloadType, SessionCodecConfiguration>::iterator SessionCodecIterator;
        typedef std::pair<ost::PayloadType, SessionCodecConfiguration> SessionCodecEntry;
        std::map<ost::PayloadType, SessionCodecConfiguration> sessionsCodecs;

        /**
         * The main active codec being used.
         */
        VideoCodec* activeCodec;

        /**
         * The main active video source being used by the codec.
         */
        VideoInputSource* activeVideoSource;
};
}
#endif
