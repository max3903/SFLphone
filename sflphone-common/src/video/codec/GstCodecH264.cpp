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

#include "GstCodecH264.h"
#include "logger.h"

namespace sfl {

std::string GstCodecH264::getMimeSubtype() {
	return "H264";
}

void GstCodecH264::setMaxMbps(const std::string& maxMbps) {
	_warn("max-mbps property not taken into account");
}

void GstCodecH264::setMaxFs(const std::string& maxFs) {
	_warn("max-fs property not taken into account");
}

void GstCodecH264::setMaxCpb(const std::string& maxCpb) {
	_warn("max-cpb property not taken into account");
}

void GstCodecH264::setMaxDpb(const std::string& maxDpb) {
	_warn("max-dpb property not taken into account");
}

void GstCodecH264::setMaxBr(const std::string& maxBr) {
	_warn("max-br property not taken into account");
}

void GstCodecH264::setRedundantPicCap(const std::string& redundantPicCap) {
	_warn("redundant-pic-cap property not taken into account");
}

void GstCodecH264::setParameterAdd(const std::string& parameterAdd) {
	_warn("parameter-add property not taken into account");
}

void GstCodecH264::setDeintBufCap(const std::string& deintBufCap) {
	_warn("deint-buf-cap property not taken into account");
}

void GstCodecH264::setMaxRcmdNaluSize(const std::string& maxRcmdNaluSize) {
	_warn("max-rcmd-nalu-size property not taken into account");
}

void GstCodecH264::setSpropInterleavingDepth(
		const std::string& spropInterleavingDepth) {
	_warn("sprop-interleaving-depth property not taken into account");
}

void GstCodecH264::setSpropDeintBufReq(const std::string& spropDeintBufReq) {
	_warn("sprop-deint-buf-req property not taken into account");
}

void GstCodecH264::setSpropInitBufTime(const std::string& spropInitBufTime) {
	_warn("sprop-init-buf-time property not taken into account");
}

void GstCodecH264::setSpropMaxDonDiff(const std::string& spropMaxDonDiff) {
	_warn("sprop-max-don-diff property not taken into account");
}

void GstCodecH264::setSpropParameterSets(const std::string& spropParameterSets) {
	_warn("sprop-max-don-diff property not taken into account");
}

void GstCodecH264::setPacketizationMode(const std::string& packetizationMode) {
	_warn("sprop-max-don-diff property not taken into account");
}

void GstCodecH264::setProfileLevelId(const std::string& profileLevelId) {
	_warn("sprop-max-don-diff property not taken into account");
}

enum {
	PROFILE_LEVEL_ID,
	MAX_MBPS,
	MAX_FS,
	MAX_CPB,
	MAX_DPB,
	MAX_BR,
	REDUNDANT_PIC_CAP,
	PARAMETER_ADD,
	PACKETIZATION_MODE,
	DEINT_BUF_CAP,
	MAX_RCMP_NALU_SIZE,
	SPROP_PARAMETER_SETS,
	SPROP_INTERLEAVING_DEPTH,
	SPROP_DEINT_BUF_REQ,
	SPROP_INIT_BUF_TIME,
	SPROP_MAX_DON_DIFF
};

void GstCodecH264::setProperty(int index, const std::string& value) {
	switch (index) {
	case SPROP_PARAMETER_SETS:
		setSpropParameterSets(value);
		break;
	case PACKETIZATION_MODE:
		setPacketizationMode(value);
		break;
	case PROFILE_LEVEL_ID:
		setProfileLevelId(value);
		break;
	case MAX_MBPS:
		setMaxMbps(value);
		break;
	case MAX_FS:
		setMaxFs(value);
		break;
	case MAX_CPB:
		setMaxCpb(value);
		break;
	case MAX_DPB:
		setMaxDpb(value);
		break;
	case MAX_BR:
		setMaxBr(value);
		break;
	case REDUNDANT_PIC_CAP:
		setRedundantPicCap(value);
		break;
	case PARAMETER_ADD:
		setParameterAdd(value);
		break;
	case DEINT_BUF_CAP:
		setDeintBufCap(value);
		break;
	case MAX_RCMP_NALU_SIZE:
		setMaxRcmdNaluSize(value);
		break;
	case SPROP_INTERLEAVING_DEPTH:
		setSpropInterleavingDepth(value);
		break;
	case SPROP_DEINT_BUF_REQ:
		setSpropDeintBufReq(value);
		break;
	case SPROP_INIT_BUF_TIME:
		setSpropInitBufTime(value);
		break;
	case SPROP_MAX_DON_DIFF:
		setSpropMaxDonDiff(value);
		break;
	default:
		_error("Property %d is not recognized.", index);
		break;
	};
}

void GstCodecH264::init() throw(VideoDecodingException, MissingPluginException) {
	installProperty("profile-level-id", PROFILE_LEVEL_ID);
	installProperty("max-mbps", MAX_MBPS);
	installProperty("max-fs", MAX_FS);
	installProperty("max-cpb", MAX_CPB);
	installProperty("max-dpb", MAX_DPB);
	installProperty("max-br", MAX_BR);
	installProperty("redundant-pic-cap", REDUNDANT_PIC_CAP);
	installProperty("parameter-add", PARAMETER_ADD);
	installProperty("packetization-mode", PACKETIZATION_MODE);
	installProperty("deint-buf-cap", DEINT_BUF_CAP);
	installProperty("max-rcmd-nalu-size", MAX_RCMP_NALU_SIZE);
	installProperty("sprop-parameter-sets", SPROP_PARAMETER_SETS);
	installProperty("sprop-interleaving-depth", SPROP_INTERLEAVING_DEPTH);
	installProperty("sprop-deint-buf-req", SPROP_DEINT_BUF_REQ);
	installProperty("sprop-init-buf-time", SPROP_INIT_BUF_TIME);
	installProperty("sprop-max-don-diff", SPROP_MAX_DON_DIFF);
}

GstCodecH264::GstCodecH264() : AbstractVideoCodec<GstEncoderH264, GstDecoderH264>() {
	init();
}

}
