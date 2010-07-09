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

#ifndef __SFL_H264_GST_ENCODER_H__
#define __SFL_H264_GST_ENCODER_H__

#include "util/gstreamer/Pipeline.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"
#include "video/encoder/VideoEncoder.h"

#include <gst/rtp/gstrtpbuffer.h>

#include <map>

namespace sfl {

/**
 * Extends VideoEncoder, and implements RetrievablePipelineObserver
 */
class H264GstEncoder : public VideoEncoder {
public:
	/**
	 * @Override
	 */
	H264GstEncoder(VideoInputSource& source) throw(VideoDecodingException, MissingPluginException);
	/**
	 * @param source The video source from which to capture data from.
	 * @param maxFrameQueued The maximum number of frames to be queued before starting to drop the following ones.
	 * @throw VideoEncodingException if an error occurs while opening the video decoder.
	 */
	H264GstEncoder(VideoInputSource& source, unsigned maxFrameQueued) throw(VideoDecodingException, MissingPluginException);

	~H264GstEncoder();

	/**
	 * @Override
	 */
	void encode(const VideoFrame* frame) throw(VideoEncodingException);

	/**
	 * @Override
	 */
	void activate();

	/**
	 * @Override
	 */
	void deactivate();

	/**
	 * @Override
	 */
	std::string getMimeSubtype();

	/**
	 * @Override
	 */
	void setProperty(const std::string propName, const std::string propValue);

	/**
	 * @Override
	 */
	void setProperty(const std::string& name, const std::string& value);

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

	static const unsigned MAX_FRAME_QUEUED = 10;
private:
	GstElement* rtph264pay;

	/**
	 * Helper method for constructors.
	 */
	void init(VideoInputSource& source, unsigned maxFrameQueued) throw(VideoDecodingException, MissingPluginException);

	InjectablePipeline* injectableEnd;
	RetrievablePipeline* retrievableEnd;

	/**
	 * Observer object for NAL units produced by this encoder.
	 * We only re-broadcast the event externally.
	 */
	class PipelineEventObserver : public RetrievablePipelineObserver {
	public:
		PipelineEventObserver(H264GstEncoder* encoder) : parent(encoder) {}
		H264GstEncoder* parent;
		/**
		 * @Override
		 */
		void onNewBuffer(GstBuffer* buffer) {
			_debug("NAL unit produced at the sink ...");
			GstBuffer* payload = gst_rtp_buffer_get_payload_buffer(buffer);
			uint32 timestamp = gst_rtp_buffer_get_timestamp(buffer);

			uint8* payloadData = GST_BUFFER_DATA(payload);
			uint payloadSize = GST_BUFFER_SIZE(payload);

			std::pair<uint32, Buffer<uint8> > nalUnit(timestamp, Buffer<uint8>(payloadData, payloadSize));

			_debug("Notifying buffer of size %d with timestamp %u", payloadSize, timestamp);
			parent->notifyAll(nalUnit);
		}
	};

	PipelineEventObserver* outputObserver;

	// The following is used for mapping a property name to a setter method.
	typedef std::pair<std::string, int> SetterEntry;
	typedef std::map<std::string, int>::iterator SetterIterator;
	std::map<std::string, int> propertyTable;

	void installProperties();
};

}

#endif
