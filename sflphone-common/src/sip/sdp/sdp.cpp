/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
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

#include "sdp.h"
#include "Fmtp.h"

#include "Codec.h"
#include "CodecFactory.h"

#include "global.h"

#include "manager.h"

#include <algorithm>

#define ZRTP_VERSION "1.10"

static const pj_str_t STR_AUDIO = { (char*) "audio", 5 };
static const pj_str_t STR_VIDEO = { (char*) "video", 5 };
static const pj_str_t STR_IN = { (char*) "IN", 2 };
static const pj_str_t STR_IP4 = { (char*) "IP4", 3 };
static const pj_str_t STR_IP6 = { (char*) "IP6", 3 };
static const pj_str_t STR_RTP_AVP = { (char*) "RTP/AVP", 7 };
static const pj_str_t STR_RTP_SAVP = { (char*) "RTP/SAVP", 8 };
static const pj_str_t STR_SDP_NAME = { (char*) "sflphone", 8 };
static const pj_str_t STR_SENDRECV = { (char*) "sendrecv", 8 };
static const pj_str_t STR_RTPMAP = { (char*) "rtpmap", 6 };
static const pj_str_t STR_CRYPTO = { (char*) "crypto", 6 };

void Sdp::sdpAddProtocol(void) {

	this->_local_offer->origin.version = 0;
}

void Sdp::sdpAddOrigin(void) {

	pj_time_val tv;
	pj_gettimeofday(&tv);

	this->_local_offer->origin.user = pj_str(pj_gethostname()->ptr);
	// Use Network Time Protocol format timestamp to ensure uniqueness.
	this->_local_offer->origin.id = tv.sec + 2208988800UL;
	// The type of network ( IN for INternet )
	this->_local_offer->origin.net_type = STR_IN;
	// The type of address
	this->_local_offer->origin.addr_type = STR_IP4;
	// The address of the machine from which the session was created
	this->_local_offer->origin.addr = pj_str((char*) _ip_addr.c_str());
}

void Sdp::sdpAddSessionName(void) {

	this->_local_offer->name = STR_SDP_NAME;
}

void Sdp::sdpAddConnectionInfo(void) {

	this->_local_offer->conn->net_type = _local_offer->origin.net_type;
	this->_local_offer->conn->addr_type = _local_offer->origin.addr_type;
	this->_local_offer->conn->addr = _local_offer->origin.addr;
}

void Sdp::sdpAddTiming(void) {

	// RFC 3264: An offer/answer model session description protocol
	// As the session is created and destroyed through an external signaling mean (SIP), the line
	// should have a value of "0 0".

	this->_local_offer->time.start = this->_local_offer->time.stop = 0;
}

void Sdp::sdpAddAttributes() {
	pjmedia_sdp_attr *a;
	this->_local_offer->attr_count = 1;
	a = PJ_POOL_ZALLOC_T (_pool, pjmedia_sdp_attr);
	a->name = STR_SENDRECV;
	_local_offer->attr[0] = a;
}

void Sdp::sdpAddMediaDescription() {
	pjmedia_sdp_media* med;

	med = PJ_POOL_ZALLOC_T (_pool, pjmedia_sdp_media);

	std::vector<SdpMedia*> mediaList = getInitialMediaList();
	_local_offer->media_count = mediaList.size();

	int i;
	for (i = 0; i < _local_offer->media_count; i++) {
		setMediaDescriptorLine(mediaList[i], &med);
		_local_offer->media[i] = med;
	}
}

