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

#include "GstEncoderH264.h"

namespace sfl
{

void GstEncoderH264::buildFilter (Pipeline& pipeline)
throw (MissingPluginException)
{

    x264enc = pipeline.addElement ("x264enc");

    // Generate byte stream format of NALU
    g_object_set (G_OBJECT (x264enc), "byte-stream", TRUE, NULL);

    // Enable automatic multithreading
    g_object_set (G_OBJECT (x264enc), "threads", 0, NULL);

    // Set default bitrate
    g_object_set (G_OBJECT (x264enc), "bitrate", 300, NULL);

    rtph264pay = pipeline.addElement ("rtph264pay", x264enc);

}

GstElement* GstEncoderH264::getHead()
{
    return x264enc;
}

GstElement* GstEncoderH264::getTail()
{
    return rtph264pay;
}

}
