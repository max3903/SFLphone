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

#ifndef __SFL_INJECTABLE_PIPELINE_H__
#define __SFL_INJECTABLE_PIPELINE_H__

#include "Pipeline.h"
#include "util/pattern/Observer.h"
#include "util/pattern/AbstractObservable.h"

#include <gst/app/gstappsrc.h>

namespace sfl {

/**
 * Observer type for InjectablePipeline.
 */
class InjectablePipelineObserver : public Observer {
public:
	/**
	 * This method is called no more data is needed in the incoming queue (when it's full).
	 */
	virtual void onNeedData() = 0;

	/**
	 * This method is called when the incoming queue is empty and elements downstream need more data.
	 */
	virtual void onEnoughData() = 0;
};

/**
 * This object allows for easy creation of Gstreamer pipelines where
 * data could be injected and retrieved at the other end.
 *
 * Uses appsrc -> * user defined *
 */
class InjectablePipeline : public Pipeline, public AbstractObservable<void*, InjectablePipelineObserver> {
public:
	/**
	 * @param pipeline Part of the pipeline in which the data will flow through from the source.
	 * @precondition The "pipeline" argument must be existing.
	 */
	InjectablePipeline(Pipeline& pipeline);

	/**
	 * @param pipeline Part of the pipeline in which the data will flow through from the source.
	 * @param maxQueueSize The maximum amount of bytes that can be queued at the source.
	 * @precondition The "pipeline" argument must be existing.
	 */
	InjectablePipeline(Pipeline& pipeline, size_t maxQueueSize);

	/**
	 * @param pipeline Part of the pipeline in which the data will flow through from the source.
	 * @param caps The caps that this source should have.
	 * @precondition The "pipeline" argument must be existing.
	 */
	InjectablePipeline(Pipeline& pipeline, GstCaps* caps);

	/**
	 * @param pipeline Part of the pipeline in which the data will flow through from the source.
	 * @param caps The caps that this source should have.
	 * @param maxQueueSize The maximum amount of bytes that can be queued at the source.
	 * @precondition The "pipeline" argument must be existing.
	 */
	InjectablePipeline(Pipeline& middle, GstCaps* caps, size_t maxQueueSize);

	virtual ~InjectablePipeline() {};;

	/**
	 * @Override
	 */
	void stop();

	/**
	 * Set the caps at the source.
	 * @param caps The caps to set at the source.
	 */
	void setCaps(GstCaps* caps);

	/**
	 * @return The caps set at the source.
	 */
	GstCaps* getCaps();

	/**
	 * Set the value of a field identified by "name".
	 * @param name The field name.
	 * @param value The value this field should be set to.
	 * @postcondition  If the field does not exist, it is created.
	 * If the field exists, the previous value is replaced.
	 * Set in all structures of caps.
	 */
	void setField(const std::string& name, const std::string& value);

	/**
	 * @param name The field name.
	 * @return The value for this field on the first structure of the caps at the source.
	 */
	std::string getField(const std::string& name);

	/**
	 * Inject the given buffer into the pipeline.
	 * @param data The data to inject downstream.
	 * @postcondition The data gets queued until those elements downstream can process it.
	 */
	void inject(GstBuffer* data);

	/**
	 * Link the given element to the source, so that "sink" becomes a sink for the source.
	 * @param sink The target sink element for the source.
	 */
	void setSink(GstElement* sink);

	/**
	 * @param maxQueueSize The maximum amount of bytes that can be queued at the source.
	 */
	void setMaxQueueSize(size_t size);

	/**
	 * The maximum default amount of bytes that can be queued at the source.
	 */
	static const size_t MAX_QUEUE_SIZE = 10000000;

protected:
	/**
	 * @Override
	 */
	void notify(InjectablePipelineObserver* observer, const std::string& name, void* data) {
		if (name == "onFeedData") {
			observer->onEnoughData();
		} else if (name == "onNeedData") {
			observer->onNeedData();
		}
	}

	void notify(InjectablePipelineObserver* observer, void* data) {};

	/**
	 * This method is called no more data is needed in the incoming queue (when it's full).
	 * The default behaviour is to inhibit further calls to sfl#InjectablePipeline#inject.
	 */
	void onEnoughData();

	/**
	 * This method is called when the incoming queue is empty and elements downstream need more data.
	 * The default behaviour is to un-inhibit further calls to sfl#InjectablePipeline#inject.
	 */
	void onNeedData();

private:
	/**
	 * Low-level callback to allow dispatching to the actual object, rather than being just
	 * handled at the class level.
	 */
	static void enough_data_cb(GstAppSrc *src, gpointer data);

	/**
	 * Low-level callback to allow dispatching to the actual object, rather than being just
	 * handled at the class level.
	 */
	static void need_data_cb(GstAppSrc *src, guint length, gpointer user_data);

	/**
	 * Helper method for constructors.
	 */
	void init(GstCaps* caps, Pipeline& pipeline, size_t maxQueueSize);

	bool enoughData;
	GstElement* appsrc;
	static unsigned numberInstances;
};

}

#endif
