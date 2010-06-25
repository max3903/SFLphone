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
#ifndef __SFL_H264_GST_DECODER_H__
#define __SFL_H264_GST_DECODER_H__

#include "util/Buffer.h"
#include "VideoDecoder.h"
#include "video/FrameFormat.h"

#include "video/VideoExceptions.h"

#include <queue>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

namespace sfl {

/**
 * H264 decode based on Gstreamer's element ffdec_h264.
 */
class H264GstDecoder: public VideoDecoder {
public:
	H264GstDecoder() throw (VideoDecodingException, MissingPluginException);
	H264GstDecoder(const VideoFormat& decodingFormat)
			throw (VideoDecodingException, MissingPluginException);
	~H264GstDecoder();

	/**
	 * This call is asynchronous. The data will simply be placed in a queue and
	 * only be processed when Gstreamer requests it.
	 *
	 * @Override
	 */
	void decode(Buffer<uint8>& buffer) throw (VideoDecodingException);

	/**
	 * @Override
	 */
	Dimension getDimension() const;

	/**
	 * @Override
	 */
	std::string getFourcc() const;

	/**
	 * @Override
	 */
	VideoFormat getOutputFormat() const;

	/**
	 * @Override
	 */
	void setOutputFormat(const VideoFormat& decodingFormat)
			throw (VideoDecodingException);

private:
	/**
	 * Helper method to avoid code duplications with different constructors.
	 */
	void init();

	/**
	 * This signal callback is called when the internal queue in appsrc is full.
	 */
	static void onEnoughData(GstAppSrc *src, gpointer user_data);

	/**
	 * This method is called when a new buffer becomes available at the sink.
	 */
	static GstFlowReturn onNewBuffer(GstAppSink* sink, gpointer self);

	/**
	 * Pull the buffer out from the decoder and notify the observers.
	 */
	void dispatchEvent();

	/**
	 * Start the pipeline.
	 */
	void start() throw (VideoDecodingException);

	/**
	 * Stop the pipeline.
	 */
	void stop() throw (VideoDecodingException);

	GstElement* pipeline;
	GstElement* appsrc;
	GstElement* depayloader;
	GstElement* parser;
	GstElement* decoder;
	GstElement* ffmpegcolorspace;
	GstElement* deinterlace;
	GstElement* videoscale;
	GstElement* tee;
	GstElement* displayQueue;
	GstElement* displayWindow;
	GstElement* appsinkQueue;
	GstElement* appsink;

	static const int MAX_BUS_POOL_WAIT = 10;
};

}

#endif
