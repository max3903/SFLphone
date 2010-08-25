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
 * @return A new video cairo widget, unbound to any source.
 */
SFLVideoCairoShm*
sfl_video_cairo_shm_new ();

G_END_DECLS

#endif
