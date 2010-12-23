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

#ifndef __SFL_GST_DECODER_H__
#define __SFL_GST_DECODER_H__

#include "util/gstreamer/Filter.h"
#include "util/gstreamer/Pipeline.h"
#include "util/gstreamer/InjectablePipeline.h"
#include "util/gstreamer/RetrievablePipeline.h"
#include "video/codec/decoder/VideoDecoder.h"

#include <gst/rtp/gstrtpbuffer.h>

#include <list>

namespace sfl
{

/**
 * Extends VideoDecoder
 */
class GstDecoder : public VideoDecoder, protected Filter
{
    public:
        /**
         * @Override
         */
        GstDecoder() throw (VideoDecodingException, MissingPluginException);
        /**
         * @param maxFrameQueued The maximum number of frames to be queued before starting to drop the following ones.
         * @throw VideoEncodingException if an error occurs while opening the video decoder.
         */
        GstDecoder (VideoFormat& outputFormat) throw (VideoDecodingException, MissingPluginException);

        /**
         * Delete the endpoints and stop the pipeline.
         */
        ~GstDecoder();

        /**
         * @Override
         */
        void setOutputFormat (VideoFormat& format);

        /**
         * @Override
         */
        VideoFormat getOutputFormat();

        /**
         * @Override
         */
        void decode (Buffer<uint8>& data) throw (VideoDecodingException);

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
        void setParameter (const std::string& name, const std::string& value);

        /**
         * @Override
         */
        std::string getParameter (const std::string& name);

    private:
        VideoFormat outputVideoFormat;

        /**
         * This method contains virtual methods and therefore cannot be called from the constructor.
         * Therefore, it's rather being called in the activate() method.
         */
        void init() throw (VideoDecodingException, MissingPluginException);

        InjectablePipeline* injectableEnd;
        RetrievablePipeline* retrievableEnd;

        /**
         * Observer object for raw video frames produced by this decoder.
         * We only re-broadcast the event externally.
         */
        class PipelineEventObserver : public RetrievablePipelineObserver
        {
            public:
                PipelineEventObserver (GstDecoder* encoder) : parent (encoder) {}
                GstDecoder* parent;
                /**
                 * @Override
                 */
                void onNewBuffer (GstBuffer* buffer) {
                	gst_caps_unref(parent->currentCaps);

                	GstCaps* currentCaps = gst_buffer_get_caps(buffer);
                	if (currentCaps != NULL) {
                    	parent->currentCaps = currentCaps;
                	}

                    Buffer<uint8_t> buf(GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer));
                    parent->notifyAll(buf);
                }
        };
        PipelineEventObserver* outputObserver;

        /**
         * Holds a list of codec-specific parameters. This is needed because init() can't be called
         * within the constructor because it contains pure virtual functions. Therefore, init() only
         * gets called when activate() is called. But if the user calls setParameters() before that,
         * we might end up into big troubles.
         */
        std::list<std::pair<std::string, std::string> > parameters;

        /**
         * Holds the current caps, read on the buffer.
         */
        GstCaps* currentCaps;
};

}

#endif
