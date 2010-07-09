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

#ifndef __SFL_GST_CODEC_H264_H__
#define __SFL_GST_CODEC_H264_H__

#include "AbstractVideoCodec.h"
#include "video/encoder/GstEncoderH264.h"
#include "video/decoder/GstDecoderH264.h"

namespace sfl {
class GstCodecH264: public AbstractVideoCodec<GstEncoderH264, GstDecoderH264> {
public:
	GstCodecH264();

	virtual ~GstCodecH264() {};

	std::string getMimeSubtype();

	void setProfileLevelId(const std::string& profileLevelId);

	void setMaxMbps(const std::string& maxMbps);

	void setMaxFs(const std::string& maxFs);

	void setMaxCpb(const std::string& maxCpb);

	void setMaxDpb(const std::string& maxDpb);

	void setMaxBr(const std::string& maxBr);

	void setRedundantPicCap(const std::string& redundantPicCap);

	void setParameterAdd(const std::string& parameterAdd);

	void setPacketizationMode(const std::string& packetizationMode);

	void setDeintBufCap(const std::string& deintBufCap);

	void setMaxRcmdNaluSize(const std::string& maxRcmdNaluSize);

	void setSpropParameterSets(const std::string& spropParameterSets);

	void setSpropInterleavingDepth(const std::string& spropInterleavingDepth);

	void setSpropDeintBufReq(const std::string& spropDeintBufReq);

	void setSpropInitBufTime(const std::string& spropInitBufTime);

	void setSpropMaxDonDiff(const std::string& spropMaxDonDiff);

protected:
	void setProperty(int index, const std::string& value);

private:
	void init() throw(VideoDecodingException, MissingPluginException) ;
};
}

#endif
