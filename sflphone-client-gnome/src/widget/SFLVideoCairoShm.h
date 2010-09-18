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

#ifndef __SFL_VIDEO_CAIRO_SHM_H__
#define __SFL_VIDEO_CAIRO_SHM_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SFL_TYPE_VIDEO_CAIRO_SHM              (sfl_video_cairo_shm_get_type ())
#define SFL_VIDEO_CAIRO_SHM(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), SFL_TYPE_VIDEO_CAIRO_SHM, SFLVideoCairoShm))
#define SFL_VIDEO_CAIRO_SHM_CLASS(obj)        (G_TYPE_CHECK_CLASS_CAST ((obj), SFL_VIDEO_CAIRO_SHM, SFLVideoCairoShmClass))
#define SFL_IS_VIDEO_CAIRO_SHM(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SFL_TYPE_VIDEO_CAIRO_SHM))
#define SFL_IS_VIDEO_CAIRO_SHM_CLASS(obj)     (G_TYPE_CHECK_CLASS_TYPE ((obj), SFL_TYPE_VIDEO_CAIRO_SHM))
#define SFL_GET_VIDEO_CAIRO_SHM_CLASS         (G_TYPE_INSTANCE_GET_CLASS ((obj), SFL_TYPE_VIDEO_CAIRO_SHM, SFLVideoCairoShmClass))

typedef struct _SFLVideoCairoShm SFLVideoCairoShm;
typedef struct _SFLVideoCairoShmClass SFLVideoCairoShmClass;

struct _SFLVideoCairoShm
{
  GtkDrawingArea parent;
};

struct _SFLVideoCairoShmClass
{
  GtkDrawingAreaClass parent_class;
};

GType
sfl_video_cairo_shm_get_type (void);

/**
 * Start reading and displaying frames from the shared memory segment.
 * @return < 0 on failure, >= 0 otherwise.
 */
int
sfl_video_cairo_shm_start (SFLVideoCairoShm* self);

/**
 * Start reading and displaying frames from the shared memory segment.
 * @return < 0 on failure, >= 0 otherwise.
 * @postcondition The allocated structures will be freed and will only be re-established through #sfl_video_cairo_shm_start.
 */
int
sfl_video_cairo_shm_stop (SFLVideoCairoShm* self);

/**
 * Save the current image buffer to a PNG file.
 * @param filename The filename for the new image file.
 */
int
sfl_video_cairo_shm_take_snapshot(SFLVideoCairoShm* self, gchar* filename);

/**
 * @return A new video cairo widget, unbound to any source.
 * @param The shared memory segment to read data from.
 */
SFLVideoCairoShm*
sfl_video_cairo_shm_new (const gchar* shm);

G_END_DECLS

#endif
