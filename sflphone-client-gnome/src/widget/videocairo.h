/**
 * cairo.h
 *
 * A GTK+ widget that implements a cairo face
 *
 * (c) 2005, Davyd Madeley
 *
 * Authors:
 *   Davyd Madeley  <davyd@madeley.id.au>
 */

#ifndef __VIDEO_CAIRO_H__
#define __VIDEO_CAIRO_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define VIDEO_TYPE_CAIRO             (video_cairo_get_type ())
#define VIDEO_CAIRO(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), VIDEO_TYPE_CAIRO, VideoCairo))
#define VIDEO_CAIRO_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), VIDEO_CAIRO, VideoCairoClass))
#define VIDEO_IS_CAIRO(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VIDEO_TYPE_CAIRO))
#define VIDEO_IS_CAIRO_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), EFF_TYPE_CAIRO))
#define VIDEO_CAIRO_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), VIDEO_TYPE_CAIRO, VideoCairoClass))

typedef struct _VideoCairo            VideoCairo;
typedef struct _VideoCairoClass       VideoCairoClass;

struct _VideoCairo
{
        GtkDrawingArea parent;
};

struct _VideoCairoClass
{
        GtkDrawingAreaClass parent_class;
};

GtkWidget *video_cairo_new (const gchar *source);

G_END_DECLS

#endif
