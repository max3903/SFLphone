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
#include "VideoConfDevice.h"
#include "sflphone_const.h"

G_DEFINE_TYPE (VideoConfDevice, video_conf_device, GTK_TYPE_VBOX)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_VIDEO_CONF_DEVICE, VideoConfDevicePrivate))

typedef struct _VideoConfDevicePrivate VideoConfDevicePrivate;

struct _VideoConfDevicePrivate
{
  GtkWidget* devices_combo;
  GtkListStore* devices_store;

  GtkWidget* expander;

  GtkListStore* resolutions_store;
  GtkWidget* resolutions_combo;

  GtkListStore* framerates_store;
  GtkWidget* framerates_combo;
};

enum
{
  COLUMN_NAME, COLUMN_IDENTIFIER, COLUMN_N
};

enum
{
  COLUMN_RESOLUTION_STRING, COLUMN_RESOLUTION_POINTER, COLUMN_RESOLUTION_N
} ResolutionColumns;

enum
{
  COLUMN_FRAMERATE_STRING, COLUMN_FRAMERATE_POINTER, COLUMN_FRAMERATE_N
} FramerateColumns;

static void
video_conf_device_dispose (GObject *object)
{
  VideoConfDevicePrivate* priv = GET_PRIVATE((VideoConfDevice*) object);

  G_OBJECT_CLASS (video_conf_device_parent_class)->dispose (object);
}

static void
video_conf_device_finalize (GObject *object)
{
  G_OBJECT_CLASS (video_conf_device_parent_class)->finalize (object);
}

static void
video_conf_device_class_init (VideoConfDeviceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof(VideoConfDevicePrivate));

  object_class->dispose = video_conf_device_dispose;
  object_class->finalize = video_conf_device_finalize;
}

static void
on_framerates_combo_changed_cb (GtkWidget* widget, gpointer self)
{
  /* Nothing */
}