void Sdp::sdpAddSdesAttribute(std::vector<std::string>& crypto) {

	// temporary buffer used to store crypto attribute
	char tempbuf[256];

	std::vector<std::string>::iterator iter = crypto.begin();

	while (iter != crypto.end()) {
		// the attribute to add to sdp
		pjmedia_sdp_attr *attribute = (pjmedia_sdp_attr*) pj_pool_zalloc(_pool,
				sizeof(pjmedia_sdp_attr));

		attribute->name = pj_strdup3(_pool, "crypto");

		// _debug("crypto from sdp: %s", crypto.c_str());

		int len = pj_ansi_snprintf(tempbuf, sizeof(tempbuf), "%.*s",
				(int) (*iter).size(), (*iter).c_str());

		attribute->value.slen = len;
		attribute->value.ptr = (char*) pj_pool_alloc(_pool,
				attribute->value.slen + 1);
		pj_memcpy(attribute->value.ptr, tempbuf, attribute->value.slen + 1);

		// get number of media for this SDP
		int media_count = _local_offer->media_count;

		// add crypto attribute to media
		for (int i = 0; i < media_count; i++) {
			if (pjmedia_sdp_media_add_attr(_local_offer->media[i], attribute)
					!= PJ_SUCCESS) {
				// if(pjmedia_sdp_attr_add(&(_local_offer->attr_count), _local_offer->attr, attribute) != PJ_SUCCESS){
				throw SdpException();
			}
		}

		iter++;
	}
}

void Sdp::sdpAddZrtpAttribute(pjmedia_sdp_media* media, std::string hash) {
	pjmedia_sdp_attr *attribute;
	char tempbuf[256];
	int len;

	attribute = (pjmedia_sdp_attr*) pj_pool_zalloc(_pool,
			sizeof(pjmedia_sdp_attr));

	attribute->name = pj_strdup3(_pool, "zrtp-hash");

	/* Format: ":version value" */
	len = pj_ansi_snprintf(tempbuf, sizeof(tempbuf), "%.*s %.*s", 4,
			ZRTP_VERSION, (int) hash.size(), hash.c_str());

	attribute->value.slen = len;
	attribute->value.ptr = (char*) pj_pool_alloc(_pool, attribute->value.slen
			+ 1);
	pj_memcpy(attribute->value.ptr, tempbuf, attribute->value.slen + 1);

	if (pjmedia_sdp_media_add_attr(media, attribute) != PJ_SUCCESS) {
		throw SdpException();
	}
}

std::string Sdp::intToString(int value) {
	std::ostringstream result;
	result << value;
	return result.str();
}

void Sdp::setRemoteIpFromSdp(const pjmedia_sdp_session *r_sdp) {
	std::string remote_ip(r_sdp->conn->addr.ptr, r_sdp->conn->addr.slen);
	_info ("SDP: Remote IP from fetching SDP: %s", remote_ip.c_str());
	this->setRemoteIp(remote_ip);
}

pjmedia_sdp_media* Sdp::getAudioSdpMedia(const pjmedia_sdp_session* remoteSdp) {
	if (!remoteSdp) {
		_warn("Passing NULL parameter (%s:%d)", __FILE__, __LINE__);
		return NULL;
	}

	int i;
	for (i = 0; i < remoteSdp->media_count; ++i) {
		if (pj_stricmp2(&remoteSdp->media[i]->desc.media, "audio") == 0) {
			return remoteSdp->media[i];;
		}
	}

	return NULL;
}

pjmedia_sdp_media* Sdp::getVideoSdpMedia(const pjmedia_sdp_session* remoteSdp) {
	if (!remoteSdp) {
		return NULL;
	}

	int i;
	for (i = 0; i < remoteSdp->media_count; ++i) {
		if (pj_stricmp2(&remoteSdp->media[i]->desc.media, "video") == 0) {
			return remoteSdp->media[i];;
		}
	}

	return NULL;
}

pj_pool_t* Sdp::getPool() {
	return _pool;
}

