#ifndef _VIDEO_PANE_H__
#define _VIDEO_PANE_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SFL_TYPE_VIDEOPANE video_pane_get_type()

#define SFL_VIDEOPANE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), SFL_TYPE_VIDEOPANE, VideoPane))

#define SFL_VIDEOPANE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), SFL_TYPE_VIDEOPANE, VideoPaneClass))

#define SFL_IS_VIDEOPANE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SFL_TYPE_VIDEOPANE))

#define SFL_IS_VIDEOPANE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), SFL_TYPE_VIDEOPANE))

#define SFL_VIDEOPANE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), SFL_TYPE_VIDEOPANE, VideoPaneClass))

typedef struct {
  GtkVBox parent;
} VideoPane;

typedef struct {
  GtkVBoxClass parent_class;
} VideoPaneClass;

GType video_pane_get_type (void);

VideoPane* video_pane_new (void);

G_END_DECLS

#endif /* _VIDEO_PANE */
