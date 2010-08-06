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
#ifndef __SFL_NULL_DECODER_H__
#define __SFL_NULL_DECODER_H__

#include "VideoDecoder.h"

#include "video/codec/mime/MimeParametersNull.h"

namespace sfl
{
/**
 * Null object pattern for the VideoDecoder class of objects.
 */
class NullDecoder: public MimeParametersNull, public VideoDecoder
{
    public:
        NullDecoder() : VideoDecoder() {}

        virtual ~NullDecoder() {}

        /**
         * @Override
         */
        void decode (Buffer<uint8>& data) throw (VideoDecodingException) {
            _error ("No decoder for decoding %d bytes of data", data.getSize());
        }

        /**
         * @Override
         */
        void setOutputFormat (VideoFormat& format) {
        }


        /**
         * @Override
         */
        void activate() {
            _warn ("Activating the NullDecoder");
        }


        /**
         * @Override
         */
        void deactivate() {
            _warn ("Deactivating the NullDecoder");
        }
};
}

#endif