pj_bool_t Sdp::on_format_negotiation(const pjmedia_sdp_media *offer,
		unsigned o_fmt_idx, const pjmedia_sdp_media *answer,
		unsigned a_fmt_idx, pjmedia_sdp_media** modified_answered,
		void *user_data) {
	Sdp* self = (Sdp*) user_data;

	/* Parse offerer FMTP */
	sfl::Fmtp fmtpOfferer;
	pjmedia_sdp_fmtp fmtpAttribute;
	const pjmedia_sdp_attr* attr_ofr = pjmedia_sdp_media_find_attr2(offer,
			"fmtp", &offer->desc.fmt[o_fmt_idx]);
	if (attr_ofr) {
		if (pjmedia_sdp_attr_get_fmtp(attr_ofr, &fmtpAttribute) != PJ_SUCCESS)
			/* Invalid fmtp format. */
			return PJ_FALSE;

		fmtpOfferer = sfl::Fmtp(std::string(fmtpAttribute.fmt.ptr,
				fmtpAttribute.fmt.slen), std::string(
				fmtpAttribute.fmt_param.ptr, fmtpAttribute.fmt_param.slen));
	}

	/* Parse answerer FMTP */
	sfl::Fmtp fmtpAnswerer;
	const pjmedia_sdp_attr* attr_ans = pjmedia_sdp_media_find_attr2(answer,
			"fmtp", &answer->desc.fmt[a_fmt_idx]);
	if (attr_ans) {
		if (pjmedia_sdp_attr_get_fmtp(attr_ans, &fmtpAttribute) != PJ_SUCCESS)
			/* Invalid fmtp format. */
			return PJ_FALSE;

		fmtpAnswerer = sfl::Fmtp(std::string(fmtpAttribute.fmt.ptr,
				fmtpAttribute.fmt.slen), std::string(
				fmtpAttribute.fmt_param.ptr, fmtpAttribute.fmt_param.slen));
	}

	// Get the corresponding codec from the offer
	std::string mime(offer->desc.media.ptr, offer->desc.media.slen);
	std::string payload(offer->desc.fmt[o_fmt_idx].ptr, offer->desc.fmt[o_fmt_idx].slen);

	const sfl::Codec* codec = self->getInitialCodec(mime, payload);
	if (codec) {
		sfl::Fmtp negotiatedFormat(std::string(answer->desc.fmt[a_fmt_idx].ptr,
				answer->desc.fmt[a_fmt_idx].slen));

		// FIXME Stripping the const away. We know however that it won't impact the codec state undesirably
		_debug("Calling the negotiation handler for the codec in the inital list...");

		try {
			negotiatedFormat = (const_cast<sfl::Codec*> (codec))->negotiate(
					fmtpOfferer, fmtpAnswerer);
		} catch (sfl::SdpFormatNegotiationException e) {
			_error("%s", e.what());
			return PJ_FALSE;
		}

		_debug("Offerer format %s", fmtpOfferer.getParametersFormatted().c_str());
		_debug("Answerer format %s", fmtpAnswerer.getParametersFormatted().c_str());
		_debug("Negotiated format %s", negotiatedFormat.getParametersFormatted().c_str());

		// The format line has changed
		if (negotiatedFormat != fmtpAnswerer) {
			if (modified_answered) {
				std::string formattedList = negotiatedFormat.getParametersFormatted();
				std::string value = negotiatedFormat.getPayloadType()
						+ std::string(" ") + formattedList;

				// Set the new format in the pjmedia structure
				(*modified_answered) = pjmedia_sdp_media_clone(self->getPool(), answer);

				pjmedia_sdp_attr* attrNewMedia = pjmedia_sdp_media_find_attr2((*modified_answered),
						"fmtp", &(*modified_answered)->desc.fmt[a_fmt_idx]);
				if (attrNewMedia) {
					_debug("Setting new negotiated format to \"%s\"", value.c_str());
					attrNewMedia->value = pj_str(strdup(value.c_str()));
				}
			} else {
				_debug("Modified_answer is NULL");
			}
		}
	} else {
		_error("Codec is NULL in (%s:%d) meaning that we failed to retrieve a codec from the initial list", __FILE__, __LINE__);
	}

	return PJ_TRUE;
}

SdpMedia* Sdp::getInitialMedia(const std::string& mime) {
	_debug("Getting media %s", mime.c_str());
	std::vector<SdpMedia*>::iterator it;
	for (it = _initialMedias.begin(); it != _initialMedias.end(); it++) {
		if ((*it)->getMediaTypeStr() == mime) {
			return (*it);
		}
	}

	return NULL;
}

const sfl::Codec* Sdp::getInitialCodec(const std::string& mime, const std::string& payload) {
	_debug("Getting initial media %s %s", mime.c_str(), payload.c_str());
	SdpMedia* media = getInitialMedia(mime);
	if (media) {
		uint8 pt = atoi(payload.c_str());
		const sfl::Codec* codec = media->getCodec(pt);
		if (codec) {
			return codec;
		}
	}

	return NULL;
}

