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
#include "RetrievablePipeline.h"
#include "logger.h"

#include <sstream>

namespace sfl
{
unsigned int RetrievablePipeline::numberInstances = 0;

GstFlowReturn RetrievablePipeline::onNewBuffer (GstAppSink* sink, gpointer data)
{
    RetrievablePipeline* self = (RetrievablePipeline*) data;

    GstBuffer* buffer = gst_app_sink_pull_buffer (GST_APP_SINK (self->appsink));
    if (!buffer) {
        _warn ("RetreivablePipeline: Pulled a NULL buffer");
        return GST_FLOW_OK;
    }

    // Notify the observers (it's their only chance to copy the data if need to. Otherwise, we'll free the buffer just after.)
    self->notifyAll (buffer);
    // Free the buffer. Big mem leak if missing.
    gst_buffer_unref (buffer);

    return GST_FLOW_OK;
}

GstBuffer* RetrievablePipeline::getBuffer()
{
	return gst_app_sink_pull_preroll (GST_APP_SINK (appsink));
}

GstFlowReturn RetrievablePipeline::onNewPreroll (GstAppSink *sink, gpointer data)
{

	/*
    _debug ("RetreivablePipeline: New preroll buffer is available");

    RetrievablePipeline* self = (RetrievablePipeline*) data;

    GstBuffer *prerollBuffer = gst_app_sink_pull_preroll(GST_APP_SINK (self->appsink));
    if(prerollBuffer == NULL) {
    	_warn ("RetreivablePipeline: Pulled a NULL buffer");
    	return GST_FLOW_OK;
    }

    // Notify the observers
    self->notifyAll (prerollBuffer);
	*/
    return GST_FLOW_OK;
}

GstFlowReturn RetrievablePipeline::onNewBufferList (GstAppSink *sink,
        gpointer user_data)
{
    _debug ("RetreivablePipeline: New buffer list is available");
    return GST_FLOW_OK;
}

void RetrievablePipeline::onEos (GstAppSink *sink, gpointer user_data)
{
    _warn ("RetreivablePipeline: Got EOS on pipeline at appsink");
}

void RetrievablePipeline::init (GstCaps* caps, Pipeline& pipeline)
{
    // Create new appsink
    gchar* name = gst_element_get_name (getGstPipeline());

    std::stringstream ss;
    ss << name;
    ss << "_retrievable_";
    ss << numberInstances;

    _debug("RetreivablePipeline: Init appsink");

    appsink = gst_element_factory_make ("appsink", (ss.str()).c_str());

    gst_app_sink_set_max_buffers (GST_APP_SINK (appsink), 2); // FIXME Hardcoded
    gst_app_sink_set_drop (GST_APP_SINK (appsink), FALSE);
    //gst_app_sink_set_caps(GST_APP_SINK (appsink), caps);

    // Configure callbacks for the appsink
    GstAppSinkCallbacks sinkCallbacks;
    sinkCallbacks.eos = RetrievablePipeline::onEos;
    sinkCallbacks.new_preroll = RetrievablePipeline::onNewPreroll;
    sinkCallbacks.new_buffer = RetrievablePipeline::onNewBuffer;
    sinkCallbacks.new_buffer_list = RetrievablePipeline::onNewBufferList;

    gst_app_sink_set_callbacks (GST_APP_SINK (appsink), &sinkCallbacks, this,
                                NULL);

    _debug ("RetreivablePipeline: Callbacks configured on appsink");

    // Add to the existing pipeline
    gst_bin_add_many (GST_BIN (getGstPipeline()), appsink, NULL);
}

void RetrievablePipeline::setSource (GstElement* source)
{
    if (gst_element_link (source, appsink) == FALSE) {
        throw VideoDecodingException (
            "Failed to append appsink to the existing pipeline.");
    }
}

void RetrievablePipeline::setCaps (GstCaps* caps)
{
    gst_app_sink_set_caps (GST_APP_SINK (appsink), caps);
}

GstCaps* RetrievablePipeline::getCaps()
{
    return  gst_app_sink_get_caps (GST_APP_SINK (appsink));
}

RetrievablePipeline::RetrievablePipeline (Pipeline& pipeline) :
        Pipeline (pipeline.getGstPipeline())
{
    init (NULL, pipeline);
}

RetrievablePipeline::RetrievablePipeline (Pipeline& pipeline, GstCaps* caps) :
        Pipeline (pipeline.getGstPipeline())
{
    init (caps, pipeline);
}

RetrievablePipeline::RetrievablePipeline (Pipeline& pipeline, GstCaps* caps,
        uint maxBuffers) :
        Pipeline (pipeline.getGstPipeline())
{

    init (caps, pipeline);
    gst_app_sink_set_max_buffers (GST_APP_SINK (appsink), maxBuffers);
}

}
