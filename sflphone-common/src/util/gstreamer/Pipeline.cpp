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

#include "Pipeline.h"
#include "logger.h"

#include <sstream>

namespace sfl {

unsigned Pipeline::UNIQUE_COUNTER_NAME = 0;

gboolean Pipeline::message_cb(GstBus*, GstMessage* msg, gpointer data) {
	Pipeline* self = reinterpret_cast<Pipeline*>(data);

	_debug("Message callback");

  switch ( GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      break;

    case GST_MESSAGE_TAG:
      break;

    default:
      break;
  }

  return FALSE;
}

GstBusSyncReply
Pipeline::message_sync_cb (GstBus * bus, GstMessage * message, gpointer data)
{
	//Pipeline* self = reinterpret_cast<Pipeline*>(data);
	Pipeline* self = ((Pipeline*) data);

	GError* error = NULL;
	gchar* debug = NULL;

	  switch (GST_MESSAGE_TYPE(message)) {
	    case GST_MESSAGE_STATE_CHANGED:
	        GstState oldState, newState;
	        gst_message_parse_state_changed (message, &oldState, &newState, NULL);

	        if (newState == GST_STATE_PLAYING) {
	        	self->playing = true;
	        } else {
	        	self->playing = false;
	        }

	        _debug("Went from state %s to %s",
	        		gst_element_state_get_name(oldState),
	        		gst_element_state_get_name(newState));

	        break;

	    case GST_MESSAGE_ERROR:

	    	gst_message_parse_error(message, &error, &debug);
	    	_error("Gstreamer error : %s", debug);

	    	g_error_free(error);
	    	g_free(debug);
	      break;

	    default:
	      break;
	  }
	  return GST_BUS_PASS;
}

Pipeline::Pipeline(const std::string& name) throw (GstException) :
		playing(false)  {
	GstElement* element = gst_pipeline_new(name.c_str());

	if (element == NULL) {
		throw GstException("Failed to create new pipeline");
	}

	init(element);
}

Pipeline::Pipeline(GstElement* pipeline) {
	init(pipeline);
}

void Pipeline::init(GstElement* pipeline) {
	this->pipeline = pipeline;
	prefix = "sfl";

	gst_init(0, NULL);
}

void Pipeline::stop() throw (GstException) {

    gst_bus_set_sync_handler(gst_pipeline_get_bus(GST_PIPELINE(pipeline)), NULL, NULL);
	g_source_remove(watchId);

	GstStateChangeReturn ret;

	ret = gst_element_set_state(pipeline, GST_STATE_NULL);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		_error("sfl:Pipeline : could not go to NULL\n");
	}
	ret = gst_element_get_state(pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
	if (ret != GST_STATE_CHANGE_SUCCESS) {
		_error("sfl::Pipeline : could not complete NULL\n");
	}

	gst_object_unref(GST_OBJECT(pipeline));

	_warn ("Video decoder stopped.");
}

bool Pipeline::isPlaying() throw (GstException) {
	return playing;
}

void Pipeline::start() throw (GstException) {
	GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_set_sync_handler (bus, message_sync_cb, this);
	watchId = gst_bus_add_watch(bus, message_cb, this);
	gst_object_unref(bus);

	GstStateChangeReturn ret = gst_element_set_state(pipeline,
			GST_STATE_PLAYING);

	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_print("Failed to start up pipeline!\n");

		// Check if there is an error message with details on the bus
		std::string errorMessage = "An error occured while starting decoding. ";
		GstBus* bus = gst_element_get_bus(pipeline);
		GstMessage *msg = gst_bus_poll(bus, GST_MESSAGE_ERROR,
				MAX_BUS_POOL_WAIT);
		gst_object_unref(bus);

		if (msg) {
			GError *err = NULL;
			gst_message_parse_error(msg, &err, NULL);

			errorMessage = errorMessage + std::string(err->message);
			g_error_free(err);
			gst_message_unref(msg);
		}

		throw GstException(errorMessage);
	}
}

