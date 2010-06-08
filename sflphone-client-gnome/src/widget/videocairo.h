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

typedef struct _VideoCairo VideoCairo;
typedef struct _VideoCairoClass VideoCairoClass;

struct _VideoCairo
{
  GtkDrawingArea parent;
};

struct _VideoCairoClass
{
  GtkDrawingAreaClass parent_class;
};

GType
video_cairo_get_type (void);

/**
 * @return A new video cairo widget, unbound to any source.
 */
VideoCairo*
video_cairo_new ();

/**
 * @param source The video source to connect to.
 * @return A new video cairo widget, bound to the given source.
 */
VideoCairo*
video_cairo_new_with_source (const gchar* source);

/**
 * @param video_cairo The VideoCairo widget to set the source.
 * @param source The video source to connect to.
 */
void
video_cairo_set_source (VideoCairo* video_cairo, gchar* source);

/**
 * @param video_cairo The VideoCairo widget to set the source.
 * @param height The desired and supported height.
 */
void
video_cairo_set_capture_width (VideoCairo* video_cairo, gint width);

/**
 * @param video_cairo The VideoCairo widget to set the source.
 * @param source The video source to connect to.
 */
void
video_cairo_set_capture_height (VideoCairo* video_cairo, gint height);

/**
 * @param video_cairo The VideoCairo widget to set the source.
 * @param fps The framerate of the form "num/denom".
 */
void
video_cairo_set_capture_framerate (VideoCairo* video_cairo, gchar* fps);

/**
 * @param video_cairo The VideoCairo widget on which to start video display.
 * @precondition The source should have been specified.
 */
int
video_cairo_start (VideoCairo* video_cairo);

/**
 * @param video_cairo The VideoCairo widget on which to stop video display.
 * @precondition Video streaming must be active.
 */
int
video_cairo_stop (VideoCairo* video_cairo);

G_END_DECLS

#endif
