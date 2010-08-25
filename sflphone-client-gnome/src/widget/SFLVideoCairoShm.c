#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "SFLVideoCairoShm.h"
#include "util/video_endpoint.h"
#include "sflphone_const.h"

G_DEFINE_TYPE (SFLVideoCairoShm, sfl_video_cairo_shm, GTK_TYPE_DRAWING_AREA)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_VIDEO_CAIRO_SHM, SFLVideoCairoShmPrivate))

typedef struct _SFLVideoCairoShmPrivate SFLVideoCairoShmPrivate;
struct _SFLVideoCairoShmPrivate
{
  cairo_surface_t* surface;
  unsigned char* image_data;
  int image_stride;
  gchar* source;
  gchar* fps;
  gint width;
  gint height;
  gboolean capturing;
  sflphone_video_endpoint_t* endpt;
};

enum
{
  PROP_SOURCE = 1, PROP_WIDTH, PROP_HEIGHT, PROP_FPS, LAST_PROPERTY
} SFLVideoCairoShmProperties;

static void
sfl_video_cairo_shm_set_property (GObject *object, guint property_id,
    const GValue *value, GParamSpec *pspec)
{
  SFLVideoCairoShm *self = SFL_VIDEO_CAIRO_SHM(object);
  SFLVideoCairoShmPrivate *priv = GET_PRIVATE(self);

  DEBUG("Setting property.");

  switch (property_id)
    {
  case PROP_SOURCE:
    DEBUG("Setting source %s", g_value_get_string (value));
    sflphone_video_set_device (priv->endpt, priv->source);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    return;
    }
}

static void
sfl_video_cairo_shm_get_property (GObject *object, guint property_id,
    GValue *value, GParamSpec *pspec)
{
  SFLVideoCairoShm *self = SFL_VIDEO_CAIRO_SHM(object);
  SFLVideoCairoShmPrivate *priv = GET_PRIVATE(self);

  switch (property_id)
    {
  case PROP_SOURCE:
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static char*
get_timestamp ()
{
  time_t rawtime;
  time (&rawtime);

  struct tm* timeinfo = localtime (&rawtime);

  static char output[80];
  strftime (output, 80, "%d-%m-%Y-%H-%M-%S", timeinfo);

  return output;
}

static void
cairo_dump_buffer (guchar* pucPixelBuffer, /* pointer to image-data */
gint iWidth, /* width of image        */
gint iHeight /* height of image       */)
{
  cairo_surface_t* pSurface = NULL;
  cairo_status_t status = CAIRO_STATUS_SUCCESS;

  /* sanity check */
  g_assert (pucPixelBuffer != NULL);
  g_assert (iHeight != 0);

  /* create surface from supplied texture-data */
  pSurface = cairo_image_surface_create_for_data (pucPixelBuffer,
      CAIRO_FORMAT_RGB24, iWidth, iHeight, iWidth);
  /* check for errors */
  status = cairo_surface_status (pSurface);
  if (status != CAIRO_STATUS_SUCCESS)
    {
      ERROR("While creating cairo surface for dumping image to png : (%s)", cairo_status_to_string (status));
      return;
    }

  gchar* filename =
      g_strconcat ("buffer-dump-", get_timestamp (), ".png", NULL);
  DEBUG("Writing filename : %s", filename);
  cairo_surface_write_to_png (pSurface, filename);
  g_free (filename);

  cairo_surface_destroy (pSurface);
}

static void
on_new_frame_cb (uint8_t* frame, void* widget)
{
  // DEBUG("Got frame");

  SFLVideoCairoShmPrivate* priv = GET_PRIVATE((SFLVideoCairoShm*) widget);

  // Copy the frame into the image surface
  //  /memcpy (priv->image_data, frame, priv->width * priv->height * DEFAULT_BPP);

  gtk_widget_queue_draw (GTK_WIDGET(widget));
  //sfl_video_cairo_shm_redraw_canvas((SFLVideoCairoShm*) widget);
}

static void
sfl_video_cairo_shm_finalize (GObject *object)
{
  SFLVideoCairoShm *self = SFL_VIDEO_CAIRO_SHM(object);
  SFLVideoCairoShmPrivate *priv = GET_PRIVATE(self);

  free (priv->image_data);
  cairo_surface_destroy (priv->surface);
  g_free (priv->source);

  G_OBJECT_CLASS (sfl_video_cairo_shm_parent_class)->finalize (object);
}

static gboolean
sfl_video_cairo_shm_expose (GtkWidget* cairo_video, GdkEventExpose* event)
{
  SFLVideoCairoShmPrivate* priv = GET_PRIVATE (cairo_video);

  cairo_t* cr = gdk_cairo_create (cairo_video->window);

  cairo_rectangle(cr, 0, 0, event->area.width, event->area.height);
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_fill (cr);

  cairo_destroy (cr);

  return FALSE;
}

static void
sfl_video_cairo_shm_class_init (SFLVideoCairoShmClass *class)
{
  GObjectClass *obj_class;
  GtkWidgetClass *widget_class;

  obj_class = G_OBJECT_CLASS (class);
  obj_class->get_property = sfl_video_cairo_shm_get_property;
  obj_class->set_property = sfl_video_cairo_shm_set_property;
  obj_class->finalize = sfl_video_cairo_shm_finalize;

  widget_class = GTK_WIDGET_CLASS (class);
  widget_class->expose_event = sfl_video_cairo_shm_expose;

  g_object_class_install_property (obj_class, PROP_SOURCE, g_param_spec_string (
      "source", "source", "String specifying the source SHM for the video",
      NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
          | G_PARAM_READWRITE));

  g_type_class_add_private (obj_class, sizeof(SFLVideoCairoShmPrivate));
}

static void
sfl_video_cairo_shm_init (SFLVideoCairoShm *self)
{
  SFLVideoCairoShmPrivate *priv = GET_PRIVATE(self);

  gtk_widget_set_size_request (GTK_WIDGET(self), 320, 240);

  DEBUG("Initializing video cairo shm widget ...");

  priv->endpt = sflphone_video_init();

}

SFLVideoCairoShm*
sfl_video_cairo_shm_new ()
{
  return g_object_new (SFL_TYPE_VIDEO_CAIRO_SHM, NULL);
}
