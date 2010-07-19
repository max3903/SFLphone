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
#include "Codec.h"
#include "global.h"
#include "manager.h"

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

Sdp::Sdp(pj_pool_t *pool) :
	_local_media_cap(), _session_media(0), _negotiator(NULL), _ip_addr(""),
			_local_offer(NULL), _negociated_offer(NULL), _pool(NULL),
			_local_extern_audio_port(0) {
	_pool = pool;
}

Sdp::~Sdp() {
	// cleanSessionMedia();
	// cleanLocalMediaCapabilities();
}

void Sdp::setMediaDescriptorLine(SdpMedia *media, pjmedia_sdp_media** p_med) {
	pjmedia_sdp_media* med;
	pjmedia_sdp_rtpmap rtpmap;
	pjmedia_sdp_attr *attr;
	sfl::Codec* codec;
	int count, i;
	std::string tmp;

	med = PJ_POOL_ZALLOC_T(_pool, pjmedia_sdp_media);

	// Set the MIME type for this codec
	pj_strdup (_pool, &med->desc.media, (media->getMediaType() == MIME_TYPE_AUDIO) ? &STR_AUDIO : &STR_VIDEO);

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
	count = media->getMediaCodecList().size();
	med->desc.fmt_count = count;

	// Add the payload list
	for (i = 0; i < count; i++) {
		codec = media->getMediaCodecList()[i]; // FIXME is copying the whole array every time.
		tmp = this->inToString(codec->getPayloadFormat().getPayloadType());
		_debug ("%s", tmp.c_str());
		pj_strdup2(_pool, &med->desc.fmt[i], tmp.c_str());

		// Add a rtpmap field for each codec
		// We could add one only for dynamic payloads because the codecs with static RTP payloads
		// are entirely defined in the RFC 3351, but if we want to add other attributes like an asymmetric
		// connection, the rtpmap attribute will be useful to specify for which codec it is applicable
		rtpmap.pt = med->desc.fmt[i];
		rtpmap.enc_name = pj_str((char*) codec->getMimeSubtype().c_str());

		// G722 require G722/8000 media description even if it is 16000 codec
		if (codec->getPayloadFormat().getPayloadType() == 9) {
			rtpmap.clock_rate = 8000;
		} else {
			rtpmap.clock_rate = codec->getPayloadFormat().getRTPClockRate();
		}

		// Add the channel number only if different from 1
		if (codec->getMimeType() == "audio") {
			AudioCodec* audioCodec = static_cast<AudioCodec*>(codec);
			if (audioCodec->getChannel() > 1)
				rtpmap.param = pj_str((char*) audioCodec->getChannel());
			else
				rtpmap.param.slen = 0;
		}

		pjmedia_sdp_rtpmap_to_attr(_pool, &rtpmap, &attr);

		med->attr[med->attr_count++] = attr;
	}

	// Add the direction stream
	attr = (pjmedia_sdp_attr*) pj_pool_zalloc(_pool, sizeof(pjmedia_sdp_attr));

	pj_strdup2(_pool, &attr->name, media->getStreamDirectionStr().c_str());

	med->attr[med->attr_count++] = attr;

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

int Sdp::createLocalOffer(CodecOrder selectedCodecs) {
	_info("SDP: Create local offer");

	// Build local media capabilities
	setLocalMediaCapabilities(selectedCodecs);

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

int Sdp::createInitialOffer(CodecOrder selectedCodecs) {

	pj_status_t status;
	pjmedia_sdp_neg_state state;

	_info("SDP: Create initial offer");
	// Build the SDP session descriptor
	status = createLocalOffer(selectedCodecs);

	if (status != PJ_SUCCESS) {
		_error ("SDP: Error: Failed to create initial offer");
		return status;
	}

	// Create the SDP negotiator instance with local offer
	status = pjmedia_sdp_neg_create_w_local_offer(_pool, getLocalSdpSession(),
			&_negotiator);

	if (status != PJ_SUCCESS) {
		_error ("SDP: Error: Failed to create an initial SDP negotiator");
		return status;
	}

	state = pjmedia_sdp_neg_get_state(_negotiator);

	PJ_ASSERT_RETURN (status == PJ_SUCCESS, 1);

	return PJ_SUCCESS;
}

int Sdp::receivingInitialOffer(pjmedia_sdp_session* remote,
		CodecOrder selectedCodecs) {

	// Create the SDP negotiator instance by calling
	// pjmedia_sdp_neg_create_w_remote_offer with the remote offer, and by providing the local offer ( optional )

	pj_status_t status;

	if (!remote) {
		return !PJ_SUCCESS;
	}

	// Create the SDP negotiator instance by calling
	// pjmedia_sdp_neg_create_w_remote_offer with the remote offer, and by providing the local offer ( optional )

	// Build the local offer to respond
	status = createLocalOffer(selectedCodecs);

	if (status != PJ_SUCCESS) {
		_error ("SDP: Error: Failed to create initial offer");
		return status;
	}

	// Retrieve some useful remote information
	this->setMediaTransportFromRemoteSdp(remote);

	status = pjmedia_sdp_neg_create_w_remote_offer(_pool, getLocalSdpSession(),
			remote, &_negotiator);

	PJ_ASSERT_RETURN (status == PJ_SUCCESS, 1);

	return PJ_SUCCESS;
}

pj_status_t Sdp::checkSdpAnswer(pjsip_inv_session *inv, pjsip_rx_data *rdata) {
	static const pj_str_t str_application = { (char*) "application", 11 };
	static const pj_str_t str_sdp = { (char*) "sdp", 3 };
	pj_status_t status;
	pjsip_msg * message = NULL;
	pjmedia_sdp_session * remote_sdp = NULL;

	if (pjmedia_sdp_neg_get_state(inv->neg)
			== PJMEDIA_SDP_NEG_STATE_LOCAL_OFFER) {

		message = rdata->msg_info.msg;

		if (message == NULL) {
			_error ("SDP: No message");
			return PJMEDIA_SDP_EINSDP;
		}

		if (message->body == NULL) {
			_error ("SDP: Empty message body");
			return PJMEDIA_SDP_EINSDP;
		}

		if (pj_stricmp(&message->body->content_type.type, &str_application)
				|| pj_stricmp(&message->body->content_type.subtype, &str_sdp)) {
			_error ("SDP: Incoming Message does not contain SDP");
			return PJMEDIA_SDP_EINSDP;
		}

		// Parse the SDP body.
		status = pjmedia_sdp_parse(rdata->tp_info.pool,
				(char*) message->body->data, message->body->len, &remote_sdp);

		if (status == PJ_SUCCESS) {
			status = pjmedia_sdp_validate(remote_sdp);
		}

		if (status != PJ_SUCCESS) {
			_warn ("SDP: cannot be validated");
			return PJMEDIA_SDP_EINSDP;
		}

		// This is an answer
		_debug ("SDP: Got SDP answer %s", pjsip_rx_data_get_info (rdata));

		status = pjmedia_sdp_neg_set_remote_answer(inv->pool, inv->neg,
				remote_sdp);

		if (status != PJ_SUCCESS) {
			_error ("SDP: Error: while processing remote answer %s", pjsip_rx_data_get_info (rdata));
			return PJMEDIA_SDP_EINSDP;
		}

		// Prefer our codecs to remote when possible
		pjmedia_sdp_neg_set_prefer_remote_codec_order(inv->neg, 0);

		status = pjmedia_sdp_neg_negotiate(inv->pool, inv->neg, 0);

		_debug ("Negotiation returned with status %d PJ_SUCCESS being %d", status, PJ_SUCCESS);
	} else {
		_debug ("No need to check sdp answer since we are UAS");
		return PJ_SUCCESS;
	}

	return status;
}

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
	int nb_media, i;

	med = PJ_POOL_ZALLOC_T (_pool, pjmedia_sdp_media);
	nb_media = getLocalMediaCap().size();
	this->_local_offer->media_count = nb_media;

	for (i = 0; i < nb_media; i++) {
		setMediaDescriptorLine(getLocalMediaCap()[i], &med);
		this->_local_offer->media[i] = med;
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

std::string Sdp::mediaToString(void) {
	int size, i;
	std::ostringstream res;

	size = _local_media_cap.size();

	for (i = 0; i < size; i++) {
		res << _local_media_cap[i]->toString();
	}

	res << std::endl;

	return res.str();
}

void Sdp::cleanSessionMedia() {
	_info("SDP: Clean session media");

	if (_session_media.size() > 0) {

		std::vector<SdpMedia *>::iterator iter = _session_media.begin();
		SdpMedia *media;

		while (iter != _session_media.end()) {
			media = *iter;
			delete media;
			iter++;
		}
		_session_media.clear();
	}
}

void Sdp::cleanLocalMediaCapabilities() {
	_info("SDP: Clean local media capabilities");

	if (_local_media_cap.size() > 0) {

		std::vector<SdpMedia *>::iterator iter = _local_media_cap.begin();
		SdpMedia *media;

		while (iter != _local_media_cap.end()) {
			media = *iter;
			delete media;
			iter++;
		}
		_local_media_cap.clear();
	}
}

void Sdp::setNegotiatedSdp(const pjmedia_sdp_session *sdp) {

	int nb_media, nb_codecs;
	int i, j, port;
	pjmedia_sdp_media *current;
	SdpMedia *media;
	std::string type, dir;

	pjmedia_sdp_attr *attribute = NULL;
	pjmedia_sdp_rtpmap *rtpmap;

	_negociated_offer = (pjmedia_sdp_session*) sdp;

	CodecFactory& factory = Manager::instance().getCodecFactory();

	// retrieve the media information
	nb_media = _negociated_offer->media_count;

	for (i = 0; i < nb_media; i++) {
		// Retrieve the media
		current = _negociated_offer->media[i];
		type = current->desc.media.ptr;
		port = current->desc.port;
		media = new SdpMedia(type, port);

		// Retrieve the payload
		nb_codecs = current->desc.fmt_count;

		for (j = 0; j < nb_codecs; j++) {
			attribute = pjmedia_sdp_media_find_attr(current, &STR_RTPMAP, NULL);

			if (!attribute) {
				_error("a=rtpmap cannot be found in SDP.");
				return;
			}

			pjmedia_sdp_attr_to_rtpmap(_pool, attribute, &rtpmap);
			AudioCodec* codec = factory.getCodec((AudioCodecType) pj_strtoul(&rtpmap->pt));
			if (codec != NULL) {
				media->addCodec(codec);
			}
		}

		_session_media.push_back(media);
	}
}

sfl::Codec* Sdp::getSessionMedia(void) {
	int nb_media;
	int nb_codec;
	sfl::Codec *codec = NULL;
	std::vector<SdpMedia*> media_list;

	_debug ("SDP: Executing sdp line %d - get_session_media()", __LINE__);

	media_list = getSessionMediaList();
	nb_media = media_list.size();

	if (nb_media > 0) {
		nb_codec = media_list[0]->getMediaCodecList().size();

		if (nb_codec > 0) {
			codec = media_list[0]->getMediaCodecList()[0];
		}
	}

	return codec;
}

pj_status_t Sdp::startNegotiation() {
	pj_status_t status;

	if (_negotiator) {
		status = pjmedia_sdp_neg_negotiate(_pool, _negotiator, 0);
	} else {
		status = !PJ_SUCCESS;
	}

	return status;
}

void Sdp::toString(void) {

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

	sdp << "\n";

	_debug ("LOCAL SDP: \n%s", sdp.str().c_str());
}

void Sdp::setLocalMediaCapabilities(CodecOrder selectedCodecs) {

	_local_media_cap.clear();

	_debug ("SDP: Fetch local media capabilities. Published audio port: %i" , getPublishedAudioPort());

	// Create new SDP media of type "audio".
	SdpMedia *audio;
	audio = new SdpMedia(MIME_TYPE_AUDIO);
	audio->setPort(getPublishedAudioPort());

	// Add selected audio codec to the SDP media.
	CodecFactory& factory = Manager::instance().getCodecFactory();
	int i;
	for (i = 0; i < selectedCodecs.size(); i++) {
		AudioCodec* codec = factory.getCodec(selectedCodecs[i]);
		if (codec != NULL) {
			audio->addCodec(codec);
		}
	}

	_local_media_cap.push_back(audio);
}

void Sdp::setPortOnAllMedia(int port) {
	std::vector<SdpMedia*> medias;
	int i, size;

	setPublishedAudioPort(port);

	medias = getLocalMediaCap();
	size = medias.size();

	for (i = 0; i < size; i++) {
		medias[i]->setPort(port);
	}
}

std::string Sdp::inToString(int value) {
	std::ostringstream result;
	result << value;
	return result.str();
}

void Sdp::setRemoteIpFromSdp(const pjmedia_sdp_session *r_sdp) {

	std::string remote_ip(r_sdp->conn->addr.ptr, r_sdp->conn->addr.slen);
	_info ("SDP: Remote IP from fetching SDP: %s", remote_ip.c_str());
	this->setRemoteIp(remote_ip);
}

void Sdp::setRemoteAudioPortFromSdp(pjmedia_sdp_media *r_media) {

	int remote_port;

	remote_port = r_media->desc.port;
	_info ("SDP: Remote Audio Port from fetching SDP: %d", remote_port);
	this->setRemoteAudioPort(remote_port);
}

void Sdp::setMediaTransportFromRemoteSdp(const pjmedia_sdp_session *remote_sdp) {

	_info ("SDP: Fetching media from sdp");

	if (!remote_sdp)
		return;

	pjmedia_sdp_media *r_media;

	this->getRemoteSdpMediaFromOffer(remote_sdp, &r_media);

	if (r_media == NULL) {
		_warn ("SDP: Error: no remote sdp media found in the remote offer");
		return;
	}

	this->setRemoteAudioPortFromSdp(r_media);

	this->setRemoteIpFromSdp(remote_sdp);

}

void Sdp::getRemoteSdpMediaFromOffer(
		const pjmedia_sdp_session* remote_sdp, pjmedia_sdp_media** r_media) {
	int count, i;

	if (!remote_sdp)
		return;

	count = remote_sdp->media_count;
	*r_media = NULL;

	for (i = 0; i < count; ++i) {
		if (pj_stricmp2(&remote_sdp->media[i]->desc.media, "audio") == 0) {
			*r_media = remote_sdp->media[i];
			return;
		}
	}
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

