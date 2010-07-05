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

#include "InjectablePipeline.h"
#include "logger.h"

#include <sstream>

namespace sfl {

unsigned InjectablePipeline::numberInstances = 0;

void InjectablePipeline::onEnoughData() {
	_debug("Appsrc queue has enough data");
	enoughData = true;

	notifyAll(NULL, "onEnoughData");
}

void InjectablePipeline::enough_data_cb(GstAppSrc *src, gpointer data) {
	InjectablePipeline* self = (InjectablePipeline*) data;
	self->onEnoughData();
}

void InjectablePipeline::onNeedData() {
	_debug("Appsrc queue now needs data");
	enoughData = false;
	notifyAll(NULL, "onNeedData");
}

void InjectablePipeline::need_data_cb(GstAppSrc *src, guint length,
		gpointer data) {
	InjectablePipeline* self = (InjectablePipeline*) data;
	self->onNeedData();
}

void InjectablePipeline::inject(GstBuffer* data) {
	if (enoughData == false) {
		_debug("Injecting buffer ...");
		if (gst_app_src_push_buffer(GST_APP_SRC(appsrc), data) != GST_FLOW_OK) {
			_warn("Failed to push buffer.");
		}
	}
}

void InjectablePipeline::stop() {
	_warn("Sending EOS message from injectable endpoint (%s:%d)", __FILE__, __LINE__);
	gst_app_src_end_of_stream(GST_APP_SRC(appsrc));
	Pipeline::stop();
}

void InjectablePipeline::init(GstCaps* caps, Pipeline& pipeline,
		GstElement* head, size_t maxQueueSize) {
	gst_init(0, NULL);

	// Let the data be queued initially
	enoughData = false;

	// Create the appsrc (the injectable element)
	gchar* name = gst_element_get_name(getGstPipeline());

	std::stringstream ss;
	ss << name;
	ss << "_injectable_";
	ss << numberInstances;

	appsrc = gst_element_factory_make("appsrc", (ss.str()).c_str());

	if (appsrc == NULL) {
		throw MissingGstPluginException(
				"Plugin \"appsrc\" could not be found. "
					"Check your install (you need gst-plugins-base). "
					"Run gst-inspect to get the list of available plugins");
	}

	gst_base_src_set_live(GST_BASE_SRC(appsrc), TRUE); // FIXME probably useless

	// Install the callbacks
	GstAppSrcCallbacks sourceCallbacks;
	sourceCallbacks.need_data = InjectablePipeline::need_data_cb;
	sourceCallbacks.enough_data = InjectablePipeline::enough_data_cb;
	sourceCallbacks.seek_data = NULL;
	gst_app_src_set_callbacks(GST_APP_SRC(appsrc), &sourceCallbacks, this, NULL);

	// Set a maximum amount amount of bytes the queue can hold
	gst_app_src_set_stream_type(GST_APP_SRC(appsrc), GST_APP_STREAM_TYPE_STREAM);
	gst_app_src_set_max_bytes(GST_APP_SRC(appsrc), maxQueueSize);

	// Set the caps on the source
	if (caps != NULL) {
		gst_app_src_set_caps(GST_APP_SRC(appsrc), caps);
	}

	// Add to the existing pipeline
	gst_bin_add(GST_BIN(getGstPipeline()), appsrc);

	// Link the new source to the existing pipeline
	if (head != NULL) {
		if (gst_element_link(appsrc, head) == FALSE) {
			throw GstException("Failed to prepend appsrc to head.");
		}
	}
}

void InjectablePipeline::setSink(GstElement* sink) {
	if (gst_element_link(appsrc, sink) == FALSE) {
		throw GstException("Failed to prepend appsrc to head.");
	}
}

InjectablePipeline::InjectablePipeline(Pipeline& pipeline, GstElement* head) :
	Pipeline(pipeline.getGstPipeline()) {
	init(NULL, pipeline, head, MAX_QUEUE_SIZE);
}

InjectablePipeline::InjectablePipeline(Pipeline& pipeline, GstElement* head,
		GstCaps* caps) :
	Pipeline(pipeline.getGstPipeline()) {
	init(caps, pipeline, head, MAX_QUEUE_SIZE);
}

InjectablePipeline::InjectablePipeline(Pipeline& pipeline, GstElement* head,
		GstCaps* caps, size_t maxQueueSize) :
	Pipeline(pipeline.getGstPipeline()) {
	init(caps, pipeline, head, maxQueueSize);
}

}
