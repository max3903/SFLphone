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
#include <string.h>
#include <stdlib.h>

#include "SFLVideoDevicePreview.h"
#include "util/video_endpoint.h"
#include "sflphone_const.h"

#define SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SFL_TYPE_VIDEO_DEVICE_PREVIEW, SFLVideoDevicePreviewPrivate))

typedef struct _SFLVideoDevicePreviewPrivate SFLVideoDevicePreviewPrivate;
struct _SFLVideoDevicePreviewPrivate {
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

static gpointer sfl_video_device_preview_parent_class = NULL;

enum {
    PROP_SOURCE = 1, PROP_WIDTH, PROP_HEIGHT, PROP_FPS, LAST_PROPERTY
} SFLVideoDevicePreviewProperties;

static const int DEFAULT_NO_DEVICE_WIDTH = 320;
static const int DEFAULT_NO_DEVICE_HEIGHT = 240;
static const int DEFAULT_BPP = 4;
static const gchar* DEFAULT_FPS = "30/1";

static void
reallocate_buffer (SFLVideoDevicePreview* self)
{
    SFLVideoDevicePreviewPrivate* priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE ( (SFLVideoDevicePreview*) self);

    DEBUG ("Reallocating buffers");

    free (priv->image_data);

    priv->image_data = malloc (priv->width * priv->height * DEFAULT_BPP);

    memset (priv->image_data, 0x000000ff, priv->width * priv->height
            * DEFAULT_BPP);

    priv->image_stride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32,
                         priv->width);

    priv->surface = cairo_image_surface_create_for_data (priv->image_data,
                    CAIRO_FORMAT_ARGB32, priv->width, priv->height, priv->image_stride);
}

static void
sfl_video_device_preview_set_property (GObject *object, guint property_id,
                                       const GValue *value, GParamSpec *pspec)
{
    SFLVideoDevicePreview *self = SFL_VIDEO_DEVICE_PREVIEW (object);
    SFLVideoDevicePreviewPrivate *priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE (self);

    DEBUG ("Setting property.");

    switch (property_id) {
        case PROP_SOURCE:
            DEBUG ("Setting source %s", g_value_get_string (value))
            ;
            priv->source = g_strdup (g_value_get_string (value));
            sflphone_video_set_device (priv->endpt, priv->source);
            break;
        case PROP_WIDTH:
            DEBUG ("Setting width %d", g_value_get_int (value))
            ;
            priv->width = g_value_get_int (value);
            reallocate_buffer (self);
            sflphone_video_set_width (priv->endpt, priv->width);
            break;
        case PROP_HEIGHT:
            DEBUG ("Setting height %d", g_value_get_int (value))
            ;
            priv->height = g_value_get_int (value);
            reallocate_buffer (self);
            sflphone_video_set_height (priv->endpt, priv->height);
            break;
        case PROP_FPS:
            DEBUG ("Setting fps %s", g_value_get_string (value))
            ;
            priv->fps = g_strdup (g_value_get_string (value));
            sflphone_video_set_framerate (priv->endpt, priv->fps);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            return;
    }
}

