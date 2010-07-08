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

#include "video/encoder/H264GstEncoder.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"

namespace sfl {

enum SetterIndex {
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

H264GstEncoder::H264GstEncoder(VideoInputSource& source,
		unsigned maxFrameQueued) throw (VideoDecodingException,
		MissingPluginException) :
	VideoEncoder(source) {
	init(source, maxFrameQueued);
}

H264GstEncoder::H264GstEncoder(VideoInputSource& source)
		throw (VideoDecodingException, MissingPluginException) :
	VideoEncoder(source) {
	init(source, MAX_FRAME_QUEUED);
}

void H264GstEncoder::init(VideoInputSource& source, unsigned maxFrameQueued)
		throw (VideoDecodingException, MissingPluginException) {
	gst_init(0, NULL);

	Pipeline pipeline("sfl_h264_encoding");
	pipeline.setPrefix("sfl_encoder_");

	GstElement* ffmpegcolorspace = pipeline.addElement("ffmpegcolorspace");
	GstElement* previous = pipeline.addElement("videoscale", ffmpegcolorspace);

	GstElement* x264enc = pipeline.addElement("x264enc", previous);
	// Generate byte stream format of NALU
	g_object_set(G_OBJECT(x264enc), "byte-stream", TRUE, NULL);
	// Enable automatic multithreading
	g_object_set(G_OBJECT(x264enc), "threads", 0, NULL);
	g_object_set(G_OBJECT(x264enc), "bitrate", 300, NULL); // FIXME Hardcoded

	rtph264pay = pipeline.addElement("rtph264pay", x264enc);

	// Add injectable endpoint
	VideoFormat format = source.getOutputFormat();

	GstCaps* sourceCaps = gst_caps_new_simple(
			"video/x-raw-rgb", // FIXME Hardcoded !
			"format", GST_TYPE_FOURCC, GST_STR_FOURCC(
					format.getFourcc().c_str()), "width", G_TYPE_INT,
			format.getWidth(), "height", G_TYPE_INT, format.getHeight(),
			"bpp", G_TYPE_INT, 32, // FIXME Hardcoded !,
			"depth", G_TYPE_INT, 32, // FIXME Hardcoded !
			"endianness", G_TYPE_INT, 4321,
			"red_mask", G_TYPE_INT, 65280,
			"green_mask", G_TYPE_INT, 16711680,
			"blue_mask", G_TYPE_INT, -16777216,
			"framerate", GST_TYPE_FRACTION,
			format.getPreferredFrameRate().getNumerator(),
			format.getPreferredFrameRate().getDenominator(), NULL);

	injectableEnd = new InjectablePipeline(pipeline, sourceCaps, format.getWidth() * format.getHeight() * 32 * maxFrameQueued);  // FIXME Hardcoded

	// Add retrievable endpoint
	retrievableEnd = new RetrievablePipeline(pipeline);
	outputObserver = new PipelineEventObserver(this);
	retrievableEnd->addObserver(outputObserver);

	// Connect both endpoints to the graph.
	injectableEnd->setSink(ffmpegcolorspace);
	retrievableEnd->setSource(rtph264pay);
}

H264GstEncoder::~H264GstEncoder() {
	deactivate();

	delete retrievableEnd;
	delete injectableEnd;
}

void H264GstEncoder::encode(const VideoFrame* frame)
		throw (VideoEncodingException) {
	GstBuffer* buffer = gst_buffer_new();
	GST_BUFFER_SIZE(buffer) = frame->getSize();
	GST_BUFFER_DATA(buffer) = (guint8*) frame->getFrame();

	_info("Encoding frame of raw size %d", GST_BUFFER_SIZE(buffer));

	injectableEnd->inject(buffer);
}

void H264GstEncoder::activate() {
	VideoEncoder::activate();

	_info("Activating h264 encoder");

	retrievableEnd->start();
}

void H264GstEncoder::deactivate() {
	VideoEncoder::deactivate();

	_info("Deactivating h264 encoder");

	clearObservers();

	retrievableEnd->removeObserver(outputObserver);
	retrievableEnd->stop();
}

void H264GstEncoder::setProfileLevelId(const std::string& profileLevelId)
{
	g_object_set(G_OBJECT(rtph264pay), "profile-level-id", profileLevelId.c_str(), NULL);
}

void H264GstEncoder::setMaxMbps(const std::string& maxMbps)
{
	_warn("max-mbps property not taken into account");
}

void H264GstEncoder::setMaxFs(const std::string& maxFs)
{
	_warn("max-fs property not taken into account");
}

void H264GstEncoder::setMaxCpb(const std::string& maxCpb)
{
	_warn("max-cpb property not taken into account");
}

void H264GstEncoder::setMaxDpb(const std::string& maxDpb)
{
	_warn("max-dpb property not taken into account");
}

void H264GstEncoder::setMaxBr(const std::string& maxBr)
{
	_warn("max-br property not taken into account");
}

void H264GstEncoder::setRedundantPicCap(const std::string& redundantPicCap)
{
	_warn("redundant-pic-cap property not taken into account");
}

void H264GstEncoder::setParameterAdd(const std::string& parameterAdd)
{
	_warn("parameter-add property not taken into account");
}

void H264GstEncoder::setPacketizationMode(const std::string& packetizationMode)
{
	_info("Setting packetization-mode property");
	g_object_set(G_OBJECT(rtph264pay), "scan-mode", packetizationMode.c_str(), NULL);
}

void H264GstEncoder::setDeintBufCap(const std::string& deintBufCap)
{
	_warn("deint-buf-cap property not taken into account");
}

void H264GstEncoder::setMaxRcmdNaluSize(const std::string& maxRcmdNaluSize)
{
	_warn("max-rcmd-nalu-size property not taken into account");
}

void H264GstEncoder::setSpropParameterSets(const std::string& spropParameterSets)
{
	_info("Setting sprop-parameter-sets property");
	g_object_set(G_OBJECT(rtph264pay), "sprop-parameter-sets", spropParameterSets.c_str(), NULL);

}

void H264GstEncoder::setSpropInterleavingDepth(const std::string& spropInterleavingDepth)
{
	_warn("sprop-interleaving-depth property not taken into account");
}

void H264GstEncoder::setSpropDeintBufReq(const std::string& spropDeintBufReq)
{
	_warn("sprop-deint-buf-req property not taken into account");
}

void H264GstEncoder::setSpropInitBufTime(const std::string& spropInitBufTime)
{
	_warn("sprop-init-buf-time property not taken into account");
}

void H264GstEncoder::setSpropMaxDonDiff(const std::string& spropMaxDonDiff)
{
	_warn("sprop-max-don-diff property not taken into account");
}

void H264GstEncoder::installProperties() {
	// Would use Lambdas from C++0x, but the old guy that wrote this code was using G++ 4.4.3 at that time (lambdas are in 4.5)
	propertyTable.insert(SetterEntry("profile-level-id", PROFILE_LEVEL_ID));
	propertyTable.insert(SetterEntry("max-mbps", MAX_MBPS));
	propertyTable.insert(SetterEntry("max-fs", MAX_FS));
	propertyTable.insert(SetterEntry("max-cpb", MAX_CPB));
	propertyTable.insert(SetterEntry("max-dpb", MAX_DPB));
	propertyTable.insert(SetterEntry("max-br", MAX_BR));
	propertyTable.insert(SetterEntry("redundant-pic-cap", REDUNDANT_PIC_CAP));
	propertyTable.insert(SetterEntry("parameter-add", PARAMETER_ADD));
	propertyTable.insert(SetterEntry("packetization-mode", PACKETIZATION_MODE));
	propertyTable.insert(SetterEntry("deint-buf-cap", DEINT_BUF_CAP));
	propertyTable.insert(SetterEntry("max-rcmd-nalu-size", MAX_RCMP_NALU_SIZE));
	propertyTable.insert(SetterEntry("sprop-parameter-sets", SPROP_PARAMETER_SETS));
	propertyTable.insert(SetterEntry("sprop-interleaving-depth", SPROP_INTERLEAVING_DEPTH));
	propertyTable.insert(SetterEntry("sprop-deint-buf-req", SPROP_DEINT_BUF_REQ));
	propertyTable.insert(SetterEntry("sprop-init-buf-time", SPROP_INIT_BUF_TIME));
	propertyTable.insert(SetterEntry("sprop-max-don-diff", SPROP_MAX_DON_DIFF));
}

void H264GstEncoder::setProperty(const std::string& name, const std::string& value)
{
	SetterIterator it = propertyTable.find(name);
	if (it == propertyTable.end()) {
		// throw
		_warn("Parameter %s cannot be handled.", name.c_str());
	}

	switch((*it).second) {
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

}
