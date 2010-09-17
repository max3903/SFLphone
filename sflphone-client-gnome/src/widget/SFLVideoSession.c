/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
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

  gchar* shm;
};

enum
{
  PROP_SHM_PATH = 1, LAST_PROPERTY
} SFLVideoSessionProperties;

static void
playing_cb(SFLVideoCairoShm* widget, gpointer* self) {
  DEBUG("\"playing\" signal received.");

  SFLVideoSessionPrivate* priv = GET_PRIVATE(self);

  gint index = gtk_notebook_page_num(GTK_NOTEBOOK(priv->notebook), GTK_WIDGET(widget));
  if (index < 0) {
    ERROR("Could not find the remote video widget in the notebook.");
    return;
  }

  gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->notebook), index);
}

static void
append_video_shm_page(SFLVideoSession* self)
{
  SFLVideoSessionPrivate* priv = GET_PRIVATE(self);

  // Video page
  priv->remote_video = sfl_video_cairo_shm_new (priv->shm);

  // Connect the "playing" signal so that we can switch
  // to the appropriate page automatically
  g_signal_connect(G_OBJECT(priv->remote_video), "playing", G_CALLBACK(playing_cb), self);
  DEBUG("playing handler connected");

  // Append the page to the notebook
  gtk_widget_show (GTK_WIDGET(priv->remote_video));
  gtk_notebook_append_page (GTK_NOTEBOOK (priv->notebook),
      GTK_WIDGET(priv->remote_video), gtk_label_new ("video"));

  // Start displaying video
  if (sfl_video_cairo_shm_start(priv->remote_video) < 0) {
    ERROR("Failed to start video from SFLVideoSession widget");
  }
}

static void
sfl_video_session_set_property (GObject *object, guint property_id,
    const GValue *value, GParamSpec *pspec)
{
  SFLVideoSession* self = SFL_VIDEO_SESSION(object);
  SFLVideoSessionPrivate* priv = GET_PRIVATE(self);

  switch (property_id)
    {
  case PROP_SHM_PATH:
    priv->shm = g_value_dup_string (value);
    append_video_shm_page(self);
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
  case PROP_SHM_PATH:
    g_value_set_string (value, priv->shm);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
sfl_video_session_finalize (GObject *object)
{
  SFLVideoSession* self = SFL_VIDEO_SESSION(object);
  SFLVideoSessionPrivate* priv = GET_PRIVATE(self);

  g_free(priv->shm);

  DEBUG("SFLVideoSession widget finalized.");
}

static void
sfl_video_session_class_init (SFLVideoSessionClass *class)
{
  GObjectClass* obj_class = G_OBJECT_CLASS (class);
  obj_class->get_property = sfl_video_session_get_property;
  obj_class->set_property = sfl_video_session_set_property;
  obj_class->finalize = sfl_video_session_finalize;

  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (class);

  g_object_class_install_property (obj_class, PROP_SHM_PATH,
      g_param_spec_string ("shm", "shm",
          "The shared memory segment to read data from.", NULL,
          G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
              | G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_type_class_add_private (obj_class, sizeof(SFLVideoSessionPrivate));
}

/**
 * Taken from cheese-widget.c
 */
static void
sfl_video_session_widget_spinner_invert (GtkWidget *spinner, GtkWidget *parent)
{
  GtkStyle *style;
  guint i;

  for (i = GTK_STATE_NORMAL; i <= GTK_STATE_INSENSITIVE; i++)
    {
      GdkColor *fg, *bg;

      style = gtk_widget_get_style (spinner);
      fg = gdk_color_copy (&style->fg[i]);
      bg = gdk_color_copy (&style->bg[i]);

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
  gtk_widget_set_size_request(priv->spinner, 320, 240);
  GtkWidget* box = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (box), priv->spinner);
  sfl_video_session_widget_spinner_invert (priv->spinner, box);
  gtk_widget_show_all (box);

  gtk_notebook_append_page (GTK_NOTEBOOK (priv->notebook), box, gtk_label_new (
      "spinner"));

  // Problem page
  //  priv->problem = gtk_drawing_area_new ();
  //  gtk_widget_show (priv->problem);
  //  gtk_notebook_append_page (GTK_NOTEBOOK (widget),
  //                            priv->problem,
  //                            gtk_label_new ("got problems"));

  // Align the notebook
  GtkWidget* align_notebook = gtk_alignment_new (0.5, 0.0, 1.0, 1.0);
  gtk_container_add (GTK_CONTAINER(align_notebook), GTK_WIDGET(priv->notebook));
  gtk_box_pack_start (GTK_BOX(self), align_notebook, FALSE, TRUE, 0);

  // Create the toolbar
  priv->controls = sfl_video_session_control_new ();
  GtkWidget* align_toolbar = gtk_alignment_new (0.5, 0.0, 0.5, 0.0);
  gtk_container_add (GTK_CONTAINER(align_toolbar), GTK_WIDGET(priv->controls));
  gtk_box_pack_start (GTK_BOX(self), align_toolbar, FALSE, TRUE, 0);

  // Start the spinner
  gtk_spinner_start (GTK_SPINNER(priv->spinner));

  gtk_widget_show_all (GTK_WIDGET(priv->notebook));
}

SFLVideoSession*
sfl_video_session_new (const gchar* shm)
{
  return g_object_new (SFL_TYPE_VIDEO_SESSION, "shm", shm, NULL);
}
