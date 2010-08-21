/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
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

#ifndef __SDP_H__
#define __SDP_H__

#include <pjmedia/sdp.h>
#include <pjmedia/sdp_neg.h>
#include <pjsip/sip_transport.h>
#include <pjlib.h>
#include <pjsip_ua.h>
#include <pjmedia/errno.h>
#include <pj/pool.h>
#include <pj/assert.h>
#include <vector>

#include "CodecFactory.h"
#include "SdpMedia.h"

#include <exception>

class SdpException: public std::exception
{
        virtual const char* what() const throw () {
            return "An SdpException occured";
        }
};

typedef std::vector<std::string> CryptoOffer;

class Sdp
{
    public:
        /*
         * Class Constructor.
         *
         * @param ip_addr
         */
        Sdp (pj_pool_t *pool);

        /* Class destructor */
        ~Sdp();

        /*
         * Get the list of the local media capabilities.
         * @return std::vector<SdpMedia*>   the vector containing the different media
         */
        std::vector<SdpMedia*> getInitialMediaList (void);

        /**
         * Get the sdp session information
         *  @return pjmedia_sdp_session   The structure that describes a SDP session
         */
        pjmedia_sdp_session* getLocalSdpSession (void);

        /*
         * Build the local SDP offer from the SDP media that were configured.
         * @precondition The user must have configured some sdp media to use
         * @see Sdp#setLocalMediaCapabilities
         */
        int createLocalOffer();

        /*
         * Build the SDP media section for a given media type (audio/video)
         * Add rtpmap field if necessary
         *
         * @param media The media to add to SDP
         * @param med   The structure to receive the media section
         */
        void setMediaDescriptorLine (SdpMedia* media, pjmedia_sdp_media** p_med);

        /* Set the zrtp hash that was previously calculated from the hello message in the zrtp layer.
         * This hash value is unique at the media level. Therefore, if video support is added, one would
         * have to set the correct zrtp-hash value in the corresponding media section.
         * @param hash The hello hash of a rtp session. (Only audio at the moment)
         */
        void setZrtpHash (const std::string& hash);

        /**
         * Set the srtp _master_key
         * @param mk The Master Key of a srtp session.
         */
        void setSrtpMasterKey (const std::vector<std::string> lc);

        /**
         * @param mime The mime type to search for
         * @return The fist SDP media corresponding to the given mime type, or NULL.
         */
        SdpMedia* getInitialMedia(const std::string& mime);

        /**
         * @param mime The mime type to search for
         * @param payload The payload type to search for.
         * @return The codec with the given mime type, and payload type.
         */
        const sfl::Codec* getInitialCodec(const std::string& mime, const std::string& payload);

        /**
         * Creates a new SdpMedia for the given mime type and selected codecs.
         * @param mime The mime type for this SDP media. Either audio, or video
         * @param selectedCodecs The codecs that the user has chosen.
         */
        void setLocalMediaCapabilities (MimeType mime, CodecOrder selectedCodecs);

        /**
         * This callback is called when format-specific validation must be performed.
         *
         * @param offer The current offer.
         *
         * @param answer The current answer.
         *
         * @param modified_answered A pointer passed by the negotiator so that
         * if modifications must be made in the answer, then those will be taken into
         * account. To modify the current answer, one must first clone the media with
         * #pjmedia_sdp_media_clone() the set the pointer to the modified media. If
         * no changes were made, then the value of the pointer should be left to NULL.
         * If the currrent SDP media can't be modified at some given point, then the
         * argument will be NULL, and it's up to the caller to see if it is the case.
         *
         * @return PJ_FALSE if the format must be rejected (does not match), PJ_TRUE otherwise.
         */
        static pj_bool_t on_format_negotiation(const pjmedia_sdp_media *offer, unsigned o_fmt_index,
			      const pjmedia_sdp_media *answer, unsigned a_fmt_index,
			      pjmedia_sdp_media** modified_answered, void *user_data);

        /*
         * On receiving an invite outside a dialog, build the local offer and create the
         * SDP negociator instance with the remote offer.
         *
         * @param remote    The remote offer
         */
        int receiveInitialOffer (pjmedia_sdp_session* remote);

        /**
         * Remove all media in the session media vector.
         */
        void cleanSessionMedia ();

        /**
         * Remove all media in local media capability vector
         */
        void cleanLocalMediaCapabilities ();

