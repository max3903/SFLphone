/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author : Laurielle Lea <laurielle.lea@savoirfairelinux.com>
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

#include "sipcall.h"
#include "global.h" // for _debug
#include "sdp/sdp.h"
#include "audio/audiortp/AudioRtpFactory.h"
#include "sipaccount.h"

SipCall::SipCall(CallId id, Call::CallType type, SIPAccount* account) : Call(id, type) {
	init(type, account);
}

SipCall::SipCall(Call::CallType type, SIPAccount* account) :
	Call(type) {
	init(type, account);
}

void SipCall::init(Call::CallType type, SIPAccount* account)
{
	// Init fields
	_cid = 0;
	_did = 0;
	_tid = 0;
	_audiortp = new sfl::AudioRtpFactory();
	_xferSub = NULL;
	_invSession = NULL;
	_sdpSession = 0;
	_account  = account;

	_video_enabled = account->isAlwaysOfferVideo();
	_debug("Always offer video : %d", account->isAlwaysOfferVideo());

	// Create an SDP object.
	SipVoipLink* link = SipVoipLink::instance(account->getAccountID());
	_sdpSession = new Sdp(link->getMemoryPool());

	// Set the IP address
	std::string addrSdp;
	if (account->isStunEnabled()) {
		addrSdp = account->getPublishedAddress();
	} else {
		addrSdp = link->getInterfaceAddrFromName(account->getLocalInterface());
	}

	if (addrSdp == "0.0.0.0") {
		link->getLocalIp(&addrSdp);
	}

	setLocalIp(addrSdp);

	_sdpSession->setIpAddress(addrSdp);

	// Set the audio/video ports
	unsigned int callLocalAudioPort = RANDOM_LOCAL_PORT;
	unsigned int callLocalExternAudioPort = callLocalAudioPort;

	if (account->isStunEnabled()) {
		// If use Stun server
		callLocalExternAudioPort = account->getStunPort();
	}

	// Set the audio ports
	setLocalAudioPort(callLocalAudioPort);
	setPublishedAudioPort(callLocalExternAudioPort);
	_debug ("            Local audio port : %d", callLocalAudioPort);
	_debug ("            Published audio port : %d", callLocalExternAudioPort);

	// Add audio capabilities
	_sdpSession->setLocalMediaCapabilities(MIME_TYPE_AUDIO,
			account->getActiveAudioCodecs());

	if (isVideoEnabled()) {
		// Set the video ports
		setLocalVideoPort(callLocalAudioPort);
		setPublishedVideoPort(callLocalExternAudioPort);
		_debug ("            Local video port : %d", callLocalAudioPort);
		_debug ("            Published video port : %d", callLocalExternAudioPort);

		// Add video capabilities
		_sdpSession->setLocalMediaCapabilities(MIME_TYPE_VIDEO,
				account->getActiveVideoCodecs());
	}

	_sdpSession->createInitialOffer();
}

SipCall::~SipCall() {
	_debug ("SIPCall: Delete call");

	delete _audiortp;
	_audiortp = 0;
	delete _sdpSession;
	_sdpSession = 0;
}

int SipCall::getCid() {
	return _cid;
}

void SipCall::setCid(int cid) {
	_cid = cid;
}

int SipCall::getDid() {
	return _did;
}

void SipCall::setDid(int did) {
	_did = did;
}

int SipCall::getTid() {
	return _tid;
}

void SipCall::setTid(int tid) {
	_tid = tid;
}

void SipCall::setXferSub(pjsip_evsub* sub) {
	_xferSub = sub;
}

pjsip_evsub* SipCall::getXferSub() {
	return _xferSub;
}

void SipCall::setInvSession(pjsip_inv_session* inv) {
	_invSession = inv;
}

pjsip_inv_session* SipCall::getInvSession() {
	return _invSession;
}

void SipCall::setPublishedAudioPort(unsigned int port) {
	Call::setPublishedAudioPort(port);
	_sdpSession->setPublishedAudioPort(port);
}

void SipCall::setPublishedVideoPort(unsigned int port) {
	Call::setPublishedVideoPort(port);
	_sdpSession->setPublishedVideoPort(port);
}

Sdp* SipCall::getLocalSDP(void) {
	return _sdpSession;
}

sfl::AudioRtpFactory* SipCall::getAudioRtp(void) {
	return _audiortp;
}

bool SipCall::isVideoEnabled() {
	return _video_enabled;
}

void SipCall::setVideoDevice(sfl::VideoDevice& device) {
	_videoDevice = &device;
}
