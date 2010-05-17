#include "videopane.h"
#include "sflphone_const.h"

G_DEFINE_TYPE (VideoPane, video_pane, GTK_TYPE_VBOX)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_VIDEOPANE, VideoPanePrivate))

typedef struct _VideoPanePrivate VideoPanePrivate;

struct _VideoPanePrivate {
  GtkWidget* capture_button;
  GtkWidget* video_cairo;
  GtkWidget* devices_combo;
  GtkListStore* devices_model;
};

static void
video_pane_dispose (GObject *object)
{
  G_OBJECT_CLASS (video_pane_parent_class)->dispose (object);
}

static void
video_pane_finalize (GObject *object)
{
  G_OBJECT_CLASS (video_pane_parent_class)->finalize (object);
}

static void
video_pane_class_init (VideoPaneClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (VideoPanePrivate));

  object_class->dispose = video_pane_dispose;
  object_class->finalize = video_pane_finalize;
}

void capture_cb(GtkWidget *widget, gpointer data)
{
  VideoPanePrivate* priv = GET_PRIVATE((VideoPane *) data);
  gchar * device = gtk_combo_box_get_active_text(priv->devices_combo);

  DEBUG("Starting capture on %s", device);

  video_cairo_set_source(priv->video_cairo, device);
  video_cairo_start(priv->video_cairo);
}

static void
video_pane_init (VideoPane *self)
{
  // Capture button
  VideoPanePrivate* priv = GET_PRIVATE(self);
  priv->capture_button = gtk_button_new_with_label("Capture");

  gtk_signal_connect(GTK_OBJECT (priv->capture_button), "clicked", GTK_SIGNAL_FUNC (capture_cb), (gpointer) self);

  // Cairo video
  priv->video_cairo = video_cairo_new();

  // Device list
  gchar** available_devices = NULL;
  available_devices = dbus_video_enumerate_devices();
  priv->devices_combo =  gtk_combo_box_new_text();

  if (available_devices != NULL)
    {
      char **available_devices_ptr;
      for (available_devices_ptr = available_devices; *available_devices_ptr; available_devices_ptr++)
        {
          DEBUG ("Device %s", *available_devices_ptr);
          gtk_combo_box_append_text (GTK_COMBO_BOX(priv->devices_combo), *available_devices_ptr);
        }
    }

  gtk_box_pack_start(GTK_BOX(self), priv->video_cairo, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(self), priv->devices_combo, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self), priv->capture_button, TRUE, FALSE, 0);

  gtk_widget_show_all(GTK_WIDGET(self));
}

VideoPane*
video_pane_new (void)
{
  return g_object_new (SFL_TYPE_VIDEOPANE, NULL);
}