static void
sfl_video_device_preview_get_property (GObject *object, guint property_id, GValue *value,
                                       GParamSpec *pspec)
{
    SFLVideoDevicePreview *self = SFL_VIDEO_DEVICE_PREVIEW (object);
    SFLVideoDevicePreviewPrivate *priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE (self);

    switch (property_id) {
        case PROP_SOURCE:
            g_value_set_string (value, priv->source);
            break;
        case PROP_WIDTH:
            g_value_set_int (value, priv->width);
            break;
        case PROP_HEIGHT:
            g_value_set_int (value, priv->height);
            break;
        case PROP_FPS:
            g_value_set_string (value, priv->fps);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

void
sfl_video_device_preview_set_source (SFLVideoDevicePreview* sfl_video_device_preview, gchar* source)
{
    DEBUG ("Setting source (%s)", source);
    g_object_set (G_OBJECT (sfl_video_device_preview), "source", source, NULL);
}

void
sfl_video_device_preview_set_capture_width (SFLVideoDevicePreview* sfl_video_device_preview, gint width)
{
    DEBUG ("Setting width (%d)", width);
    g_object_set (G_OBJECT (sfl_video_device_preview), "width", width, NULL);
}

void
sfl_video_device_preview_set_capture_height (SFLVideoDevicePreview* sfl_video_device_preview, gint height)
{
    DEBUG ("Setting height (%d)", height);
    g_object_set (G_OBJECT (sfl_video_device_preview), "height", height, NULL);
}

void
sfl_video_device_preview_set_capture_framerate (SFLVideoDevicePreview* sfl_video_device_preview, gchar* fps)
{
    DEBUG ("Setting frame rate (%s)", fps);
    g_object_set (G_OBJECT (sfl_video_device_preview), "fps", fps, NULL);
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

    if (status != CAIRO_STATUS_SUCCESS) {
        ERROR ("While creating cairo surface for dumping image to png : (%s)", cairo_status_to_string (status));
        return;
    }

    gchar* filename =
        g_strconcat ("buffer-dump-", get_timestamp (), ".png", NULL);
    DEBUG ("Writing filename : %s", filename);
    cairo_surface_write_to_png (pSurface, filename);
    g_free (filename);

    cairo_surface_destroy (pSurface);
}

static void
sfl_video_device_preview_redraw_canvas (SFLVideoDevicePreview* self)
{
    GtkWidget *widget;
    GdkRegion *region;

    widget = GTK_WIDGET (self);

    if (!widget->window)
        return;

    region = gdk_drawable_get_clip_region (widget->window);

    /* redraw the cairo canvas completely by exposing it */
    gdk_window_invalidate_region (widget->window, region, TRUE);
    //gdk_window_process_updates (widget->window, TRUE);

    gdk_region_destroy (region);
}

static void
on_new_frame_cb (uint8_t* frame, void* widget)
{
    // DEBUG("Got frame");

    SFLVideoDevicePreviewPrivate* priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE ( (SFLVideoDevicePreview*) widget);

    // Copy the frame into the image surface
    memcpy (priv->image_data, frame, priv->width * priv->height * DEFAULT_BPP);

    gtk_widget_queue_draw (GTK_WIDGET (widget));
    //sfl_video_device_preview_redraw_canvas((SFLVideoDevicePreview*) widget);
}

static void
sfl_video_device_preview_init (SFLVideoDevicePreview *self)
{
    SFLVideoDevicePreviewPrivate *priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE (self);
    DEBUG ("Initializing cairo");

    priv->endpt = sflphone_video_init ();

    priv->image_data = malloc (DEFAULT_NO_DEVICE_WIDTH * DEFAULT_NO_DEVICE_HEIGHT
                               * DEFAULT_BPP);
    memset (priv->image_data, 0x000000ff, DEFAULT_NO_DEVICE_WIDTH
            * DEFAULT_NO_DEVICE_HEIGHT * DEFAULT_BPP);
    priv->image_stride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32,
                         DEFAULT_NO_DEVICE_WIDTH);

    priv->surface = cairo_image_surface_create_for_data (priv->image_data,
                    CAIRO_FORMAT_ARGB32, DEFAULT_NO_DEVICE_WIDTH, DEFAULT_NO_DEVICE_HEIGHT,
                    priv->image_stride);

    priv->capturing = FALSE;
}

static void
sfl_video_device_preview_finalize (GObject *object)
{
    SFLVideoDevicePreview *self = SFL_VIDEO_DEVICE_PREVIEW (object);
    SFLVideoDevicePreviewPrivate *priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE (self);

    free (priv->image_data);
    cairo_surface_destroy (priv->surface);
    g_free (priv->source);

    G_OBJECT_CLASS (sfl_video_device_preview_parent_class)->finalize (object);
}

static gboolean
sfl_video_device_preview_expose (GtkWidget* cairo_video, GdkEventExpose* event)
{
    SFLVideoDevicePreviewPrivate* priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE (cairo_video);

    // Redraw on every expose event.
    cairo_t* cairo_context = gdk_cairo_create (cairo_video->window);

    // DEBUG("Expose event for video cairo widget invalidate %d %d", event->area.x, event->area.y);

    cairo_set_source_surface (cairo_context, priv->surface, event->area.x,
                              event->area.y);
    cairo_paint (cairo_context);
    cairo_destroy (cairo_context);

    return FALSE;
}

static void
sfl_video_device_preview_class_init (SFLVideoDevicePreviewClass *class)
{
    GObjectClass *obj_class;
    GtkWidgetClass *widget_class;

    obj_class = G_OBJECT_CLASS (class);
    obj_class->get_property = sfl_video_device_preview_get_property;
    obj_class->set_property = sfl_video_device_preview_set_property;
    obj_class->finalize = sfl_video_device_preview_finalize;

    widget_class = GTK_WIDGET_CLASS (class);
    widget_class->expose_event = sfl_video_device_preview_expose;

    g_object_class_install_property (obj_class, PROP_SOURCE, g_param_spec_string (
                                         "source", "source", "String specifying the source SHM for the video",
                                         NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
                                         | G_PARAM_READWRITE));

    g_object_class_install_property (obj_class, PROP_WIDTH, g_param_spec_int (
                                         "width", "width", "The frame width", G_MININT, G_MAXINT, 0,
                                         G_PARAM_READWRITE));

    g_object_class_install_property (obj_class, PROP_HEIGHT, g_param_spec_int (
                                         "height", "height", "The frame height", G_MININT, G_MAXINT, 0,
                                         G_PARAM_READWRITE));

    g_object_class_install_property (obj_class, PROP_FPS, g_param_spec_string (
                                         "fps", "fps",
                                         "String specifying the frame rate of the form numerator/denominator",
                                         NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
                                         | G_PARAM_READWRITE));

    g_type_class_add_private (obj_class, sizeof (SFLVideoDevicePreviewPrivate));
}

static void
sfl_video_device_preview_class_intern_init (gpointer klass)
{
    sfl_video_device_preview_parent_class = g_type_class_peek_parent (klass);
    sfl_video_device_preview_class_init ( (SFLVideoDevicePreviewClass*) klass);
}

GType
sfl_video_device_preview_get_type (void)
{
    static GType g_define_type_id = 0;

    if (g_define_type_id == 0) {
        static const GTypeInfo g_define_type_info = { sizeof (SFLVideoDevicePreviewClass), (GBaseInitFunc) NULL,
                (GBaseFinalizeFunc) NULL,
                (GClassInitFunc) sfl_video_device_preview_class_intern_init,
                (GClassFinalizeFunc) NULL, NULL, /* class_data */
                sizeof (SFLVideoDevicePreview), 0, /* n_preallocs */
                (GInstanceInitFunc) sfl_video_device_preview_init, NULL,
                                                    };
        g_define_type_id = g_type_register_static (GTK_TYPE_DRAWING_AREA,
                           "SFLVideoDevicePreview", &g_define_type_info, 0);
    }

    return g_define_type_id;
}

SFLVideoDevicePreview *
sfl_video_device_preview_new_with_source (const gchar *source)
{
    DEBUG ("Creating new SFLVideoDevicePreview.");
    return g_object_new (SFL_TYPE_VIDEO_DEVICE_PREVIEW, "source", source, "width",
                         DEFAULT_NO_DEVICE_WIDTH, "height", DEFAULT_NO_DEVICE_HEIGHT, "fps",
                         DEFAULT_FPS, NULL);
}

SFLVideoDevicePreview*
sfl_video_device_preview_new ()
{
    return sfl_video_device_preview_new_with_source ("");
}

int
sfl_video_device_preview_start (SFLVideoDevicePreview* self)
{
    DEBUG ("Starting video cairo capture");

    SFLVideoDevicePreviewPrivate* priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE (self);

    if (sflphone_video_add_observer (priv->endpt, &on_new_frame_cb, self) < 0) {
        ERROR ("Failed to register as an observer and start video %s:%d", __FILE__, __LINE__);
        return -1;
    }

    if (sflphone_video_open_device (priv->endpt) < 0) {
        ERROR ("Failed to open and start video %s:%d", __FILE__, __LINE__);
        return -1;
    }

    if (sflphone_video_start_async (priv->endpt) < 0) {
        ERROR ("Failed to start video %s:%d", __FILE__, __LINE__);
        return -1;
    }

    DEBUG ("Registered as an observer");

    priv->capturing = TRUE;
}

int
sfl_video_device_preview_stop (SFLVideoDevicePreview* self)
{
    DEBUG ("Stopping video cairo capture");

    SFLVideoDevicePreviewPrivate* priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE (self);

    sflphone_video_remove_observer (priv->endpt, &on_new_frame_cb);

    sflphone_video_stop_async (priv->endpt);
    sflphone_video_close_device (priv->endpt);

    priv->capturing = FALSE;
}

gboolean
sfl_video_device_preview_is_capturing (SFLVideoDevicePreview* sfl_video_device_preview)
{
    SFLVideoDevicePreviewPrivate* priv = SFL_VIDEO_DEVICE_PREVIEW_GET_PRIVATE (sfl_video_device_preview);
    return priv->capturing;
}
