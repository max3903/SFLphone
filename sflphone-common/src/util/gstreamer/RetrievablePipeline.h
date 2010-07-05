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

#ifndef __SFL_RETRIEVABLE_PIPELINE_H__
#define __SFL_RETRIEVABLE_PIPELINE_H__

#include "util/gstreamer/Pipeline.h"
#include "util/pattern/AbstractObservable.h"

#include <gst/app/gstappsink.h>
#include "logger.h"
namespace sfl {

/**
 * This type of observer gets called when some data can be retrieve in a non-blocking fashion.
 */
class RetrievablePipelineObserver : public Observer {
public:
	virtual void onNewBuffer(GstBuffer* buffer) = 0;
};

class RetrievablePipeline : public Pipeline, public AbstractObservable<GstBuffer*, RetrievablePipelineObserver> {
public:
	/**
	 * @param bin Part of the pipeline in which the data will flow through from the previous elements.
	 * @param tail The last element in the processing graph from which to retreive frames.
	 * @precondition The "pipeline" argument must be existing.
	 */
	RetrievablePipeline(Pipeline& bin, GstElement* tail);

	/**
	 * @param bin Part of the pipeline in which the data will flow through from the previous elements.
	 * @param caps The caps that this source should have.
	 * @precondition The "pipeline" argument must be existing.
	 */
	RetrievablePipeline(Pipeline& bin, GstElement* tail, GstCaps* caps);

	/**
	 * @param bin Part of the pipeline in which the data will flow through from the previous elements.
	 * @param caps The caps that this source should have.
	 * @param maxBuffers The number of buffers that can be queued at the sink.
	 * @precondition The "pipeline" argument must be existing.
	 */
	RetrievablePipeline(Pipeline& bin, GstElement* tail, GstCaps* caps, uint maxBuffers);

	/**
	 * @param source The element from which the retrievable endpoint should receive data from.
	 */
	void setSource(GstElement* source);

protected:
	/**
	 * Simple dispatch for this observer type.
	 */
	void notify(RetrievablePipelineObserver* observer, GstBuffer* data) {
		_debug("Notifying observers");
		observer->onNewBuffer(data);
	}

	void notify(RetrievablePipelineObserver* observer, const std::string& name, GstBuffer* data){};

	/**
	 * Helper method for constructors.
	 */
	void init(GstCaps* caps, Pipeline& pipeline, GstElement* tail);

private:
	static GstFlowReturn onNewBuffer(GstAppSink* sink, gpointer data);
	static GstFlowReturn onNewPreroll(GstAppSink* sink, gpointer data);
	static GstFlowReturn onNewBufferList(GstAppSink* sink, gpointer data);
	static void onEos(GstAppSink* sink, gpointer data);

	GstElement* appsink;
	static unsigned numberInstances;
};
}

#endif