void Sdp::setLocalMediaCapabilities(MimeType mime, CodecOrder selectedCodecs) {
	SdpMedia* media;

	switch (mime) {
	case MIME_TYPE_AUDIO: {
		media = new SdpMedia(MIME_TYPE_AUDIO);
		media->setPort(getPublishedAudioPort());
		break;
	}
	case MIME_TYPE_VIDEO: {
		media = new SdpMedia(MIME_TYPE_VIDEO);
		media->setPort(getPublishedVideoPort());
		break;
	}
	default:
		_error ("Unknown mime type (%s:%d)", __FILE__, __LINE__);
		break;
	};

	// Add selected audio codec to the SDP media.
	CodecFactory& factory = CodecFactory::getInstance();

	CodecOrder::iterator it;
	for (it = selectedCodecs.begin(); it != selectedCodecs.end(); it++) {
		const sfl::Codec* codec = factory.getCodec((*it));
		if (codec != NULL) {
			media->addCodec(codec);
		}
	}

	_debug("Adding media %s to initial medias", media->getMediaTypeStr().c_str());
	_initialMedias.push_back(media);
}

void Sdp::setMediaDescriptorLine(SdpMedia *media, pjmedia_sdp_media** p_med) {
	pjmedia_sdp_media* med;
	pjmedia_sdp_rtpmap rtpmap;
	pjmedia_sdp_attr *attr;

	med = PJ_POOL_ZALLOC_T (_pool, pjmedia_sdp_media);

	// Set the MIME type for this codec
	pj_strdup(_pool, &med->desc.media, (media->getMediaType()
			== MIME_TYPE_AUDIO) ? &STR_AUDIO : &STR_VIDEO);

	// Set the RTP port
	med->desc.port_count = 1;
	med->desc.port = media->getPort();

	// In case of sdes, media are tagged as "RTP/SAVP", RTP/AVP elsewhere
	if (_srtp_crypto.empty()) {
		pj_strdup(_pool, &med->desc.transport, &STR_RTP_AVP);
	} else {
		pj_strdup(_pool, &med->desc.transport, &STR_RTP_SAVP);
	}

	// Media format (RTP payload)
	std::vector<const sfl::Codec*> codecList = media->getMediaCodecList();
	int count = codecList.size();
	med->desc.fmt_count = count;

	// Add the payload list
	int i;
	for (i = 0; i < count; i++) {
		const sfl::Codec* codec = codecList[i];
		std::string payloadType = intToString(codec->getPayloadType());
		pj_strdup2(_pool, &med->desc.fmt[i], payloadType.c_str());

		// Add a rtpmap field for each codec
		// We could add one only for dynamic payloads because the codecs with static RTP payloads
		// are entirely defined in the RFC 3351, but if we want to add other attributes like an asymmetric
		// connection, the rtpmap attribute will be useful to specify for which codec it is applicable
		rtpmap.pt = med->desc.fmt[i];
		pj_strdup2(_pool, &rtpmap.enc_name, codec->getMimeSubtype().c_str());

		// Set clock rate
		rtpmap.clock_rate = codec->getClockRate();

		// Set audio specific options
		if (codec->getMimeType() == "audio") {
			const AudioCodec* audioCodec =
					static_cast<const AudioCodec*> (codec);

			// G722 require G722/8000 media description even if it is 16000 codec
			if (audioCodec->getPayloadType() == 9) {
				rtpmap.clock_rate = 8000;
			}

			// Add the channel number only if different from 1
			if (audioCodec->getChannel() > 1) {
				rtpmap.param = pj_str((char*) audioCodec->getChannel());
			} else {
				rtpmap.param.slen = 0;
			}
		}

		// Convert the rtpmap structure into an SDP attribute
		pjmedia_sdp_rtpmap_to_attr(_pool, &rtpmap, &attr);
		_debug("%.*s", attr->value.slen, attr->value.ptr);
		pjmedia_sdp_media_add_attr(med, attr);

		// We know it's OK to strip the const away in this very case,
		// but maybe we should think about doing it differently
		std::string params =
				const_cast<sfl::Codec*> (codec)->getParametersFormatted();
		if (params != "") {
			std::string value = std::string(med->desc.fmt[i].ptr,
					med->desc.fmt[i].slen) + std::string(" ") + params;

			attr = PJ_POOL_ALLOC_T(_pool, pjmedia_sdp_attr); // FIXME Have no idea how it could be freed.
			attr->name.ptr = "fmtp";
			attr->name.slen = 4;
			attr->value = pj_str(strdup(value.c_str()));

			pjmedia_sdp_media_add_attr(med, attr); // FIXME Have no idea how it could be freed.
		}
	}

	// Add the direction stream
	attr = (pjmedia_sdp_attr*) pj_pool_zalloc(_pool, sizeof(pjmedia_sdp_attr));
	pj_strdup2(_pool, &attr->name, media->getStreamDirectionStr().c_str());
	pjmedia_sdp_media_add_attr(med, attr);

	if (!_zrtp_hello_hash.empty()) {
		try {
			sdpAddZrtpAttribute(med, _zrtp_hello_hash);
		} catch (...) { // TODO Catch more precisely
			throw;
		}
	} else {
		_warn ("No hash specified");
	}

	*p_med = med;
}

