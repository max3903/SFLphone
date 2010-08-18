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

#ifndef __SFL_PIPELINE_H__
#define __SFL_PIPELINE_H__

#include "video/VideoExceptions.h"
#include <gst/gst.h>

namespace sfl
{
class Pipeline
{
    public:
        /**
         * Create a new pipeline object as well as the underlying gstreamer pipeline.
         * @param name The name for the new pipeline to create.
         */
        Pipeline (const std::string& name) throw (GstException);

        /**
         * @param middle Part of the pipeline in which the data will flow through from the source.
         * @postcondition sfl#Pipeline#start and sfl#Pipeline#stop will act upon the specified pipeline.
         */
        Pipeline (GstElement* pipeline);

        virtual ~Pipeline() {};

        /**
         * @postcondition The pipeline will be in state PLAYING.
         */
        void start() throw (GstException);

        /**
         * @postcondition The pipeline will be in state PAUSED.
         */
        void stop() throw (GstException);

        /**
         * @param factoryName a named factory to instantiate
         * @return A pointer to the newly created filter.
         * @postcondition A new element of the given type is added to the bin.
         */
        GstElement* addElement (const std::string& factoryName)
        throw (MissingGstPluginException, GstException);

        /**
         * @param factoryName a named factory to instantiate
         * @param pad A source pad to link to.
         * @return A pointer to the newly created filter.
         * @postcondition A new element of the given type is added to the bin, and linked to the provided pad.
         */
        GstElement* addElement (const std::string& factoryName,
                                GstPad* pad) throw (MissingGstPluginException, GstException);

        /**
         * @param factoryName a named factory to instantiate
         * @param previous A source element to link to.
         * @return A pointer to the newly created filter.
         * @postcondition A new element of the given type is added to the bin, and linked to the given element.
         */
        GstElement* addElement (const std::string& factoryName,
                                GstElement* previous) throw (MissingGstPluginException,
                                                             GstException);
        /**
         * @param teeName The unique identifier for this tee element.
         * @param previous A source element to link to.
         * @throw GstException if the tee cannot be added to the bin and linked.
         * @postcondition A new element of the given type is added to the bin.
         */
        void addTee (const std::string& teeName) throw (GstException) ;

        /**
         * @param teeName The unique identifier for this tee element.
         * @param previous A source element to link to.
         * @throw GstException if the tee cannot be added to the bin and linked.
         * @postcondition A new element of the given type is added to the bin, and linked to the given element.
         */
        void addTee (const std::string& teeName, GstElement* previous) throw (GstException) ;

        /**
         * @param teeName The unique identifier for this tee element.
         * @param pad A source pad to link to.
         * @throw GstException if the tee cannot be added to the bin and linked.
         * @postcondition A new element of the given type is added to the bin, and linked to the provided pad.
         */
        void addTee (const std::string& teeName, GstPad* pad) throw (GstException);

        /**
         * @param prefix A prefix to append to every element created.
         * @postcondition Every new element created from that moment on will be named of the form "PREFIX FACTORY_NAME"
         */
        void setPrefix (const std::string& prefix);

        /**
         * @param src The source element to link from.
         * @param dst The destination element to link to.
         * @throw GstException if the two elements can't be linked together.
         */
        void link (GstElement* src, GstElement* dst) throw (GstException);

        /**
         * @param src A source pad to link from.
         * @param dst The destination element to link to on its static pad.
         * @throw GstException if the two elements can't be linked together.
         */
        void link (GstPad* src, GstElement* dst) throw (GstException);

        /**
         * @param src A source element to link from
         * @param dst The destination pad to link to.
         * @throw GstException if the two elements can't be linked together.
         */
        void link (GstElement* src, GstPad* dst) throw (GstException);

        /**
         * @param src A source pad to link from.
         * @param dst A destination pad to link to.
         * @throw GstException if the two elements can't be linked together.
         */
        void link (GstPad* src, GstPad* dst) throw (GstException);

        /**
         * Branch at the given "tee".
         * @param teeName the name of an existing tee element.
         * @return A GstPad for the new source on which to connect further elements downstream.
         */
        GstPad* branch (const std::string& teeName);

        /**
         * @return The underlying gstreamer Pipeline.
         */
        GstElement* getGstPipeline();

        static const int MAX_BUS_POOL_WAIT = 10;

    private:
        GstElement* pipeline;

        /**
         * @param factoryName a named factory to instantiate
         * @return A pointer to the newly created filter.
         * @throw MissingGstPluginException if the given plugin identified by factoryName cannot be found.
         * @throw GstException if the given plugin can't be added to the bin.
         */
        GstElement* createElement (const std::string& factoryName) throw (MissingGstPluginException, GstException);

        /**
         * Helper method for constructors.
         */
        void init (GstElement* pipeline);

        std::string prefix;

        static unsigned UNIQUE_COUNTER_NAME;
};
}

#endif