static void
on_resolutions_combo_changed_cb (GtkWidget* widget, gpointer self)
{
  VideoConfDevicePrivate* priv = GET_PRIVATE((VideoConfDevice*) self);

  // Get the active name in the device combo box
  GtkTreeIter iter;
  if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX(priv->devices_combo), &iter)
      != TRUE)
    {
      return;
    }
  gchar* device_name;
  gtk_tree_model_get (GTK_TREE_MODEL(priv->devices_store), &iter, COLUMN_NAME, &device_name, -1);

  // Get the active resolution the the resolutions combo box
  gtk_combo_box_get_active_iter (GTK_COMBO_BOX(priv->resolutions_combo), &iter);
  resolution_t* resolution;
  gtk_tree_model_get (GTK_TREE_MODEL(priv->resolutions_store), &iter, COLUMN_RESOLUTION_POINTER, &resolution, -1);

  // Clear the framerates
  gtk_list_store_clear (priv->framerates_store);

  // Get the corresponding framerates over dbus
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
  VideoConfDevicePrivate* priv = GET_PRIVATE((VideoConfDevice*) self);

  DEBUG("Callback on_devices_combo_changed_cb");

  gint index = gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
  if (index >= 0)
    {
      // Get the supported resolutions
      GtkTreeIter iter_device_name;
      gtk_combo_box_get_active_iter (GTK_COMBO_BOX(widget), &iter_device_name);
      gchar* device_name;
      gtk_tree_model_get (GTK_TREE_MODEL(priv->devices_store), &iter_device_name, COLUMN_NAME, &device_name, -1);

      // Get the corresponding resolutions
      GList* resolutions = NULL;
      resolutions = dbus_video_get_resolution_for_device (device_name);

      // Clear the resolutions
      gtk_list_store_clear (priv->resolutions_store);

      // Fill the resolutions comobo
      GList* li;
      GtkTreeIter iter_resolutions;
      for (li = resolutions; li != NULL; li = g_list_next(li))
        {
          gtk_list_store_append (priv->resolutions_store, &iter_resolutions);

          // Format as a string
          resolution_t* resolution = li->data;
          gchar* resolution_str = g_strdup_printf ("%d x %d",
              resolution->width, resolution->height);

          // Store both string and pointer
          gtk_list_store_set (priv->resolutions_store, &iter_resolutions, COLUMN_RESOLUTION_STRING,
              resolution_str, COLUMN_RESOLUTION_POINTER, resolution, -1);

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
video_conf_device_init (VideoConfDevice* self)
{
  VideoConfDevicePrivate* priv = GET_PRIVATE(self);

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

  // Create an expander widget in which the resolution and framerate list will be contained
  priv->expander = gtk_expander_new ("");
  gtk_expander_set_use_markup (GTK_EXPANDER(priv->expander), TRUE);
  gchar* markup = g_markup_printf_escaped ("<b>%s</b>", _("Advanced options"));
  gtk_expander_set_label (GTK_EXPANDER(priv->expander), markup);
  g_free (markup);

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
  priv->framerates_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
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

  gtk_container_add (GTK_CONTAINER(priv->expander), hbox);

  gtk_box_pack_start (GTK_BOX(self), priv->expander, FALSE, FALSE, 10);

  // Connect signals
  g_signal_connect(G_OBJECT(priv->devices_combo), "changed", G_CALLBACK(on_devices_combo_changed_cb), self);
  g_signal_connect(G_OBJECT(priv->resolutions_combo), "changed", G_CALLBACK(on_resolutions_combo_changed_cb), self);
  g_signal_connect(G_OBJECT(priv->framerates_combo), "changed", G_CALLBACK(on_framerates_combo_changed_cb), self);

  gtk_widget_show_all (GTK_WIDGET(self));

  // Show errors if needed
  if (!(*available_devices))
    {
      gtk_widget_hide (priv->devices_combo);
      gtk_widget_hide (priv->resolutions_combo);
      gtk_widget_hide (resolutions_label);
      gtk_widget_hide (priv->framerates_combo);
      gtk_widget_hide (framerates_label);
      return;
    }
}

static gboolean
devices_combo_find_by_name (GtkTreeModel* model, GtkTreeIter* user_iter,  const gchar* name)
{
  gboolean valid = gtk_tree_model_get_iter_first (model, user_iter);
  while (valid)
    {
      gchar* column_name;
      gtk_tree_model_get (model, user_iter, COLUMN_NAME, &column_name, -1);

      if (g_strcmp0 (name, column_name) == 0)
        {
          return TRUE;
        }

      valid = gtk_tree_model_iter_next (model, user_iter);
    }

  return FALSE;
}

static gboolean
resolution_combo_box_find(GtkTreeModel* model, GtkTreeIter* user_iter, resolution_t resolution)
{
  gboolean valid = gtk_tree_model_get_iter_first (model, user_iter);
  while (valid)
    {
      resolution_t* res;
      gtk_tree_model_get (model, user_iter, COLUMN_RESOLUTION_POINTER, &res, -1);

      if ((res->width == resolution.width) && (res->height == resolution.height)) {
        return TRUE;
      }

      valid = gtk_tree_model_iter_next (model, user_iter);
    }

  return FALSE;
}

static gboolean
framerate_combo_box_find(GtkTreeModel* model, GtkTreeIter* user_iter, gchar* user_rate)
{
  gboolean valid = gtk_tree_model_get_iter_first (model, user_iter);
  while (valid)
    {
      gchar* framerate;
      gtk_tree_model_get (model, user_iter, COLUMN_FRAMERATE_STRING, &framerate, -1);

      if (g_strcmp0 (framerate, user_rate) == 0)
        {
          return TRUE;
        }

      valid = gtk_tree_model_iter_next (model, user_iter);
    }

  return FALSE;
}

VideoConfDevice*
video_conf_device_new (void)
{
  return g_object_new (SFL_TYPE_VIDEO_CONF_DEVICE, NULL);
}

VideoConfDevice*
video_conf_device_new_from_account (account_t* account)
{
  VideoConfDevice* device = video_conf_device_new();
  video_conf_device_set_from_account(device, account);
  return device;
}
void
video_conf_device_set_value (VideoConfDevice* device, const gchar* name,
    resolution_t resolution, framerate_t framerate)
{
  VideoConfDevicePrivate* priv = GET_PRIVATE(device);

  // Device combo
  GtkTreeIter iter;
  if (devices_combo_find_by_name (GTK_TREE_MODEL(priv->devices_store), &iter, name))
    {
      gtk_combo_box_set_active_iter (GTK_COMBO_BOX(priv->devices_combo), &iter);
    }

  // Resolution combo
  if (resolution_combo_box_find(GTK_TREE_MODEL(priv->resolutions_store), &iter, resolution)) {
    gtk_combo_box_set_active_iter (GTK_COMBO_BOX(priv->resolutions_combo), &iter);
  }

  // Framerate combo
  gchar* rate_str = framerate_to_string(&framerate);
  if (framerate_combo_box_find(GTK_TREE_MODEL(priv->framerates_store), &iter, rate_str)) {
    gtk_combo_box_set_active_iter (GTK_COMBO_BOX(priv->framerates_combo), &iter);
  }

  g_free(rate_str);
}

void
video_conf_device_set_from_account(VideoConfDevice* device, account_t* account)
{
  video_conf_device_set_value(device, account->video_settings->device,  account->video_settings->resolution,  account->video_settings->framerate);
}

void video_conf_device_save(VideoConfDevice* self, account_t* account) {
  VideoConfDevicePrivate* priv = GET_PRIVATE((VideoConfDevice*) self);

  // Get the device name
  GtkTreeModel* model = gtk_combo_box_get_model (GTK_COMBO_BOX(priv->devices_combo));

  GtkTreeIter iter;
  gtk_combo_box_get_active_iter (GTK_COMBO_BOX(priv->devices_combo), &iter);
  gchar* device_name;
  gtk_tree_model_get (model, &iter, COLUMN_NAME, &device_name, -1);

  // Get the corresponding resolutions
  resolution_t* resolution;
  model = gtk_combo_box_get_model (GTK_COMBO_BOX(priv->resolutions_combo));
  gtk_combo_box_get_active_iter (GTK_COMBO_BOX(priv->resolutions_combo), &iter);
  gtk_tree_model_get (model, &iter, COLUMN_RESOLUTION_POINTER, &resolution, -1);
  gint width = resolution->width;
  gint height = resolution->height;

  // Get the corresponding rate
  gchar* rate;
  model = gtk_combo_box_get_model (GTK_COMBO_BOX(priv->framerates_combo));
  gtk_combo_box_get_active_iter (GTK_COMBO_BOX(priv->framerates_combo), &iter);
  gtk_tree_model_get (model, &iter, 0, &rate, -1);
  framerate_t* framerate = framerate_new_from_string(rate);

  DEBUG("Saving device %s", device_name);
  video_settings_set_device(account->video_settings, device_name);
  DEBUG("Saving resolution %d x %d", resolution->width, resolution->height);
  video_settings_set_resolution(account->video_settings, resolution);
  DEBUG("Saving framerate %d/%d", framerate->numerator, framerate->denominator);
  video_settings_set_framerate(account->video_settings, framerate);

  video_settings_saves(account->video_settings, account->accountID);
}
