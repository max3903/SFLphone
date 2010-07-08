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

#include "GstEncoderH264.h"

namespace sfl {

void GstEncoderH264::setMaxMbps(const std::string& maxMbps) {
	_warn("max-mbps property not taken into account");
}

void GstEncoderH264::setMaxFs(const std::string& maxFs) {
	_warn("max-fs property not taken into account");
}

void GstEncoderH264::setMaxCpb(const std::string& maxCpb) {
	_warn("max-cpb property not taken into account");
}

void GstEncoderH264::setMaxDpb(const std::string& maxDpb) {
	_warn("max-dpb property not taken into account");
}

void GstEncoderH264::setMaxBr(const std::string& maxBr) {
	_warn("max-br property not taken into account");
}

void GstEncoderH264::setRedundantPicCap(const std::string& redundantPicCap) {
	_warn("redundant-pic-cap property not taken into account");
}

void GstEncoderH264::setParameterAdd(const std::string& parameterAdd) {
	_warn("parameter-add property not taken into account");
}

void GstEncoderH264::setDeintBufCap(const std::string& deintBufCap) {
	_warn("deint-buf-cap property not taken into account");
}

void GstEncoderH264::setMaxRcmdNaluSize(const std::string& maxRcmdNaluSize) {
	_warn("max-rcmd-nalu-size property not taken into account");
}

void GstEncoderH264::setSpropInterleavingDepth(
		const std::string& spropInterleavingDepth) {
	_warn("sprop-interleaving-depth property not taken into account");
}

void GstEncoderH264::setSpropDeintBufReq(const std::string& spropDeintBufReq) {
	_warn("sprop-deint-buf-req property not taken into account");
}

void GstEncoderH264::setSpropInitBufTime(const std::string& spropInitBufTime) {
	_warn("sprop-init-buf-time property not taken into account");
}

void GstEncoderH264::setSpropMaxDonDiff(const std::string& spropMaxDonDiff) {
	_warn("sprop-max-don-diff property not taken into account");
}

void GstEncoderH264::setSpropParameterSets(
		const std::string& spropParameterSets) {
	_info("Setting sprop-parameter-sets property");
	g_object_set(G_OBJECT(rtph264pay), "sprop-parameter-sets",
			spropParameterSets.c_str(), NULL);
}

void GstEncoderH264::setPacketizationMode(
		const std::string& packetizationMode) {
	_info("Setting packetization-mode property");
	g_object_set(G_OBJECT(rtph264pay), "scan-mode", packetizationMode.c_str(),
			NULL);
}

void GstEncoderH264::setProfileLevelId(const std::string& profileLevelId) {
	g_object_set(G_OBJECT(rtph264pay), "profile-level-id",
			profileLevelId.c_str(), NULL);
}

std::string GstEncoderH264::getCodecName() {
	return "H264";
}

void GstEncoderH264::buildEncodingFilter(Pipeline& pipeline, GstElement* previous)
		throw (VideoDecodingException, MissingPluginException) {
	x264enc = pipeline.addElement("x264enc", previous);
	// Generate byte stream format of NALU
	g_object_set(G_OBJECT(x264enc), "byte-stream", TRUE, NULL);

	// Enable automatic multithreading
	g_object_set(G_OBJECT(x264enc), "threads", 0, NULL);

	g_object_set(G_OBJECT(x264enc), "bitrate", 300, NULL); // FIXME Hardcoded

	rtph264pay = pipeline.addElement("rtph264pay", x264enc);
}

GstElement* GstEncoderH264::getTail() {
	return rtph264pay;
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

void GstEncoderH264::setProperty(int index, const std::string& value) {
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
	case MAX_FS:
	case MAX_CPB:
	case MAX_DPB:
	case MAX_BR:
	case REDUNDANT_PIC_CAP:
	case PARAMETER_ADD:
	case DEINT_BUF_CAP:
	case MAX_RCMP_NALU_SIZE:
	case SPROP_INTERLEAVING_DEPTH:
	case SPROP_DEINT_BUF_REQ:
	case SPROP_INIT_BUF_TIME:
	case SPROP_MAX_DON_DIFF:
	default:
		break;
	};
}

void GstEncoderH264::init() throw(VideoDecodingException, MissingPluginException) {
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

GstEncoderH264::GstEncoderH264(VideoInputSource& source)
		throw (VideoDecodingException, MissingPluginException) :
	GstEncoder(source) {
	init();
}

GstEncoderH264::GstEncoderH264(VideoInputSource& source,
		unsigned maxFrameQueued) throw (VideoDecodingException,
		MissingPluginException) :
	GstEncoder(source, maxFrameQueued) {
	init();
}

}