int Sdp::createLocalOffer() {
	_info ("SDP: Create local offer");

	// Create and initialize basic SDP session
	this->_local_offer = PJ_POOL_ZALLOC_T (_pool, pjmedia_sdp_session);
	this->_local_offer->conn = PJ_POOL_ZALLOC_T (_pool, pjmedia_sdp_conn);

	// Initialize the fields of the struct
	sdpAddProtocol();
	sdpAddOrigin();
	sdpAddSessionName();
	sdpAddConnectionInfo();
	sdpAddTiming();
	sdpAddMediaDescription();

	if (!_srtp_crypto.empty()) {
		sdpAddSdesAttribute(_srtp_crypto);
	}

	// Validate the sdp session
	return pjmedia_sdp_validate(this->_local_offer);
}

void Sdp::setNegotiatedSdp(const pjmedia_sdp_session *sdp) {
	_negotiated_offer = (pjmedia_sdp_session*) sdp;
	unsigned nb_media = _negotiated_offer->media_count;

	// Iterate over all medias
	unsigned i;
	for (i = 0; i < nb_media; i++) {
		pjmedia_sdp_media* returnedMedia = _negotiated_offer->media[i];

		std::string mediaType(returnedMedia->desc.media.ptr,
				returnedMedia->desc.media.slen);
		int port = returnedMedia->desc.port;

		// Try to match the returned media with the media in the initial offer
		InitialMediasIterator it = std::find_if(_initialMedias.begin(),
				_initialMedias.end(), IsSameMedia(mediaType, port));
		if (it == _initialMedias.end()) {
			_error ("A media of type \"%s\" was received in the SDP answer, but was not specified in the initial offer.", mediaType.c_str());
			continue;
		}

		SdpMedia* initialSdpMedia = (*it);
		SdpMedia* negotiatedMedia = new SdpMedia(mediaType, port);

		// Iterate over the payloads for that media
		unsigned nb_codecs = returnedMedia->desc.fmt_count;
		unsigned j;

		for (j = 0; j < nb_codecs; j++) {

			// Inspect the a=rtpmap line
			pjmedia_sdp_attr* attribute = pjmedia_sdp_media_find_attr(
					returnedMedia, &STR_RTPMAP, NULL);
			if (!attribute) {
				_error ("a=rtpmap cannot be found in SDP.");
				return;
			}

			// Given the codec, match it to the SdpMedia locally
			pjmedia_sdp_rtpmap *rtpmap;
			pjmedia_sdp_attr_to_rtpmap(_pool, attribute, &rtpmap);
			const sfl::Codec* negotiatedCodec = initialSdpMedia->getCodec(
					pj_strtoul(&rtpmap->pt));
			if (!negotiatedCodec) {
				_error ("Codec with payload number %d was returned in the SDP answer, but was not present initially");
				continue;
			}

			// TODO Inspect the a=fmtp line
			// Format parameters might have changed, make sure that everything is still valid.
			attribute = pjmedia_sdp_media_find_attr2(returnedMedia, "fmtp",
					&rtpmap->pt);
			if (attribute) {
				pjmedia_sdp_fmtp fmtpAttribute;
				if (pjmedia_sdp_attr_get_fmtp(attribute, &fmtpAttribute)
						== PJ_SUCCESS) {
					sfl::Fmtp fmtp(std::string(fmtpAttribute.fmt.ptr,
							fmtpAttribute.fmt.slen), std::string(
							fmtpAttribute.fmt_param.ptr,
							fmtpAttribute.fmt_param.slen));

					sfl::Codec* modifiedCodec = negotiatedCodec->clone();

					sfl::Fmtp::iterator it;
					for (it = fmtp.begin(); it != fmtp.end(); it++) {
						_debug("Parameter in setNegotiatedSdp : %s = %s", (*it).first.c_str(), (*it).second.c_str());
						// Set those parameters in the codec
						// Note that since we have a validation function (callback), then we can be sure that
						// that those options are correct.
						modifiedCodec->setParameter((*it).first, (*it).second);
					}

					negotiatedCodec = modifiedCodec;
				}
			} else {
				_error("No a=fmtp attribute in local offer");
			}

			negotiatedMedia->addCodec(negotiatedCodec);
		}

		_negotiatedMedias.push_back(negotiatedMedia);
	}
}

