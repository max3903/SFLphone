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

namespace sfl {
unsigned int RetrievablePipeline::numberInstances = 0;

GstFlowReturn RetrievablePipeline::onNewBuffer(GstAppSink* sink, gpointer data) {
	RetrievablePipeline* self = (RetrievablePipeline*) data;

	GstBuffer* buffer = gst_app_sink_pull_buffer(GST_APP_SINK(self->appsink));
	if (buffer == NULL) {
		_warn("Pulled a NULL buffer");
	}

	self->notifyAll(buffer);

	return GST_FLOW_OK;
}

void RetrievablePipeline::init(GstCaps* caps, Pipeline& pipeline)
{
	// Create new appsink
	gchar* name = gst_element_get_name(getGstPipeline());

	std::stringstream ss;
	ss << name;
	ss << "_retrievable_";
	ss << numberInstances;

	appsink = gst_element_factory_make("appsrc", (ss.str()).c_str());

	// Configure callbacks for the appsink
	GstAppSinkCallbacks sinkCallbacks;
	sinkCallbacks.eos = NULL;
	sinkCallbacks.new_preroll = NULL;
	sinkCallbacks.new_buffer = RetrievablePipeline::onNewBuffer;
	sinkCallbacks.new_buffer_list = NULL;
	gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &sinkCallbacks, this, NULL);

	// Add everything to the pipeline.
	gst_bin_add_many(GST_BIN(getGstPipeline()), appsink, NULL);

	if (gst_element_link_many(getGstPipeline(), appsink, NULL) == FALSE) {
		throw VideoDecodingException("Failed to link one or more elements.");
	}
}

RetrievablePipeline::RetrievablePipeline(Pipeline& pipeline) : Pipeline(pipeline.getGstPipeline()) {
	init(NULL, pipeline);
}

RetrievablePipeline::RetrievablePipeline(GstCaps* caps, Pipeline& pipeline) : Pipeline(pipeline.getGstPipeline()) {
	init(caps, pipeline);
}

RetrievablePipeline::RetrievablePipeline(GstCaps* caps, Pipeline& pipeline,
		uint maxBuffers) : Pipeline(pipeline.getGstPipeline()) {

	init(caps, pipeline);
	gst_app_sink_set_max_buffers(GST_APP_SINK(appsink), maxBuffers);
}

}
