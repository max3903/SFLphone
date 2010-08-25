#ifndef __SFL_VIDEO_CAIRO_H__
#define __SFL_VIDEO_CAIRO_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SFL_VIDEO_TYPE_CAIRO             (sfl_video_cairo_get_type ())
#define SFL_VIDEO_CAIRO(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SFL_VIDEO_TYPE_CAIRO, SFLVideoCairo))
#define SFL_VIDEO_CAIRO_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), SFL_VIDEO_CAIRO, SFLVideoCairoClass))
#define SFL_VIDEO_IS_CAIRO(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SFL_VIDEO_TYPE_CAIRO))
#define SFL_VIDEO_IS_CAIRO_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), EFF_TYPE_CAIRO))
#define SFL_VIDEO_CAIRO_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), SFL_VIDEO_TYPE_CAIRO, SFLVideoCairoClass))

typedef struct _SFLVideoCairo SFLVideoCairo;
typedef struct _SFLVideoCairoClass SFLVideoCairoClass;

struct _SFLVideoCairo
{
  GtkDrawingArea parent;
};

struct _SFLVideoCairoClass
{
  GtkDrawingAreaClass parent_class;
};

GType
sfl_video_cairo_get_type (void);

/**
 * @return A new video cairo widget, unbound to any source.
 */
SFLVideoCairo*
sfl_video_cairo_new ();

/**
 * @param source The video source to connect to.
 * @return A new video cairo widget, bound to the given source.
 */
SFLVideoCairo*
sfl_video_cairo_new_with_source (const gchar* source);

/**
 * @param sfl_video_cairo The SFLVideoCairo widget to set the source.
 * @param source The video source to connect to.
 */
void
sfl_video_cairo_set_source (SFLVideoCairo* sfl_video_cairo, gchar* source);

/**
 * @param sfl_video_cairo The SFLVideoCairo widget to set the source.
 * @param height The desired and supported height.
 */
void
sfl_video_cairo_set_capture_width (SFLVideoCairo* sfl_video_cairo, gint width);

/**
 * @param sfl_video_cairo The SFLVideoCairo widget to set the source.
 * @param source The video source to connect to.
 */
void
sfl_video_cairo_set_capture_height (SFLVideoCairo* sfl_video_cairo, gint height);

/**
 * @param sfl_video_cairo The SFLVideoCairo widget to set the source.
 * @param fps The framerate of the form "num/denom".
 */
void
sfl_video_cairo_set_capture_framerate (SFLVideoCairo* sfl_video_cairo, gchar* fps);

/**
 * @param sfl_video_cairo The SFLVideoCairo widget on which to start video display.
 * @precondition The source should have been specified.
 */
int
sfl_video_cairo_start (SFLVideoCairo* sfl_video_cairo);

/**
 * @param sfl_video_cairo The SFLVideoCairo widget on which to stop video display.
 * @precondition Video streaming must be active.
 */
int
sfl_video_cairo_stop (SFLVideoCairo* sfl_video_cairo);

/**
 * @return TRUE if this widget is capturing (and displaying) video.
 */
gboolean
sfl_video_cairo_is_capturing(SFLVideoCairo* sfl_video_cairo);

G_END_DECLS

#endif