int Sdp::receiveInitialOffer(pjmedia_sdp_session* remote) {
	if (!remote) {
		return !PJ_SUCCESS;
	}

	_info("Receiving initial offer.");

	// Retrieve some useful remote information
	this->setMediaFromSdpAnswer(remote);

	pj_status_t status = pjmedia_sdp_neg_create_w_remote_offer(_pool,
			getLocalSdpSession(), remote, &_negotiator);

	PJ_ASSERT_RETURN (status == PJ_SUCCESS, 1); // FIXME Useless. Return pj_status and it will be alright.

	return PJ_SUCCESS;
}

void Sdp::setMediaFromSdpAnswer(const pjmedia_sdp_session* remoteSdp) {
	_info ("SDP: Fetching media from sdp ...");
	if (!remoteSdp) {
		return;
	}

	// Set the remote IP
	setRemoteIpFromSdp(remoteSdp);

	// Set the remote audio port
	pjmedia_sdp_media* media = getAudioSdpMedia(remoteSdp);
	if (media == NULL) {
		_error ("SDP: No audio media specified in SDP answer.");
		return;
	}

	setRemoteAudioPort(media->desc.port);

	media = getVideoSdpMedia(remoteSdp);
	if (media == NULL) {
		_debug ("SDP: No video media specified in SDP answer.");
		return;
	}
}

std::vector<SdpMedia*> Sdp::getInitialMediaList() {
	_debug("Returning medias");
	return _initialMedias;
}

pjmedia_sdp_session* Sdp::getLocalSdpSession() {
	return _local_offer;
}

void Sdp::setIpAddress(std::string ip_addr) {
	_ip_addr = ip_addr;
}

std::string Sdp::getIpAddress(void) {
	return _ip_addr;
}

void Sdp::setZrtpHash(const std::string& hash) {
	_zrtp_hello_hash = hash;
}

void Sdp::setSrtpMasterKey(const std::vector<std::string> lc) {
	_srtp_crypto = lc;
}

std::string Sdp::mediaToString(void) {
	int size, i;
	std::ostringstream res;

	size = _initialMedias.size();

	for (i = 0; i < size; i++) {
		res << _initialMedias[i]->toString();
	}

	res << std::endl;

	return res.str();
}

void Sdp::cleanSessionMedia() {
	_info ("SDP: Clean session media");

	if (_negotiatedMedias.size() > 0) {

		std::vector<SdpMedia *>::iterator iter = _negotiatedMedias.begin();
		SdpMedia *media;

		while (iter != _negotiatedMedias.end()) {
			media = *iter;
			delete media;
			iter++;
		}

		_negotiatedMedias.clear();
	}
}

