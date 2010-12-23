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

#ifndef __SFL_ABSTRACT_VIDEO_CODEC_H__
#define __SFL_ABSTRACT_VIDEO_CODEC_H__

#include "VideoCodec.h"
#include "video/codec/decoder/VideoDecoder.h"
#include "video/codec/encoder/VideoEncoder.h"

#include <map>

namespace sfl
{

/**
 * Forward declaration
 */
class VideoFrame;
class VideoInputSource;

template<class EncoderType, class DecoderType>
class AbstractVideoCodec: public virtual VideoCodec
{
    public:
        AbstractVideoCodec() :
                VideoCodec() {
            // encoder = new EncoderType();
            // decoder = new DecoderType();

        }



        virtual ~AbstractVideoCodec() {
        }
        ;

        /**
         * @Override
         */
        void setVideoInputFormat(const VideoFormat& format) {
        	encoder.setVideoInputFormat(format);
        }

        /**
         * @Override
         */
        VideoFormat getVideoOutputFormat() {
        	return decoder.getOutputFormat();
        }

        /**
         * @Override
         */
        void addVideoFrameEncodedObserver (VideoFrameEncodedObserver& observer) {
            encoder.addObserver (&observer);
        }

        /**
         * @Override
         */
        void addVideoFrameDecodedObserver (VideoFrameDecodedObserver& observer) {
            decoder.addObserver (&observer);
        }

        /**
         * @Override
         */
        inline void decode (Buffer<uint8>& data) throw (VideoDecodingException) {
            decoder.decode (data);
        }

        /**
         * @Override
         */
        void encode (const VideoFrame* frame) throw (VideoEncodingException) {
            encoder.encode (frame);
        }

        /**
         * @Override
         */
        void deactivate() {
            encoder.deactivate();
            decoder.deactivate();
        }

        /**
         * @Override
         */
        void activate() {
            encoder.activate();
            decoder.activate();
        }

        /**
         * @Override
         */
        void setParameter (const std::string& name, const std::string& value) {
            encoder.setParameter (name, value);
            decoder.setParameter (name, value);
        }

        /**
         * @Override
         */
        std::string getParameter (const std::string& name) {
            return encoder.getParameter (name);
        }

    private:
        // Impose constraint on the type. Something that we can do in Java, and other languages, but not that easily in C++.
        typedef typename EncoderType::IsDerivedFromVideoEncoder
        DerivedFromEncoderGuard;

        typedef typename DecoderType::IsDerivedFromVideoDecoder
        DerivedFromDecoderGuard;

        EncoderType encoder;

        DecoderType decoder;

};

}

#endif
