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

#include <string.h>
#include <sstream>

namespace sfl
{

unsigned InjectablePipeline::numberInstances = 0;

void InjectablePipeline::setCaps (GstCaps* caps)
{
    gst_app_src_set_caps (GST_APP_SRC (appsrc), caps);
}

GstCaps* InjectablePipeline::getCaps()
{
    return gst_app_src_get_caps (GST_APP_SRC (appsrc));
}

void InjectablePipeline::setMaxQueueSize (size_t size)
{
    gst_app_src_set_max_bytes (GST_APP_SRC (appsrc), size);
}

void InjectablePipeline::setField (const std::string& name, const std::string& value)
{
    GValue gstValue;
    memset (&gstValue, 0, sizeof (GValue));
    g_value_init (&gstValue, G_TYPE_STRING);
    g_value_set_string (&gstValue, value.c_str());

    // Returns : the GstCaps produced by the source. gst_caps_unref() after usage.
    GstCaps* caps = gst_app_src_get_caps (GST_APP_SRC (appsrc));

    caps = gst_caps_make_writable (caps);

    // Note that this method set the value in all structures.
    _debug ("InjectablePipeline: Setting field %s=%s on caps", name.c_str(), value.c_str());
    gst_caps_set_value (caps, name.c_str(), &gstValue);

    gst_app_src_set_caps (GST_APP_SRC (appsrc), caps); // Might not have to do that.

    gst_caps_unref(caps);

    _debug ("InjectablePipeline: New altered caps on injectable element %" GST_PTR_FORMAT, caps);
}

std::string InjectablePipeline::getField (const std::string& name)
{
    GstCaps* caps = gst_app_src_get_caps (GST_APP_SRC (appsrc));

    // We take for granted that the first structure is the one of interest
    GstStructure* structure = gst_caps_get_structure (caps, 0);

    // Try to find a field with the given name
    const GValue*  value = gst_structure_get_value (structure, name.c_str());
    if (!G_IS_VALUE(value)) {
        // TODO throw
    	_debug("InjectablePipeline: Field \"%s\" could not be found", name.c_str());
    	return std::string("");
    }

    // Convert the value to string
    gchar* valueStr;
    std::string output;
    if ( (valueStr = gst_value_serialize (value)) == NULL) {
    	_warn("InjectablePipeline: Failed to unserialize data.");
    }
    return std::string(valueStr);
}

void InjectablePipeline::onEnoughData()
{
    //_debug ("Appsrc queue has enough data");
    enoughData = true;
    notifyAll (NULL, "onEnoughData");
}

void InjectablePipeline::enough_data_cb (GstAppSrc *src, gpointer data)
{
    InjectablePipeline* self = (InjectablePipeline*) data;
    self->onEnoughData();
}

void InjectablePipeline::onNeedData()
{
    //_debug ("Appsrc queue needs more data");
    enoughData = false;
    notifyAll (NULL, "onNeedData");
}

void InjectablePipeline::need_data_cb (GstAppSrc *src, guint length,
                                       gpointer data)
{
    InjectablePipeline* self = (InjectablePipeline*) data;
    self->onNeedData();
}

void InjectablePipeline::inject (GstBuffer* data)
{
    if (enoughData == false) {
       // _debug ("Injecting buffer ...");
        if (gst_app_src_push_buffer (GST_APP_SRC (appsrc), data) != GST_FLOW_OK) {
            _warn ("InjectablePipeline: Failed to push buffer.");
        }
    } else {
        _warn ("InjectablePipeline: Dropping buffer. Not enough space in input queue");
    }
}

void InjectablePipeline::stop()
{
    _warn ("InjectablePipeline: Sending EOS message from injectable endpoint (%s:%d)", __FILE__, __LINE__);
    gst_app_src_end_of_stream (GST_APP_SRC (appsrc));
    Pipeline::stop();
}

void InjectablePipeline::init (GstCaps* caps, Pipeline& pipeline,
                               size_t maxQueueSize)
{
	_debug("InjectablePipeline: Init");

    gst_init (0, NULL);

    // Let the data be queued initially
    enoughData = false;

    // Create the appsrc (the injectable element)
    gchar* name = gst_element_get_name (getGstPipeline());

    std::stringstream ss;
    ss << name;
    ss << "_injectable_";
    ss << numberInstances;

    appsrc = gst_element_factory_make ("appsrc", (ss.str()).c_str());

    if (appsrc == NULL) {
        throw MissingGstPluginException (
            "Plugin \"appsrc\" could not be found. "
            "Check your install (you need gst-plugins-base). "
            "Run gst-inspect to get the list of available plugins");
    }

    g_object_set (G_OBJECT (appsrc), "do-timestamp", TRUE, NULL);

    // Install the callbacks
    GstAppSrcCallbacks sourceCallbacks;
    sourceCallbacks.need_data = InjectablePipeline::need_data_cb;
    sourceCallbacks.enough_data = InjectablePipeline::enough_data_cb;
    sourceCallbacks.seek_data = NULL;
    gst_app_src_set_callbacks (GST_APP_SRC (appsrc), &sourceCallbacks, this, NULL);

    // Set a maximum amount amount of bytes the queue can hold
    gst_app_src_set_stream_type (GST_APP_SRC (appsrc), GST_APP_STREAM_TYPE_STREAM);
    gst_app_src_set_max_bytes (GST_APP_SRC (appsrc), maxQueueSize);

    // Set the caps on the source
    if (caps != NULL) {
        gst_app_src_set_caps (GST_APP_SRC (appsrc), caps);
    }

    // Add to the existing pipeline
    gst_bin_add (GST_BIN (getGstPipeline()), appsrc);
}

void InjectablePipeline::setSink (GstElement* sink)
{
    if (gst_element_link (appsrc, sink) == FALSE) {
        throw GstException ("InjectablePipeline: Failed to prepend appsrc to head.");
    }
}

void InjectablePipeline::setSink (GstPad* other)
{
    GstPad* sourcePad = gst_element_get_static_pad (appsrc, "src");
    if (sourcePad == NULL) {
        throw GstException ("InjectablePipeline: Failed to obtain static pad on the source.");
    }

	if (gst_pad_link(sourcePad, other) != GST_PAD_LINK_OK) {
        throw GstException ("InjectablePipeline: Failed to prepend appsrc to pad.");
	}
}

InjectablePipeline::InjectablePipeline (Pipeline& pipeline) :
        Pipeline (pipeline.getGstPipeline())
{
    init (NULL, pipeline, MAX_QUEUE_SIZE);
}

InjectablePipeline::InjectablePipeline (Pipeline& pipeline, size_t maxQueueSize) :
        Pipeline (pipeline.getGstPipeline())
{
    init (NULL, pipeline, maxQueueSize);
}

InjectablePipeline::InjectablePipeline (Pipeline& pipeline, GstCaps* caps) :
        Pipeline (pipeline.getGstPipeline())
{
    init (caps, pipeline, MAX_QUEUE_SIZE);
}

InjectablePipeline::InjectablePipeline (Pipeline& pipeline, GstCaps* caps,
                                        size_t maxQueueSize) :
        Pipeline (pipeline.getGstPipeline())
{
    init (caps, pipeline, maxQueueSize);
}

}
