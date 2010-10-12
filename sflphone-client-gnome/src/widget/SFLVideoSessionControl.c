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
#include "SFLVideoSessionControl.h"
#include "sflphone_const.h"

G_DEFINE_TYPE (SFLVideoSessionControl, sfl_video_session_control, GTK_TYPE_TOOLBAR)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_VIDEO_SESSION_CONTROL, SFLVideoSessionControlPrivate))

typedef struct _SFLVideoSessionControlPrivate SFLVideoSessionControlPrivate;

struct _SFLVideoSessionControlPrivate {
    GtkToolItem* screenshot;
    GtkToolItem* pause;
    GtkToolItem* fullscreen;
};

/**
 * Signals
 */
enum {
    SNAPSHOT_CLICKED, LAST_SIGNAL
};

static guint sfl_video_session_control_signals[LAST_SIGNAL] = { 0 };

static void
sfl_video_session_control_dispose (GObject *object)
{
    SFLVideoSessionControlPrivate* priv =
        GET_PRIVATE ( (SFLVideoSessionControl*) object);

    G_OBJECT_CLASS (sfl_video_session_control_parent_class)->dispose (object);
}

static void
sfl_video_session_control_finalize (GObject *object)
{
    G_OBJECT_CLASS (sfl_video_session_control_parent_class)->finalize (object);
}

static void
sfl_video_session_control_class_init (SFLVideoSessionControlClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (SFLVideoSessionControlPrivate));

    object_class->dispose = sfl_video_session_control_dispose;
    object_class->finalize = sfl_video_session_control_finalize;

    // Install signals
    sfl_video_session_control_signals[SNAPSHOT_CLICKED] = g_signal_new ("snapshot-clicked",
            G_TYPE_FROM_CLASS (klass), (GSignalFlags) (G_SIGNAL_RUN_FIRST
                    | G_SIGNAL_ACTION), 0, NULL, NULL, g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE, 0);
}

static void
fullscreen_toggled_cb (GtkToggleToolButton *toolbutton, gpointer user_data)
{
    DEBUG ("fullscreen toggled");

    GtkWindow* window = gtk_widget_get_toplevel (GTK_WIDGET (toolbutton));

    if (gtk_toggle_tool_button_get_active (toolbutton) == TRUE) {
        gtk_window_fullscreen (window);
        gtk_tool_button_set_stock_id (GTK_TOOL_BUTTON (toolbutton), GTK_STOCK_LEAVE_FULLSCREEN);
    } else {
        gtk_window_unfullscreen (window);
        gtk_tool_button_set_stock_id (GTK_TOOL_BUTTON (toolbutton), GTK_STOCK_FULLSCREEN);
    }
}

static void
screenshot_clicked_cb (GtkToolButton* toolbutton, gpointer user_data)
{
    SFLVideoSessionControl* self = (SFLVideoSessionControl*) user_data;

    g_signal_emit (self, sfl_video_session_control_signals[SNAPSHOT_CLICKED], 0);
}

static void
sfl_video_session_control_init (SFLVideoSessionControl* self)
{
    SFLVideoSessionControlPrivate* priv = GET_PRIVATE (self);

    //priv->pause = gtk_toggle_tool_button_new_from_stock (GTK_STOCK_MEDIA_PAUSE);
    priv->fullscreen = gtk_toggle_tool_button_new_from_stock (GTK_STOCK_FULLSCREEN);
    priv->screenshot =  gtk_tool_button_new (NULL, "snapshot");

    gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (priv->screenshot), "applets-screenshooter");

    gtk_toolbar_insert (GTK_TOOLBAR (self), GTK_TOOL_ITEM (priv->screenshot), -1);
    //gtk_toolbar_insert (GTK_TOOLBAR(self), GTK_TOOL_ITEM(priv->pause), -1);
    gtk_toolbar_insert (GTK_TOOLBAR (self), GTK_TOOL_ITEM (priv->fullscreen), -1);

    // Install callback for the fullscreen button
    g_signal_connect (G_OBJECT (priv->fullscreen), "toggled",
                      G_CALLBACK (fullscreen_toggled_cb), NULL);

    // Install callback for the snapshot button
    g_signal_connect (G_OBJECT (priv->screenshot), "clicked",
                      G_CALLBACK (screenshot_clicked_cb), self);

    gtk_toolbar_set_style (GTK_TOOLBAR (self), GTK_TOOLBAR_ICONS);
}

SFLVideoSessionControl*
sfl_video_session_control_new (void)
{
    return g_object_new (SFL_TYPE_VIDEO_SESSION_CONTROL, NULL);
}
