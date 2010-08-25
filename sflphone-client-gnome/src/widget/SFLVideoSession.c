#include <gtk/gtk.h>

#include "SFLVideoSession.h"
#include "SFLVideoCairoShm.h"
#include "SFLVideoSessionControl.h"

#include "sflphone_const.h"

G_DEFINE_TYPE (SFLVideoSession, sfl_video_session, GTK_TYPE_VBOX)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_VIDEO_SESSION, SFLVideoSessionPrivate))

typedef struct _SFLVideoSessionPrivate SFLVideoSessionPrivate;
struct _SFLVideoSessionPrivate
{
  SFLVideoCairoShm* remote_video;
  SFLVideoSessionControl* controls;
};

enum
{
  PROP_SOURCE = 1, PROP_WIDTH, PROP_HEIGHT, PROP_FPS, LAST_PROPERTY
} SFLVideoSessionProperties;

static void
sfl_video_session_set_property (GObject *object, guint property_id,
    const GValue *value, GParamSpec *pspec)
{
  SFLVideoSession* self = SFL_VIDEO_SESSION(object);
  SFLVideoSessionPrivate* priv = GET_PRIVATE(self);

  switch (property_id)
    {
  case PROP_SOURCE:
    DEBUG("Setting source %s", g_value_get_string (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    return;
    }
}

static void
sfl_video_session_get_property (GObject *object, guint property_id,
    GValue *value, GParamSpec *pspec)
{
  SFLVideoSession* self = SFL_VIDEO_SESSION(object);
  SFLVideoSessionPrivate* priv = GET_PRIVATE(self);

  switch (property_id)
    {
  case PROP_SOURCE:
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
sfl_video_session_class_init (SFLVideoSessionClass *class)
{
  GObjectClass *obj_class;
  GtkWidgetClass *widget_class;

  obj_class = G_OBJECT_CLASS (class);
  obj_class->get_property = sfl_video_session_get_property;
  obj_class->set_property = sfl_video_session_set_property;

  g_object_class_install_property (obj_class, PROP_SOURCE, g_param_spec_string (
      "source", "source", "String specifying the source SHM for the video",
      NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
          | G_PARAM_READWRITE));

  g_type_class_add_private (obj_class, sizeof(SFLVideoSessionPrivate));
}

static void
sfl_video_session_init (SFLVideoSession* self)
{
  SFLVideoSessionPrivate* priv = GET_PRIVATE(self);

  DEBUG("Initializing SFLVideoSession");

  // Create the cairo object
  priv->remote_video = sfl_video_cairo_shm_new();
  GtkWidget* align_video = gtk_alignment_new(0.5, 0.0, 1.0, 1.0);
  gtk_container_add(GTK_CONTAINER(align_video), GTK_WIDGET(priv->remote_video));
  gtk_box_pack_start (GTK_BOX(self), align_video, FALSE, TRUE, 0);

  // Create the toolbar
  priv->controls = sfl_video_session_control_new ();
  GtkWidget* align_toolbar = gtk_alignment_new(0.5, 0.0, 0.5, 0.0);
  gtk_container_add(GTK_CONTAINER(align_toolbar), GTK_WIDGET(priv->controls));
  gtk_box_pack_start (GTK_BOX(self), align_toolbar, FALSE, TRUE, 0);
}

SFLVideoSession*
sfl_video_session_new ()
{
  return g_object_new (SFL_TYPE_VIDEO_SESSION, NULL);
}
