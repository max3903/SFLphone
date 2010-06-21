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

#include "VideoDecoder.h"
#include "video/FrameFormat.h"

namespace sfl {
/**
 * H264 decode based on Gstreamer's element ffdec_h264.
 */
class H264GstDecoder: public VideoDecoder {
public:
	H264GstDecoder(const VideoFormat& encodingFormat,
			const VideoFormat& decodingFormat) throw (VideoDecodingException,
			MissingPluginException);
	~H264Decoder();

	/**
	 * This call is asynchronous. The data will simply be placed in a queue and
	 * only be processed when Gstreamer requests it.
	 *
	 * @Override
	 */
	int decode(Buffer<uint8_t>& buffer) throw (VideoDecodingException);

private:
	/**
	 * Forward declaration. We don't want to pollute the header with includes.
	 */
	class GstBus;
	class GObject;
	class GMainLoop;
	class GstElement;
	class GstMessage;
	class GParamSpec;
	typedef int gboolean;
	typedef void* gpointer;
	typedef unsigned int guint;

	/**
	 * Gstreamer callback for messages that are posted on the bus.
	 */
	friend static gboolean gstreamer_bus_callback(GstBus* bus, GstMessage* msg,
			gpointer data);

	/**
	 * This function is called when the appsrc is initialized.
	 */
	friend static void init_appsrc_callback(GObject* object, GObject* orig,
			GParamSpec* pspec, gpointer self);

	/**
	 * This signal callback is called when appsrc needs data,
	 * we add an idle handler to the mainloop to start pushing
	 * data into the appsrc.
	 */
	friend static void start_feed(GstElement* playbin, guint size, gpointer self);

	/**
	 * This callback is called when appsrc has enough data and
	 * we can stop sending. We remove the idle handler from the mainloop.
	 */
	friend static void stop_feed(GstElement* playbin, gpointer self);

	/**
	 * This method is called by the idle GSource in the mainloop.
	 *
	 * The idle handler is added to the mainloop when appsrc requests us to start
	 * sending data (need-data signal) and is removed when appsrc has enough data
	 * (enough-data signal).
	 */
	friend static gboolean read_data(gpointer self);

	GstElement* pipeline;
	GstElement* appsrc;
	GstElement* decoder;
	GstElement* appsink;

	GstBus* bus;
	guint sourceid;
	GMainLoop* loop;

	std::queue<Buffer<uint8_t> > nalUnits;
	static const int MAX_BUS_POOL_WAIT = 10;

};

}

#endif
