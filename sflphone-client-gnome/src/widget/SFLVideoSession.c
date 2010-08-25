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
  GtkWidget* notebook;
  GtkWidget* spinner;

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

/**
 * Taken from cheese-widget.c
 */
static void
sfl_video_session_widget_spinner_invert (GtkWidget *spinner, GtkWidget *parent)
{
  GtkStyle *style;
  guint     i;

  for (i = GTK_STATE_NORMAL; i <= GTK_STATE_INSENSITIVE; i++)
  {
    GdkColor *fg, *bg;

    style = gtk_widget_get_style (spinner);
    fg    = gdk_color_copy (&style->fg[i]);
    bg    = gdk_color_copy (&style->bg[i]);

    gtk_widget_modify_fg (spinner, i, bg);
    gtk_widget_modify_bg (spinner, i, fg);

    gtk_widget_modify_fg (parent, i, bg);
    gtk_widget_modify_bg (parent, i, fg);

    gdk_color_free (fg);
    gdk_color_free (bg);
  }
}

static void
sfl_video_session_init (SFLVideoSession* self)
{
  SFLVideoSessionPrivate* priv = GET_PRIVATE(self);

  DEBUG("Initializing SFLVideoSession");

  // Create a notebook for hiding different pages
  priv->notebook = gtk_notebook_new ();
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->notebook), FALSE);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (priv->notebook), FALSE);

  // Spinner page
  priv->spinner = gtk_spinner_new ();
  GtkWidget* box = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (box), priv->spinner);
  sfl_video_session_widget_spinner_invert (priv->spinner, box);
  gtk_widget_show_all (box);

  gtk_notebook_append_page (GTK_NOTEBOOK (priv->notebook),
                            box, gtk_label_new ("spinner"));

  // Video page
  priv->remote_video = sfl_video_cairo_shm_new();
  gtk_widget_show (GTK_WIDGET(priv->remote_video));
  gtk_notebook_append_page (GTK_NOTEBOOK (priv->notebook),
                            GTK_WIDGET(priv->remote_video), gtk_label_new ("video"));

  // Problem page
//  priv->problem = gtk_drawing_area_new ();
//  gtk_widget_show (priv->problem);
//  gtk_notebook_append_page (GTK_NOTEBOOK (widget),
//                            priv->problem,
//                            gtk_label_new ("got problems"));

  // Align the notebook
  GtkWidget* align_notebook = gtk_alignment_new(0.5, 0.0, 1.0, 1.0);
  gtk_container_add(GTK_CONTAINER(align_notebook), GTK_WIDGET(priv->notebook));
  gtk_box_pack_start (GTK_BOX(self), align_notebook, FALSE, TRUE, 0);

  // Create the toolbar
  priv->controls = sfl_video_session_control_new ();
  GtkWidget* align_toolbar = gtk_alignment_new(0.5, 0.0, 0.5, 0.0);
  gtk_container_add(GTK_CONTAINER(align_toolbar), GTK_WIDGET(priv->controls));
  gtk_box_pack_start (GTK_BOX(self), align_toolbar, FALSE, TRUE, 0);

  // Start the spinner
  gtk_spinner_start(GTK_SPINNER(priv->spinner));
}

SFLVideoSession*
sfl_video_session_new ()
{
  return g_object_new (SFL_TYPE_VIDEO_SESSION, NULL);
}
