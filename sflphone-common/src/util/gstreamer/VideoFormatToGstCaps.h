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
#ifndef __SFL_VIDEO_FORMAT_TO_GST_CAPS_H__
#define __SFL_VIDEO_FORMAT_TO_GST_CAPS_H__

#include "video/source/VideoFormat.h"

#include <functional>

#include <gst/gst.h>
#include <gst/video/video.h>

namespace sfl
{

struct VideoFormatToGstCaps : public std::unary_function<VideoFormat&, GstCaps*> {
    /**
     * gst_video_format_from_fourcc has only partial definition.
     * @param fourcc The 32 bits encoding of the fourcc code.
     * @return The GstVideoFormat corresponding to the fourcc code.
     */
    GstVideoFormat GstVideoFormatFromFourcc (guint32 fourcc) {
        switch (fourcc) {
            case GST_MAKE_FOURCC ('I', '4', '2', '0') :
                return GST_VIDEO_FORMAT_I420;
            case GST_MAKE_FOURCC ('Y', 'V', '1', '2') :
                return GST_VIDEO_FORMAT_YV12;
            case GST_MAKE_FOURCC ('Y', 'U', 'Y', '2') :
                return GST_VIDEO_FORMAT_YUY2;
            case GST_MAKE_FOURCC ('Y', 'V', 'Y', 'U') :
                return GST_VIDEO_FORMAT_YVYU;
            case GST_MAKE_FOURCC ('U', 'Y', 'V', 'Y') :
                return GST_VIDEO_FORMAT_UYVY;
            case GST_MAKE_FOURCC ('A', 'Y', 'U', 'V') :
                return GST_VIDEO_FORMAT_AYUV;
            case GST_MAKE_FOURCC ('Y', '4', '1', 'B') :
                return GST_VIDEO_FORMAT_Y41B;
            case GST_MAKE_FOURCC ('Y', '4', '2', 'B') :
                return GST_VIDEO_FORMAT_Y42B;
            case GST_MAKE_FOURCC ('Y', '4', '4', '4') :
                return GST_VIDEO_FORMAT_Y444;
            case GST_MAKE_FOURCC ('v', '2', '1', '0') :
                return GST_VIDEO_FORMAT_v210;
            case GST_MAKE_FOURCC ('v', '2', '1', '6') :
                return GST_VIDEO_FORMAT_v216;
            case GST_MAKE_FOURCC ('N', 'V', '1', '2') :
                return GST_VIDEO_FORMAT_NV12;
            case GST_MAKE_FOURCC ('N', 'V', '2', '1') :
                return GST_VIDEO_FORMAT_NV21;
            case GST_MAKE_FOURCC ('R', 'G', 'B', 'x') :
                return GST_VIDEO_FORMAT_RGBx;
            case GST_MAKE_FOURCC ('B', 'G', 'R', 'x') :
                return GST_VIDEO_FORMAT_BGRx;
            case GST_MAKE_FOURCC ('x', 'R', 'G', 'B') :
                return GST_VIDEO_FORMAT_xRGB;
            case GST_MAKE_FOURCC ('x', 'B', 'G', 'R') :
                return GST_VIDEO_FORMAT_xBGR;
            case GST_MAKE_FOURCC ('R', 'G', 'B', 'A') :
                return GST_VIDEO_FORMAT_RGBA;
            case GST_MAKE_FOURCC ('B', 'G', 'R', 'A') :
                return GST_VIDEO_FORMAT_BGRA;
            case GST_MAKE_FOURCC ('A', 'R', 'G', 'B') :
                return GST_VIDEO_FORMAT_ARGB;
            case GST_MAKE_FOURCC ('A', 'B', 'G', 'R') :
                return GST_VIDEO_FORMAT_ABGR;
            case GST_MAKE_FOURCC ('R', 'G', 'B', ' ') :
                return GST_VIDEO_FORMAT_RGB;
            case GST_MAKE_FOURCC ('B', 'G', 'R', ' ') :
                return GST_VIDEO_FORMAT_BGR;
            default:
                return GST_VIDEO_FORMAT_UNKNOWN;
        }
    }

    GstCaps* operator() (VideoFormat& format) {
        FrameRate rate = format.getPreferredFrameRate();
        guint32 fourcc = GST_STR_FOURCC (format.getFourcc().c_str());
        GstVideoFormat fmt = GstVideoFormatFromFourcc (fourcc);
        _debug("******** ROW STRIDE %d", gst_video_format_get_row_stride(fmt, 0, format.getWidth()));
        _debug("Width %d height %d", format.getWidth(), format.getHeight());
        return gst_video_format_new_caps (fmt, format.getWidth(),
                                          format.getHeight(), rate.getNumerator(), rate.getDenominator(),
                                          1, 1);
    }
};

}
#endif
