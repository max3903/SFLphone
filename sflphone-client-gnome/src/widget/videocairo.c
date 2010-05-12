#include <gtk/gtk.h>
#include "videocairo.h"

#define VIDEO_CAIRO_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), VIDEO_TYPE_CAIRO, VideoCairoPrivate))

typedef struct _VideoCairoPrivate VideoCairoPrivate;
struct _VideoCairoPrivate
{
  cairo_surface_t* image;
  gchar* source;
};

G_DEFINE_TYPE (VideoCairo, video_cairo, GTK_TYPE_DRAWING_AREA)
;
static gboolean
video_cairo_expose (GtkWidget *cairo, GdkEventExpose *event);

enum
{
  PROP_SOURCE = 1, LAST_PROPERTY
} HazeConnectionProperties;

static void
video_cairo_set_property (GObject *object, guint property_id,
    const GValue *value, GParamSpec *pspec)
{
  VideoCairo *self = VIDEO_CAIRO(object);
  VideoCairoPrivate *priv = VIDEO_CAIRO_GET_PRIVATE(self);

  switch (property_id)
    {
  case PROP_SOURCE:
    priv->source = g_strdup (g_value_get_string (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
video_cairo_get_property (GObject *object, guint property_id, GValue *value,
    GParamSpec *pspec)
{
  VideoCairo *self = VIDEO_CAIRO(object);
  VideoCairoPrivate *priv = VIDEO_CAIRO_GET_PRIVATE(self);

  switch (property_id)
    {
  case PROP_SOURCE:
    g_value_set_string (value, priv->source);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

void
video_cairo_set_source (VideoCairo * video_cairo, const char *source)
{
  g_object_set (G_OBJECT(video_cairo), "source", source, NULL);
}

static void
video_cairo_init (VideoCairo *self)
{
  VideoCairoPrivate *priv = VIDEO_CAIRO_GET_PRIVATE(self);

  priv->image
      = cairo_image_surface_create_from_png (
          "/home/pierre-luc/workspace/sflphone/sflphone-client-gnome/src/widget/image.png");
}

static void
video_cairo_finalize (GObject *object)
{
  VideoCairo *self = VIDEO_CAIRO(object);
  VideoCairoPrivate *priv = VIDEO_CAIRO_GET_PRIVATE(self);

  g_free(priv->source);
  g_free (priv);

  G_OBJECT_CLASS (video_cairo_parent_class)->finalize (object);
}

static void
video_cairo_class_init (VideoCairoClass *class)
{
  GObjectClass *obj_class;
  GtkWidgetClass *widget_class;

  obj_class = G_OBJECT_CLASS (class);
  obj_class->get_property = video_cairo_get_property;
  obj_class->set_property = video_cairo_set_property;
  obj_class->finalize = video_cairo_finalize;

  widget_class = GTK_WIDGET_CLASS (class);
  widget_class->expose_event = video_cairo_expose;

  GParamSpec *param_spec;
  param_spec = g_param_spec_string ("source", "source",
      "String specifying the source SHM for the video", NULL,
      G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
          | G_PARAM_READWRITE);
  g_object_class_install_property (obj_class, PROP_SOURCE, param_spec);

  g_type_class_add_private (obj_class, sizeof (VideoCairoPrivate));

}

static gboolean
video_cairo_expose (GtkWidget *cairo, GdkEventExpose *event)
{
  // Redraw on every expose event.
  cairo_t *cr;

  cr = gdk_cairo_create (cairo->window);

  VideoCairoPrivate *priv;
  priv = VIDEO_CAIRO_GET_PRIVATE (cairo);

  cairo_set_source_surface (cr, priv->image, event->area.x, event->area.y);
  cairo_paint (cr);
  cairo_destroy (cr);

  return FALSE;
}

GtkWidget *
video_cairo_new (const gchar *source)
{
  return g_object_new (VIDEO_TYPE_CAIRO, "source", source, NULL);
}