        /*
         * Return a string description of the media added to the session,
         * ie the local media capabilities
         */
        std::string mediaToString ();

        /*
         * @return the codec (audio or video, the order being arbitrary) of the first media after negotiation
         */
        const sfl::Codec* getFirstCodec ();

        /**
         * @return The first audio codec appearing in the negotiated offer under the "audio" media section.
         */
        const AudioCodec* getFirstNegotiatedAudioCodec ();

        /**
         * @return The first video codec appearing in the negotiated offer under the "video" media section.
         */
        const sfl::VideoCodec* getFirstNegotiatedVideoCodec ();

        /**
         * @return A list of negotiated video codecs. Note that in most cases, this list will contain only one element.
         * @see Sdp#getFirstNegotiatedVideoCodec
         */
        std::vector<const sfl::VideoCodec*> getNegotiatedVideoCodecs();

        /*
         * read accessor. Return the negotiated offer
         *
         * @return pjmedia_sdp_session  The negotiated offer
         */
        pjmedia_sdp_session* getNegotiatedOffer (void);

        /*
         * Start the sdp negotiation.
         *
         * @return pj_status_t  0 on success
         *                      1 otherwise
         */
        pj_status_t startNegotiation (void);

        /*
         * Retrieve the negotiated sdp offer from the sip payload.
         *
         * @param sdp   the negociated offer
         */
        void setNegotiatedSdp (const pjmedia_sdp_session *sdp);

        /**
         * @param port The audio port (published/STUNed) of the upcoming RTP session.
         */
        void setPublishedAudioPort (int port);

        /**
         * @return port The audio port (published/STUNed) of the upcoming RTP session.
         */
        int getPublishedAudioPort (void);

        /**
         * @param port The video port (published/STUNed) of the upcoming RTP session.
         */
        void setPublishedVideoPort (int port);

        /**
         * @return port The video port (published/STUNed) of the upcoming RTP session.
         */
        int getPublishedVideoPort (void);

        /*
         * @param ip_addr The local IP address that will be used in the sdp session
         */
        void setIpAddress (std::string ip_addr);

        /*
         * Read accessor. Get the local IP address
         */
        std::string getIpAddress (void);

        /**
         * @param ip The remote IP address
         */
        void setRemoteIp (const std::string& ip);

        /**
         * @return The remote IP address
         */
        const std::string& getRemoteIp();

        /**
         * Set remote's audio port.
         * @param port  The remote audio port
         */
        void setRemoteAudioPort (unsigned int port);

        /**
         * Return audio port at destination
         * @return unsigned int The remote audio port
         */
        unsigned int getRemoteAudioPort();

        /**
         * Set the remote video port.
         * @param port The remote video port
         */
        void setRemoteVideoPort (unsigned int port);

        /**
         * Return video port at destination
         * @return unsigned int The remote video port
         */
        unsigned int getRemoteVideoPort();

        /**
         * Setting the VideoFormat is required for some codec such as Theora that must
         * send width/height and subsampling parameters in SDP.
         *
         * @param format The video format in which the frames will be obtained.
         * @postcondition Calling this method will trigger creation of SDP parameters on every
         * available video codec.
         */
        void setVideoFormat(const sfl::VideoFormat& format);

        void setMediaFromSdpAnswer (const pjmedia_sdp_session *remote_sdp);

        std::vector<SdpMedia*> getNegotiatedMediaList (void);

        void getRemoteSdpCryptoFromOffer (const pjmedia_sdp_session* remote_sdp,
                                          CryptoOffer& crypto_offer);

        /**
         * @return A textual description of the object.
         */
        std::string toString (void);

    private:
        pj_pool_t* getPool();

        /** Codec Map */
        std::vector<SdpMedia*> _initialMedias;
        typedef std::vector<SdpMedia*>::iterator InitialMediasIterator;

        /**
         * Payload predicate to match an SdpMedia
         */
        class IsSameMedia
        {
            public:
                IsSameMedia (std::string other, uint16 port) :
                        otherMedia (other),  port (port) {
                }
                bool operator() (SdpMedia*& media) {
                    return (media->getMediaTypeStr() == otherMedia) && (media->getPort() == port);
                }
            private:
                std::string otherMedia;
                uint16 port;
        };

