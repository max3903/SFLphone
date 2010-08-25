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
 */
SFLVideoSession*
sfl_video_session_new ();

G_END_DECLS

#endif