void Pipeline::addTee(const std::string& teeName) throw (GstException) {
	GstElement* tee = gst_element_factory_make("tee", teeName.c_str());

	if (tee == NULL) {
		throw GstException("Failed to create \"tee\"");
	}

	if (gst_bin_add(GST_BIN(pipeline), tee) == FALSE) {
		throw GstException("The bin does not want to accept the element");
	}
}

void Pipeline::addTee(const std::string& teeName, GstElement* previous)
		throw (GstException) {
	GstElement* tee = gst_element_factory_make("tee", teeName.c_str());

	if (tee == NULL) {
		throw GstException("Failed to create \"tee\"");
	}

	if (gst_bin_add(GST_BIN(pipeline), tee) == FALSE) {
		throw GstException("The bin does not want to accept the element");
	}

	link(previous, tee);
}

void Pipeline::addTee(const std::string& teeName, GstPad* pad)
		throw (GstException) {
	GstElement* tee = gst_element_factory_make("tee", teeName.c_str());

	if (tee == NULL) {
		throw GstException("Failed to create \"tee\"");
	}

	if (gst_bin_add(GST_BIN(pipeline), tee) == FALSE) {
		throw GstException("The bin does not want to accept the element");
	}

	link(pad, tee);
}

/**
 * Branch at the given "tee".
 * @param teeName the name of an existing tee element.
 */
GstPad* Pipeline::branch(const std::string& teeName) {
	GstElement* tee = gst_bin_get_by_name(GST_BIN(pipeline), teeName.c_str());

	//Branch at the current branchName by requesting a new pad.
	return gst_element_get_request_pad(tee, "src%d");
}

GstElement* Pipeline::addElement(const std::string& factoryName)
		throw (MissingGstPluginException, GstException) {
	GstElement* element = createElement(factoryName);

	return element;
}

GstElement* Pipeline::addElement(const std::string& factoryName, GstPad* pad)
		throw (MissingGstPluginException, GstException) {
	GstElement* element = createElement(factoryName);
	link(pad, element);

	return element;
}

GstElement* Pipeline::addElement(const std::string& factoryName,
		GstElement* previous) throw (MissingGstPluginException, GstException) {
	GstElement* element = createElement(factoryName);
	link(previous, element);

	return element;
}

void Pipeline::setPrefix(const std::string& prefix) {
	this->prefix = prefix;
}

GstElement* Pipeline::createElement(const std::string& factoryName)
		throw (MissingGstPluginException, GstException) {
	std::ostringstream uniqueName;
	uniqueName << prefix << factoryName << "_" << UNIQUE_COUNTER_NAME;

	GstElement* element = gst_element_factory_make(factoryName.c_str(),
			(uniqueName.str()).c_str());

	if (element == NULL) {
		throw MissingGstPluginException(std::string("Plugin \"") + factoryName
				+ std::string("\" could not be found. "
					"Run gst-inspect to get the list of available plugins"));
	}

	if (gst_bin_add(GST_BIN(pipeline), element) == FALSE) {
		throw GstException("The bin does not want to accept the element");
	}

	UNIQUE_COUNTER_NAME += 1;
	return element;
}

void Pipeline::link(GstElement* src, GstElement* dst) throw (GstException) {
	if (gst_element_link(src, dst) == FALSE) {
		throw GstException("Failed to link elements");
	}
}

void Pipeline::link(GstPad* src, GstElement* dst) throw (GstException) {
	GstPad* sinkPad = gst_element_get_static_pad(dst, "sink"); // TODO throw exception
	gst_pad_link(src, sinkPad);
}

void Pipeline::link(GstElement* src, GstPad* dst) throw (GstException) {
	GstPad* sourcePad = gst_element_get_static_pad(src, "src"); // TODO throw exception
	gst_pad_link(sourcePad, dst);
}

void Pipeline::link(GstPad* src, GstPad* dst) throw (GstException) {
	gst_pad_link(src, dst); // TODO throw exception
}

GstElement* Pipeline::getGstPipeline() {
	return pipeline;
}

}

