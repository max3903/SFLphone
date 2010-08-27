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

#ifndef __SFL_VIDEO_SESSION_H__
#define __SFL_VIDEO_SESSION_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SFL_TYPE_VIDEO_SESSION            (sfl_video_session_get_type ())
#define SFL_VIDEO_SESSION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SFL_TYPE_VIDEO_SESSION, SFLVideoSession))
#define SFL_VIDEO_SESSION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SFL_VIDEO_SESSION, SFLVideoSessionClass))
#define SFL_IS_VIDEO_SESSION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SFL_TYPE_VIDEO_SESSION))
#define SFL_IS_VIDEO_SESSION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SFL_TYPE_VIDEO_SESSION))
#define SFL_GET_VIDEO_SESSION_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), SFL_TYPE_VIDEO_SESSION, SFLVideoSessionClass))

typedef struct _SFLVideoSession SFLVideoSession;
typedef struct _SFLVideoSessionClass SFLVideoSessionClass;

struct _SFLVideoSession
{
  GtkVBox parent;
};

struct _SFLVideoSessionClass
{
  GtkVBoxClass parent_class;
};

GType
sfl_video_session_get_type (void);

/**
 * @return A new video session widget.
 * @param The shared memory segment to read data from.
 */
SFLVideoSession*
sfl_video_session_new (const gchar* shm);

G_END_DECLS

#endif
