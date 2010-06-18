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
#ifndef __SFL_H264_DEPAYLOADER_H__
#define __SFL_H264_DEPAYLOADER_H__

#include "VideoDepayloader.h"

#include "util/BufferBuilder.h"
#include <stdint.h>
#include <stdexcept>
#include <string>

namespace sfl {

/**
 * This exception is thrown when the packetizing mode is unsupported in
 * this implementation.
 * TODO We will probably want to move this exception up in the hierarchy as more depayloaders
 * gets implemented.
 */
class UnsupportedPacketizationMode: public VideoDepayloadingException {
public:
	UnsupportedPacketizationMode(const std::string& msg) :
		VideoDepayloadingException(msg) {
	}
};

/**
 * H264 depayloader. Currently supports FU-A, FU-B, Single NAL mode (1-23).
 * Does not parse SDP info yet.
 */
class H264Depayloader : public VideoDepayloader {
public:
	/**
	 * @param sdp The SDP to be parsed.
	 */
	H264Depayloader(const std::string sdp);
	~H264Depayloader();

	/**
	 * @Override
	 */
	void parseSdp(const std::string& sdp);

	/**
	 * @Override
	 */
	void process(const ost::AppDataUnit* adu) throw(UnsupportedPacketizationMode);

private:
	/**
	 * NAL unit types 1-23 (section 5.6).
	 */
	void handleSingleNalUnitMode(const ost::AppDataUnit* adu);

	/**
	 * This payload type allows fragmenting a NAL unit into several RTP
	 * packets (section 5.8).
	 */
	void handleFragmentationUnit(const ost::AppDataUnit* adu);

	std::string sdpUnparsed;
	BufferBuilder<uint8_t> assemblingBuffer;
};

}


#endif