void Sdp::cleanLocalMediaCapabilities() {
	_info ("SDP: Clean local media capabilities");

	if (_initialMedias.size() > 0) {

		std::vector<SdpMedia *>::iterator iter = _initialMedias.begin();
		SdpMedia *media;

		while (iter != _initialMedias.end()) {
			media = *iter;
			delete media;
			iter++;
		}

		_initialMedias.clear();
	}
}

void Sdp::setPublishedAudioPort(int port) {
	_publishedAudioPort = port;
}

int Sdp::getPublishedAudioPort(void) {
	return _publishedAudioPort;
}

void Sdp::setPublishedVideoPort(int port) {
	_publishedVideoPort = port;
}

int Sdp::getPublishedVideoPort(void) {
	return _publishedVideoPort;
}

void Sdp::setRemoteIp(const std::string& ip) {
	_remoteIpAddress = ip;
}

const std::string& Sdp::getRemoteIp() {
	return _remoteIpAddress;
}

void Sdp::setRemoteAudioPort(unsigned int port) {
	_remoteAudioPort = port;
}

unsigned int Sdp::getRemoteAudioPort() {
	return _remoteAudioPort;
}

void Sdp::setRemoteVideoPort(unsigned int port) {
	_remoteVideoPort = port;
}

unsigned int Sdp::getRemoteVideoPort() {
	return _remoteVideoPort;
}

void Sdp::setVideoFormat(const sfl::VideoFormat& format) {
	_debug("Setting video format on SDP media ...");

	std::vector<SdpMedia*> mediaList = getInitialMediaList();
	std::vector<SdpMedia*>::iterator it;
	for (it = mediaList.begin(); it != mediaList.end(); it++) {
		if ((*it)->getMediaType() == MIME_TYPE_VIDEO) {
			(*it)->setVideoFormat(format);
		}
	}
}

const sfl::Codec* Sdp::getFirstCodec() {
	_debug ("SDP: Getting session medias. (%s:%d)", __FILE__, __LINE__);

	std::vector<SdpMedia*> mediaList = getNegotiatedMediaList();
	const sfl::Codec *codec = NULL;
	if (mediaList.size() > 0) {
		std::vector<const sfl::Codec*> codecs =
				mediaList[0]->getMediaCodecList();
		if (codecs.size()) {
			codec = codecs[0];
		}
	}

	return codec;
}

const AudioCodec* Sdp::getFirstNegotiatedAudioCodec() {
	std::vector<SdpMedia*> mediaList = getNegotiatedMediaList();
	std::vector<SdpMedia*>::iterator it;
	for (it = mediaList.begin(); it != mediaList.end(); it++) {
		if ((*it)->getMediaType() == MIME_TYPE_AUDIO) {
			std::vector<const sfl::Codec*> codecList =
					(*it)->getMediaCodecList();
			if (codecList.size() != 0) {
				return dynamic_cast<const AudioCodec*> (codecList.at(0));
			}
		}
	}

	return NULL; // FIXME Throw something instead.
}

const sfl::VideoCodec* Sdp::getFirstNegotiatedVideoCodec() {
	std::vector<SdpMedia*> mediaList = getNegotiatedMediaList();
	std::vector<SdpMedia*>::iterator it;
	for (it = mediaList.begin(); it != mediaList.end(); it++) {
		if ((*it)->getMediaType() == MIME_TYPE_VIDEO) {
			std::vector<const sfl::Codec*> codecList =
					(*it)->getMediaCodecList();
			if (codecList.size() != 0) {
				return dynamic_cast<const sfl::VideoCodec*> (codecList.at(0));
			}

		}
	}

	return NULL; // FIXME Throw something instead.
}

std::vector<const sfl::VideoCodec*> Sdp::getNegotiatedVideoCodecs() {
	std::vector<const sfl::VideoCodec*> output;

	std::vector<SdpMedia*> mediaList = getNegotiatedMediaList();
	std::vector<SdpMedia*>::iterator it;
	for (it = mediaList.begin(); it != mediaList.end(); it++) {
		if ((*it)->getMediaType() == MIME_TYPE_VIDEO) {
			std::vector<const sfl::Codec*> codecList =
					(*it)->getMediaCodecList();
			std::vector<const sfl::Codec*>::iterator codecIt;

			for (codecIt = codecList.begin(); codecIt != codecList.end(); codecIt++) {
				output.push_back(
						dynamic_cast<const sfl::VideoCodec*> ((*codecIt)));
			}
			break; // FIXME Not sure if it makes much sense to go further, ie : is it possible to have multiple "m=video" lines ?
		}
	}

	return output;
}

