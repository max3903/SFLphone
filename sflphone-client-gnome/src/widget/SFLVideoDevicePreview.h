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

#ifndef __SFL_VIDEO_DEVICE_PREVIEW_H__
#define __SFL_VIDEO_DEVICE_PREVIEW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SFL_TYPE_VIDEO_DEVICE_PREVIEW             (sfl_video_device_preview_get_type ())
#define SFL_VIDEO_DEVICE_PREVIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SFL_TYPE_VIDEO_DEVICE_PREVIEW, SFLVideoDevicePreview))
#define SFL_VIDEO_DEVICE_PREVIEW_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), SFL_VIDEO_DEVICE_PREVIEW, SFLVideoDevicePreviewClass))
#define SFL_IS_VIDEO_DEVICE_PREVIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SFL_TYPE_VIDEO_DEVICE_PREVIEW))
#define SFL_IS_VIDEO_DEVICE_PREVIEW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), EFF_TYPE_CAIRO))
#define SFL_GET_VIDEO_DEVICE_PREVIEW_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), SFL_TYPE_VIDEO_DEVICE_PREVIEW, SFLVideoDevicePreviewClass))

typedef struct _SFLVideoDevicePreview SFLVideoDevicePreview;
typedef struct _SFLVideoDevicePreviewClass SFLVideoDevicePreviewClass;

struct _SFLVideoDevicePreview
{
  GtkDrawingArea parent;
};

struct _SFLVideoDevicePreviewClass
{
  GtkDrawingAreaClass parent_class;
};

GType
sfl_video_device_preview_get_type (void);

/**
 * @return A new video cairo widget, unbound to any source.
 */
SFLVideoDevicePreview*
sfl_video_device_preview_new ();

/**
 * @param source The video source to connect to.
 * @return A new video cairo widget, bound to the given source.
 */
SFLVideoDevicePreview*
sfl_video_device_preview_new_with_source (const gchar* source);

/**
 * @param sfl_video_device_preview The SFLVideoDevicePreview widget to set the source.
 * @param source The video source to connect to.
 */
void
sfl_video_device_preview_set_source (SFLVideoDevicePreview* sfl_video_device_preview, gchar* source);

/**
 * @param sfl_video_device_preview The SFLVideoDevicePreview widget to set the source.
 * @param height The desired and supported height.
 */
void
sfl_video_device_preview_set_capture_width (SFLVideoDevicePreview* sfl_video_device_preview, gint width);

/**
 * @param sfl_video_device_preview The SFLVideoDevicePreview widget to set the source.
 * @param source The video source to connect to.
 */
void
sfl_video_device_preview_set_capture_height (SFLVideoDevicePreview* sfl_video_device_preview, gint height);

/**
 * @param sfl_video_device_preview The SFLVideoDevicePreview widget to set the source.
 * @param fps The framerate of the form "num/denom".
 */
void
sfl_video_device_preview_set_capture_framerate (SFLVideoDevicePreview* sfl_video_device_preview, gchar* fps);

/**
 * @param sfl_video_device_preview The SFLVideoDevicePreview widget on which to start video display.
 * @precondition The source should have been specified.
 */
int
sfl_video_device_preview_start (SFLVideoDevicePreview* sfl_video_device_preview);

/**
 * @param sfl_video_device_preview The SFLVideoDevicePreview widget on which to stop video display.
 * @precondition Video streaming must be active.
 */
int
sfl_video_device_preview_stop (SFLVideoDevicePreview* sfl_video_device_preview);

/**
 * @return TRUE if this widget is capturing (and displaying) video.
 */
gboolean
sfl_video_device_preview_is_capturing(SFLVideoDevicePreview* sfl_video_device_preview);

G_END_DECLS

#endif
