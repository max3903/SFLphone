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

namespace sfl {

Pipeline::Pipeline(GstElement* pipeline) {
	this->pipeline = pipeline;
}

void Pipeline::stop() throw (GstException) {
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref( pipeline);

	_warn("Video decoder stopped.");
}

void Pipeline::start() throw (GstException) {
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

GstElement* Pipeline::getGstPipeline() {
	return pipeline;
}

}

