/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */
#include "videoconf.h"
#include "sflphone_const.h"

G_DEFINE_TYPE (VideoConf, video_conf, GTK_TYPE_VBOX)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_VIDEOCONF, VideoConfPrivate))

typedef struct _VideoConfPrivate VideoConfPrivate;

struct _VideoConfPrivate
{
  GtkWidget* capture_button;
  GtkWidget* video_cairo;

  GtkListStore* resolutions_store;
  GtkWidget* resolutions_combo;

  GtkListStore* framerates_store;
  GtkWidget* framerates_combo;

  GtkWidget* devices_combo;
  GtkListStore* devices_store;
};

static void
video_conf_dispose (GObject *object)
{
  G_OBJECT_CLASS (video_conf_parent_class)->dispose (object);
}

static void
video_conf_finalize (GObject *object)
{
  G_OBJECT_CLASS (video_conf_parent_class)->finalize (object);
}

static void
video_conf_class_init (VideoConfClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof(VideoConfPrivate));

  object_class->dispose = video_conf_dispose;
  object_class->finalize = video_conf_finalize;
}

static void
on_resolutions_combo_changed_cb (GtkWidget* widget, gpointer self)
{
  VideoConfPrivate* priv = GET_PRIVATE((VideoConf*) self);

  DEBUG("Callback on_resolutions_combo_changed_cb");

  gchar* device_name;
  resolution_t* resolution;

  GtkTreeModel* model = gtk_combo_box_get_model (
      GTK_COMBO_BOX(priv->devices_combo));
  GtkTreeIter iter;
  gtk_combo_box_get_active_iter (GTK_COMBO_BOX(priv->devices_combo),
      &iter);
  gtk_tree_model_get (model, &iter, 0, &device_name, -1);

  model = gtk_combo_box_get_model (GTK_COMBO_BOX(priv->resolutions_combo));
  gtk_combo_box_get_active_iter (GTK_COMBO_BOX(priv->resolutions_combo),
      &iter);
  gtk_tree_model_get (model, &iter, 1, &resolution, -1);

  gtk_list_store_clear (priv->framerates_store);

  gchar** framerates = NULL;
  framerates = dbus_video_get_framerates (device_name, resolution->width,
      resolution->height);

  if (framerates != NULL)
    {
      gchar** framerate_ptr;
      GtkTreeIter iter_framerate;
      for (framerate_ptr = framerates; *framerate_ptr; framerate_ptr++)
        {
          DEBUG("Appending text for framerate %s", *framerate_ptr);

          gtk_list_store_append (priv->framerates_store, &iter_framerate);
          gtk_list_store_set (priv->framerates_store, &iter_framerate, 0,
              *framerate_ptr, -1);
        }
    }

  gtk_combo_box_set_active (GTK_COMBO_BOX(priv->framerates_combo), 0);

}

static void
on_devices_combo_changed_cb (GtkWidget* widget, gpointer self)
{
  VideoConfPrivate* priv = GET_PRIVATE((VideoConf*) self);

  DEBUG("Callback on_devices_combo_changed_cb");

  gint index = gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
  if (index >= 0)
    {
      // Get the supported resolutions
      GtkTreeModel* model;
      GtkTreeIter iter_device_name;
      gchar* device_name;

      model = gtk_combo_box_get_model (GTK_COMBO_BOX(widget));
      gtk_combo_box_get_active_iter (GTK_COMBO_BOX(widget), &iter_device_name);
      gtk_tree_model_get (model, &iter_device_name, 0, &device_name, -1);

      GList* resolutions = NULL;
      resolutions = dbus_video_get_resolution_for_device (device_name);

      gtk_list_store_clear (priv->resolutions_store);

      GList* li;
      GtkTreeIter iter_resolutions;
      for (li = resolutions; li != NULL; li = g_list_next(li))
        {
          resolution_t* resolution = li->data;
          gchar* resolution_str = g_strdup_printf ("%d x %d",
              resolution->width, resolution->height);

          gtk_list_store_append (priv->resolutions_store, &iter_resolutions);

          gtk_list_store_set (priv->resolutions_store, &iter_resolutions, 0,
              resolution_str, 1, resolution, -1);

          g_free (resolution_str);
        }

      if (resolutions != NULL)
        {
          g_list_free (resolutions);
        }

      g_free (device_name);
    }
}

static void
video_conf_init (VideoConf *self)
{
  // Capture button
  VideoConfPrivate* priv = GET_PRIVATE(self);

  // Cairo video

  // Device list
  gchar** available_devices = NULL;
  available_devices = dbus_video_enumerate_devices ();

  priv->devices_store = gtk_list_store_new (1, G_TYPE_STRING);
  priv->devices_combo = gtk_combo_box_new_with_model (
      GTK_TREE_MODEL(priv->devices_store));

  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(priv->devices_combo), renderer,
      TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(priv->devices_combo),
      renderer, "text", 0, NULL);

  if (available_devices != NULL)
    {
      char **available_devices_ptr;
      GtkTreeIter iter;
      for (available_devices_ptr = available_devices; *available_devices_ptr; available_devices_ptr++)
        {
          DEBUG ("Device %s", *available_devices_ptr);

          gtk_list_store_append (priv->devices_store, &iter);
          gtk_list_store_set (priv->devices_store, &iter, 0,
              *available_devices_ptr, -1);
        }
    }

  gtk_combo_box_set_active (GTK_COMBO_BOX(priv->devices_combo), 0);

  // Resolutions list
  priv->resolutions_store = gtk_list_store_new (2, G_TYPE_STRING,
      G_TYPE_POINTER);
  priv->resolutions_combo = gtk_combo_box_new_with_model (
      GTK_TREE_MODEL(priv->resolutions_store));
  GtkWidget* resolutions_label = gtk_label_new ("Resolution");

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(priv->resolutions_combo),
      renderer, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(priv->resolutions_combo),
      renderer, "text", 0, NULL);

  on_devices_combo_changed_cb (priv->devices_combo, self);
  gtk_combo_box_set_active (GTK_COMBO_BOX(priv->resolutions_combo), 0);

  // Framerate list
  priv->framerates_store = gtk_list_store_new (1, G_TYPE_STRING);
  priv->framerates_combo = gtk_combo_box_new_with_model (
        GTK_TREE_MODEL(priv->framerates_store));

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(priv->framerates_combo),
      renderer, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(priv->framerates_combo),
      renderer, "text", 0, NULL);

  GtkWidget* framerates_label = gtk_label_new ("Framerate");
  on_resolutions_combo_changed_cb (priv->resolutions_combo, self);
  gtk_combo_box_set_active (GTK_COMBO_BOX(priv->framerates_combo), 0);

  // Pack everything up
  gtk_box_pack_start (GTK_BOX(self), priv->devices_combo, FALSE, FALSE, 10);

  GtkWidget* hbox = gtk_hbox_new (FALSE, 10);
  gtk_box_pack_start (GTK_BOX(hbox), resolutions_label, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(hbox), priv->resolutions_combo, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(hbox), framerates_label, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(hbox), priv->framerates_combo, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(self), hbox, FALSE, FALSE, 10);

  // Connect signals
  g_signal_connect(G_OBJECT(priv->devices_combo), "changed", G_CALLBACK(on_devices_combo_changed_cb), self);
  g_signal_connect(G_OBJECT(priv->resolutions_combo), "changed", G_CALLBACK(on_resolutions_combo_changed_cb), self);

  gtk_widget_show_all (GTK_WIDGET(self));
}

VideoConf*
video_conf_new (void)
{
  return g_object_new (SFL_TYPE_VIDEOCONF, NULL);
}

