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

#include "video/depayloader/VideoDepayloader.h"

#include "util/Observer.h"
#include "util/AbstractObservable.h"

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
class VideoRtpSession : public AbstractObservable<Buffer<uint8_t>&, VideoFrameDecodedObserver>, public ost::Thread {
public:
	/**
	 * @param mutiCastAddress A multicast address.
	 * @param port The port to work on.
	 */
	VideoRtpSession(ost::InetMcastAddress& multiCastAddress, ost::tpport_t port);

	/**
	 * @param mutiCastAddress An inet address.
	 * @param port The port to work on.
	 */
	VideoRtpSession(ost::InetHostAddress& ia, ost::tpport_t port);

	~VideoRtpSession();

	/**
	 * Change the configuration of the object based on the information contained in
	 * the SDP "a=rtpmap" and "a=fmtp" attributes.
	 * @param rtpmap The Rtmap object object holding the information of a "a=rtpmap" line of SDP.
	 * @param fmtp The Fmtp object holding the information of a "a=fmtp" line of SDP for some corresponding "a=rtpmap".
	 */
	void configureFromSdp(const RtpMap& rtpmap, const Fmtp& fmtp);

	/**
	 * Register a given depayloader and decoder pair for a given MIME media type.
	 * TODO integrate with the plugin manager.
	 * @precondition The given decoding unit corresponding to the given MIME type must not be present in the table.
	 * @postcondition The decoding unit corresponding to the MIME type will be used if appropriate.
	 */
	void registerDecoder(const std::string& mime, VideoDepayloader& depayloader, VideoDecoder& decoder);

	/**
	 * Register a given depayloader for a given MIME media type.
	 * It is assumed that that depayloader is already configured to send its data to some decoder.
	 * @see sfl#Depayloader#setDecoder
	 */
	void registerDecoder(const std::string& mime, VideoDepayloader& depayloader);


	/**
	 * @param mime The mime type corresponding to the decoding unit to remove.
	 */
	void unregisterDecoder(const std::string mime);

	static const int SCHEDULING_TIMEOUT = 10000;
	static const int EXPIRE_TIMEOUT = 1000000;

protected:
	/**
	 * Simple dispatch for the VideoFrameDecodedObserver type.
	 * @Override
	 */
	void notify(VideoFrameDecodedObserver* observer, Buffer<uint8_t>& data);

	/**
	 * @Override
	 */
	void run();

private:
	/**
	 * Helper method to avoid code duplications with different constructors.
	 */
	void init();

	unsigned clockRate;
	ost::RTPSession* session;
	ost::PayloadType payloadType;
	VideoDepayloader* depayloader;

	std::map<std::string, VideoDepayloader*> decoderTable;
	typedef std::map<std::string, VideoDepayloader*>::iterator DecoderTableIterator;
};
}
#endif