pjmedia_sdp_session* Sdp::getNegotiatedOffer() {
	return _negotiated_offer;
}

pj_status_t Sdp::startNegotiation() {
	pj_status_t status;

	if (_negotiator) {
		_info("********************************************************************* Starting SDP negotiation ...");
		status = pjmedia_sdp_neg_negotiate(_pool, _negotiator, 0);
	} else {
		status = !PJ_SUCCESS;
	}

	return status;
}

std::vector<SdpMedia*> Sdp::getNegotiatedMediaList(void) {
	return _negotiatedMedias;
}

void Sdp::getRemoteSdpCryptoFromOffer(const pjmedia_sdp_session* remote_sdp,
		CryptoOffer& crypto_offer) {

	int i, j;
	int attr_count, media_count;
	pjmedia_sdp_attr *attribute;
	pjmedia_sdp_media *media;

	// get the number of media for this sdp session
	media_count = remote_sdp->media_count;

	CryptoOffer remoteOffer;

	// iterate over all media
	for (i = 0; i < media_count; ++i) {

		// get media
		media = remote_sdp->media[i];

		// get number of attribute for this memdia
		attr_count = media->attr_count;

		// iterate over all attribute for this media
		for (j = 0; j < attr_count; j++) {

			attribute = media->attr[j];

			// test if this attribute is a crypto
			if (pj_stricmp2(&attribute->name, "crypto") == 0) {

				std::string attr(attribute->value.ptr, attribute->value.slen);

				// @TODO our parser require the "a=crypto:" to be present
				std::string full_attr = "a=crypto:";
				full_attr += attr;

				crypto_offer.push_back(full_attr);
			}

		}
	}

}

Sdp::Sdp(pj_pool_t *pool) :
	_initialMedias(), _negotiatedMedias(0), _negotiator(NULL), _ip_addr(""),
			_local_offer(NULL), _negotiated_offer(NULL), _pool(NULL),
			_publishedAudioPort(0) {
	_pool = pool;
}

Sdp::~Sdp() {
	// cleanSessionMedia();
	// cleanLocalMediaCapabilities();
}

std::string Sdp::toString(void) {
	std::ostringstream sdp;
	int count, i;

	sdp << "origin= " << _local_offer->origin.user.ptr << "\n";
	sdp << "origin.id= " << _local_offer->origin.id << "\n";
	sdp << "origin.version= " << _local_offer->origin.version << "\n";
	sdp << "origin.net_type= " << _local_offer->origin.net_type.ptr << "\n";
	sdp << "origin.addr_type= " << _local_offer->origin.addr_type.ptr << "\n";

	sdp << "name=" << _local_offer->name.ptr << "\n";

	sdp << "conn.net_type=" << _local_offer->conn->net_type.ptr << "\n";
	sdp << "conn.addr_type=" << _local_offer->conn->addr_type.ptr << "\n";
	sdp << "conn.addr=" << _local_offer->conn->addr.ptr << "\n";

	sdp << "start=" << _local_offer->time.start << "\n";
	sdp << "stop=" << _local_offer->time.stop << "\n";

	sdp << "attr_count=" << _local_offer->attr_count << "\n";
	sdp << "media_count=" << _local_offer->media_count << "\n";
	sdp << "m=" << _local_offer->media[0]->desc.media.ptr << " ";
	sdp << _local_offer->media[0]->desc.port << " ";
	sdp << _local_offer->media[0]->desc.transport.ptr << " ";
	count = _local_offer->media[0]->desc.fmt_count;

	for (i = 0; i < count; i++) {
		sdp << _local_offer->media[0]->desc.fmt[i].ptr << " ";
	}

	return sdp.str();
}
