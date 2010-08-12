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

#ifndef __SFL_GST_ENCODER_H__
#define __SFL_GST_ENCODER_H__

#include "util/gstreamer/Filter.h"
#include "util/gstreamer/Pipeline.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"

#include "video/codec/encoder/VideoEncoder.h"

#include <gst/rtp/gstrtpbuffer.h>

#include <map>

namespace sfl
{

/**
 * Extends VideoEncoder, and implements RetrievablePipelineObserver
 */
class GstEncoder: public VideoEncoder, protected Filter
{
    public:
		GstEncoder();

        GstEncoder(const VideoFormat& format) throw (VideoEncodingException, MissingPluginException);

        /**
         * @param maxFrameQueued The maximum number of frames to be queued before starting to drop the following ones.
         * @throw VideoEncodingException if an error occurs while opening the video decoder.
         */
        GstEncoder (const VideoFormat& format, unsigned maxFrameQueued)
        throw (VideoDecodingException, MissingPluginException);

        ~GstEncoder();

        /**
         * @Override
         */
        void encode (const VideoFrame* frame) throw (VideoEncodingException);

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
    	void setVideoInputFormat(const VideoFormat& format);

        /**
         * @Override
         */
        void setParameter (const std::string& name, const std::string& value);

        /**
         * @Override
         */
        std::string getParameter (const std::string& name);

        static const unsigned MAX_FRAME_QUEUED = 10;

    private:

        unsigned maxFrameQueued;

        /**
         * Helper method for constructors.
         */
        void init() throw (VideoDecodingException, MissingPluginException);

        InjectablePipeline* injectableEnd;
        RetrievablePipeline* retrievableEnd;

        /**
         * Observer object for NAL units produced by this encoder.
         * We only re-broadcast the event externally.
         */
        class PipelineEventObserver: public RetrievablePipelineObserver
        {
            public:
                PipelineEventObserver (GstEncoder* encoder) :
                        parent (encoder) {
                }
                GstEncoder* parent;
                /**
                 * @Override
                 */
                void onNewBuffer (GstBuffer* buffer) {
                    _debug ("NAL unit produced at the sink ...");
                    // _debug("Caps on buffer at the SINK %" GST_PTR_FORMAT, gst_buffer_get_caps(buffer));

                    GstBuffer* payload = gst_rtp_buffer_get_payload_buffer (buffer);
                    uint32 timestamp = gst_rtp_buffer_get_timestamp (buffer);

                    uint8* payloadData = GST_BUFFER_DATA (payload);
                    uint payloadSize = GST_BUFFER_SIZE (payload);

                    std::pair<uint32, Buffer<uint8> > nalUnit (timestamp,
                            Buffer<uint8> (payloadData, payloadSize));

                    _debug ("Notifying buffer of size %d with timestamp %u", payloadSize, timestamp);
                    parent->notifyAll (nalUnit);
                }
        };
        PipelineEventObserver* outputObserver;

        /**
         * Holds a list of codec-specific parameters. This is needed because the video source
         * cannot be set at object creation is some cases. Then if setVideoSource() occurs after setParameters(),
         * then the latter will fail as the GstCaps had not been created yet at that point.
         */
        std::list<std::pair<std::string, std::string> > parameters;

        /**
         * Given that a video input source was set, configure the corresponding caps on the appsrc element.
         */
        void configureSource();
};

}

#endif