        /* The media that will be used by the session (after the SDP negotiation) */
        std::vector<SdpMedia*> _negotiatedMedias;
        typedef std::vector<SdpMedia*>::iterator NegotiatedMediasIterator;

        /** negotiator */
        pjmedia_sdp_neg *_negotiator;

        /** IP address */
        std::string _ip_addr;

        /** Remote's IP address */
        std::string _remoteIpAddress;

        /** Local SDP */
        pjmedia_sdp_session *_local_offer;

        /* The negotiated SDP offer */
        // Explanation: each endpoint's offer is negotiated, and a new sdp offer results from this
        // negotiation, with the compatible media from each part
        pjmedia_sdp_session *_negotiated_offer;

        // The pool to allocate memory
        pj_pool_t *_pool;

        /** Local port */
        int _publishedAudioPort;

        int _publishedVideoPort;

        /** Remote port */
        unsigned int _remoteAudioPort;

        unsigned int _remoteVideoPort;

        std::string _zrtp_hello_hash;

        /** "a=crypto" sdes local attributes obtained from AudioSrtpSession */
        std::vector<std::string> _srtp_crypto;

        /**
         * No copy constructor.
         */
        Sdp (const Sdp&);

        /**
         * No assignment operator
         */
        Sdp& operator= (const Sdp&);

        /*
         *  Mandatory field: Origin ("o=")
         *  Gives the originator of the session.
         *  Serves as a globally unique identifier for this version of this session description.
         */
        void sdpAddOrigin (void);

        /*
         *  Mandatory field: Protocol version ("v=")
         *  Add the protocol version in the SDP session description
         */
        void sdpAddProtocol (void);

        /*
         *  Optional field: Connection data ("c=")
         *  Contains connection data.
         */
        void sdpAddConnectionInfo (void);

        /*
         *  Mandatory field: Session name ("s=")
         *  Add a textual session name.
         */
        void sdpAddSessionName (void);

        /*
         *  Optional field: Session information ("s=")
         *  Provides textual information about the session.
         */
        void sdp_add_session_info (void) {
        }

        /*
         *  Optional field: Uri ("u=")
         *  Add a pointer to additional information about the session.
         */
        void sdp_add_uri (void) {
        }

        /*
         *  Optional fields: Email address and phone number ("e=" and "p=")
         *  Add contact information for the person responsible for the conference.
         */
        void sdp_add_email (void) {
        }

        /*
         *  Optional field: Bandwidth ("b=")
         *  Denotes the proposed bandwidth to be used by the session or the media .
         */
        void sdp_add_bandwidth (void) {
        }

        /*
         *  Mandatory field: Timing ("t=")
         *  Specify the start and the stop time for a session.
         */
        void sdpAddTiming (void);

        /*
         * Optional field: Time zones ("z=")
         */
        void sdp_add_time_zone (void) {
        }

        /*
         * Optional field: Encryption keys ("k=")
         */
        void sdp_add_encryption_key (void) {
        }

        /*
         * Optional field: Attributes ("a=")
         */
        void sdpAddAttributes();

        /*
         * Mandatory field: Media descriptions ("m=")
         */
        void sdpAddMediaDescription();

        std::string intToString (int value);

        void setRemoteIpFromSdp (const pjmedia_sdp_session *r_sdp);

        /**
         * @param remote The SDP answer.
         * @return The first pjmedia_sdp_media of type "audio" in the given SDP session.
         */
        pjmedia_sdp_media* getAudioSdpMedia (const pjmedia_sdp_session* remote);

        /**
         * @param remote The SDP answer.
         * @return The first pjmedia_sdp_media of type "video" in the given SDP session.
         */
        pjmedia_sdp_media* getVideoSdpMedia (const pjmedia_sdp_session* remoteSdp);

        /*
         * Adds a sdes attribute to the given media section.
         *
         * @param media The media to add the srtp attribute to
         */
        void sdpAddSdesAttribute (std::vector<std::string>& crypto);

        /*
         * Adds a zrtp-hash  attribute to
         * the given media section. The hello hash is
         * available only after is has been computed
         * in the AudioZrtpSession constructor.
         *
         * @param media The media to add the zrtp-hash attribute to
         * @param hash  The hash to which the attribute should be set to
         */
        void sdpAddZrtpAttribute (pjmedia_sdp_media* media, std::string hash);

};

#endif
