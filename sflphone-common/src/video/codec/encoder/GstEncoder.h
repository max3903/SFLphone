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

    protected:
    	/**
    	 * Generate SDP parameters from video input format.
    	 * This method is not meant to be called by GstEncoder subclasses.
    	 * @postcondition Some of the missing parameters should now be filled with values.
    	 */
    	void generateSdpParameters();

    private:

        unsigned maxFrameQueued;

        InjectablePipeline* injectableEnd;

        RetrievablePipeline* retrievableEnd;

        std::map<std::string, std::string> parameters;
        typedef std::pair<std::string, std::string> ParameterEntry;
        typedef std::map<std::string, std::string>::iterator ParameterIterator;

        GstCaps* bufferCaps;

        GstElement* videotestsrc;

        GstPad* videotestsrcPad;

        GstElement* capsFilterVideoTestSrc;

        GstPad* appsrcPad;

        GstElement* inputselector;

        /**
         * Helper method for constructors.
         */
        void init() throw (VideoDecodingException, MissingPluginException);

        /**
         * Set the GstCaps structure that contains those parameters usable for SDP.
         * Set only once.
         * @param caps The GstCaps structure obtained from th GstBuffer at the sink element.
         */
        void setBufferCaps(GstCaps* caps);

        /**
         * @return The buffer caps, or NULL if those are not available.
         */
        GstCaps* getBufferCaps();

        /**
         * Add a parameter to the local parameter list.
         * @param name The parameter name (the key in a map)
         * @param value The value that this parameter takes.
         */
        void addParameter(const std::string& name, const std::string& value);

        /**
         * A function that will be called in gst_structure_foreach(), in generateSdpParameters().
         * @param field_id the GQuark of the field name
		 * @param value the GValue of the field
		 * @param user_data user data (A pointer on the object instance)
         */
        static gboolean extractParameter(GQuark field_id, const GValue *value, gpointer user_data);

        /**
         * @param selected true if the videotestsrc element should be selected as input.
         * If set to false, the appsrc will be selected.
         */
        void selectVideoTestSrc(bool selected);

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
                    GstBuffer* payload = gst_rtp_buffer_get_payload_buffer (buffer);

                    uint32 timestamp = gst_rtp_buffer_get_timestamp (buffer);

                    uint8* payloadData = GST_BUFFER_DATA (payload);
                    uint payloadSize = GST_BUFFER_SIZE (payload);

                    std::pair<uint32, Buffer<uint8> > nalUnit (timestamp,
                            Buffer<uint8> (payloadData, payloadSize));

                    //_debug ("Notifying buffer of size %d with timestamp %u", payloadSize, timestamp);
                    parent->notifyAll (nalUnit);
                }
        };
        PipelineEventObserver* outputObserver;

        /**
         * Holds a list of codec-specific parameters. This is needed because the video source
         * cannot be set at object creation is some cases. Then if setVideoSource() occurs after setParameters(),
         * then the latter will fail as the GstCaps had not been created yet at that point.
         */
        std::list<std::pair<std::string, std::string> > userSpecifiedParameters;

        /**
         * Given that a video input source was set, configure the corresponding caps on the appsrc element.
         */
        void configureSource();
};

}

#endif
