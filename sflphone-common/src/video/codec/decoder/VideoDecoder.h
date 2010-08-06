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

#ifndef __SFL_VIDEO_DECODER_H__
#define __SFL_VIDEO_DECODER_H__

#include "video/VideoPlugin.h"
#include "video/source/VideoFormat.h"
#include "video/VideoExceptions.h"
#include "video/codec/mime/MimeParameters.h"

#include "util/pattern/AbstractObservable.h"
#include "util/memory/Buffer.h"

namespace sfl
{

/**
 * The client that wants to get access to decoded frame must
 * implement this interface.
 */
class VideoFrameDecodedObserver : public Observer
{
    public:
        /**
         * @param frame The new frame that was decoded.
         */
        virtual void onNewFrameDecoded (Buffer<uint8_t>& data) = 0;
};

/**
 * Abstract base class for every video decoder.
 */
class VideoDecoder : public virtual MimeParameters, public VideoPlugin, public AbstractObservable<Buffer<uint8_t>&, VideoFrameDecodedObserver>
{
    public:
        /**
         * @param buffer A buffer containing the depayloaded data.
         * @throw VideoDecodingException if the frame cannot be decoded.
         */
        virtual void decode (Buffer<uint8>& data)
        throw (VideoDecodingException) = 0;

        /**
         * @format The VideoFormat describing the desired output format in which to retrieve the decoded video frames.
         */
        virtual void setOutputFormat (VideoFormat& format) = 0;

        typedef int IsDerivedFromVideoDecoder;

    protected:
        /**
         * Simple dispatch for the VideoFrameDecodedObserver type.
         * @Override
         */
        void notify (VideoFrameDecodedObserver* observer, Buffer<uint8_t>& data) {
            observer->onNewFrameDecoded (data);
        }

        // FIXME Should not need to do that.
        void notify (VideoFrameDecodedObserver* observer, const std::string& name, Buffer<uint8_t>& data) {}

        VideoDecoder() {};

        inline virtual ~VideoDecoder() {}
};

}

#endif /*DECODER_H_*/
