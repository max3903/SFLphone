/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
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
#ifndef __SIPCALL_H__
#define __SIPCALL_H__

#include "call.h"

#include "video/source/VideoFormat.h"

#include <pjsip-simple/evsub.h>
#include <pjsip_ua.h>

/**
 * Forward declarations
 */
class AudioRtp;
class AudioCodec;
class Sdp;
class SIPAccount;

namespace sfl {
class AudioRtpFactory;
class VideoDevice;
}

/**
 * @file sipcall.h
 * @brief SipCall are SIP implementation of a normal Call
 */
class SipCall: public Call {
public:
	/**
	 * Constructor
	 * @param id The call id.
	 * @param type  The type of the call. Could be Incoming or Outgoing
	 * @param account The account under which this call is placed.
	 */
	SipCall(CallId id, Call::CallType type, SIPAccount* account);

	/**
	 * Constructor
	 * @param type  The type of the call. Could be Incoming or Outgoing
	 * @param account The account under which this call is placed.
	 */
	SipCall(Call::CallType type, SIPAccount* account);

	/**
	 * Destructor
	 */
	~SipCall();

	/**
	 * Call Identifier
	 * @return int  SIP call id
	 */
	int getCid();

	/**
	 * Call Identifier
	 * @param cid SIP call id
	 */
	void setCid(int cid);

	/**
	 * Domain identifier
	 * @return int  SIP domain id
	 */
	int getDid();

	/**
	 * Domain identifier
	 * @param did SIP domain id
	 */
	void setDid(int did);

	/**
	 * Transaction identifier
	 * @return int  SIP transaction id
	 */
	int getTid();

	/**
	 * Transaction identifier
	 * @param tid SIP transaction id
	 */
	void setTid(int tid);

	void setXferSub(pjsip_evsub* sub);

	pjsip_evsub *getXferSub();

	/**
	 * @param inv The invite session structure used by pjsip.
	 */
	void setInvSession(pjsip_inv_session* inv);

	/**
	 * @return the invite session structure.
	 */
	pjsip_inv_session* getInvSession();

	/**
	 * @Override
	 */
	void setPublishedAudioPort(unsigned int port);

	/**
	 * @Override
	 */
	void setPublishedVideoPort(unsigned int port);

	/**
	 * @return The SDP session object used for this call.
	 */
	Sdp* getLocalSDP(void);

	/** Returns a pointer to the AudioRtp object */
	sfl::AudioRtpFactory* getAudioRtp(void);

	/**
	 * @return true If the user has set a video device to offer in the upcoming session.
	 */
	bool isVideoEnabled();

	/**
	 * @param device The video device that the user has chosen for this call.
	 */
	void setVideoDevice(const std::string& device);

	/**
	 * @return The video device to use for a video call.
	 */
	std::string getVideoDevice();

	/**
	 * @param format The video format to use.
	 */
	void setVideoFormat(sfl::VideoFormat format);

	/**
	 * @return The video format chosen for the source device.
	 */
	sfl::VideoFormat getVideoFormat();

	/**
	 * Set the token obtained while accessing the video ressource.
	 * @param token A valid token obtained while opening a video device.
	 */
	void setVideoToken(const std::string& token);

	/**
	 * @return The video token set for this call, or the empty string if none was set.
	 */
	std::string getVideoToken();

private:
    /**
     * Helper function for constructor.
     */
	void init(Call::CallType type, SIPAccount* account);

	int _cid;
	int _did;
	int _tid;

	bool _videoEnabled;

	// Copy Constructor
	SipCall(const SipCall& rh);

	// Assignment Operator
	SipCall& operator=(const SipCall& rh);

	std::string _videoDevice;

	sfl::VideoFormat _videoFormat;

	std::string _videoToken;

	Sdp* _sdpSession;

	SIPAccount* _account;

	sfl::AudioRtpFactory* _audiortp;

	pjsip_evsub* _xferSub;

	pjsip_inv_session* _invSession;
};

#endif
