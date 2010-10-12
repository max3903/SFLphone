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
#ifndef __SFL_VIDEO_EXCEPTIONS_H__
#define __SFL_VIDEO_EXCEPTIONS_H__

#include <stdexcept>

namespace sfl
{
/**
 * This exception is thrown when a video frame cannot be decoded.
 */
class VideoDecodingException: public std::runtime_error
{
    public:
        VideoDecodingException (const std::string& msg) :
                std::runtime_error (msg) {
        }
};

/**
 * This exception is thrown when a video frame cannot be encoded.
 */
class VideoEncodingException: public std::runtime_error
{
    public:
        VideoEncodingException (const std::string& msg) :
                std::runtime_error (msg) {
        }
};

/**
 * This exception is thrown when a video codec cannot be found.
 * TODO This kind of exception is general and would benefit from being moved
 * outside of this file into a class-neutral and separated file. It could
 * also be made a supertype of MissingGstPluginException.
 */
class MissingPluginException: public std::runtime_error
{
    public:
        MissingPluginException (const std::string& msg) :
                std::runtime_error (msg) {
        }
};

/**
 * This exception is thrown when an expected plugin is missing from gstreamer.
 */
class MissingGstPluginException: public MissingPluginException
{
    public:
        MissingGstPluginException (const std::string& msg) :
                MissingPluginException (msg) {
        }
};

/**
 * This exception is thrown when a gstreamer exception occurs.
 */
class GstException: public std::runtime_error
{
    public:
        GstException (const std::string& msg) :
                std::runtime_error (msg) {
        }
};

}

#endif
