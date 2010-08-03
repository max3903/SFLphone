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
#include "VideoConfAccount.h"
#include "VideoConfDevice.h"
#include "VideoCodecList.h"

#include "widget/SFLMessageBar.h"

#include "sflphone_const.h"

static const int DEFAULT_SPACING = 10;
G_DEFINE_TYPE (VideoConfAccount, video_conf_account, GTK_TYPE_VBOX)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_VIDEO_CONF_ACCOUNT, VideoConfAccountPrivate))

enum
{
  PROP_ACCOUNT = 1, LAST_PROPERTY
} VideoConfAccountProperties;

typedef struct _VideoConfAccountPrivate VideoConfAccountPrivate;

struct _VideoConfAccountPrivate
{
  // Info bar
  SFLMessageBar* message_bar;

  // Codecs
  GtkWidget* codecs_section;

  VideoCodecList* video_codec_list;

  // Devices
  GtkWidget* devices_section;

  VideoConfDevice* video_conf_device;

  // Calls
  GtkWidget* calls_section;

  GtkWidget* always_offer_vdo_checkbox;

  // Data
  account_t* account;
};

static void
video_conf_account_set_property (GObject *object, guint property_id,
    const GValue *value, GParamSpec *pspec)
{
  VideoConfAccount* self = SFL_VIDEO_CONF_ACCOUNT(object);
  VideoConfAccountPrivate* priv = GET_PRIVATE((VideoConfAccount*) self);

  switch (property_id)
    {
  case PROP_ACCOUNT:
    priv->account = g_value_get_pointer (value);
    ;
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
video_conf_account_get_property (GObject *object, guint property_id,
    GValue *value, GParamSpec *pspec)
{
  VideoConfAccount *self = SFL_VIDEO_CONF_ACCOUNT(object);
  VideoConfAccountPrivate* priv = GET_PRIVATE((VideoConfAccount*) self);

  switch (property_id)
    {
  case PROP_ACCOUNT:
    g_value_set_pointer (value, priv->account);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

void
video_conf_account_set_account (VideoConfAccount* video_conf_account,
    account_t* account)
{
  g_object_set (G_OBJECT(video_conf_account), "account", account, NULL);
}

static void
video_conf_account_dispose (GObject *object)
{
  G_OBJECT_CLASS (video_conf_account_parent_class)->dispose (object);
}

static void
video_conf_account_finalize (GObject* object)
{
  G_OBJECT_CLASS (video_conf_account_parent_class)->finalize (object);
}

static void
video_conf_account_realize (GtkWidget* self)
{
  DEBUG("Realizing ....");
  GTK_WIDGET_CLASS(video_conf_account_parent_class)->realize (self);

  VideoConfAccountPrivate* priv = GET_PRIVATE(self);

  if (priv->account)
    {
      priv->video_codec_list = video_codec_list_new (priv->account);
      gtk_container_add (GTK_CONTAINER (priv->codecs_section),
          priv->video_codec_list);
      gtk_widget_show_all (GTK_WIDGET(self));
    }
}

static void
video_conf_account_class_init (VideoConfAccountClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = video_conf_account_dispose;
  object_class->get_property = video_conf_account_get_property;
  object_class->set_property = video_conf_account_set_property;
  object_class->finalize = video_conf_account_finalize;

  GtkWidgetClass* widget_class;
  widget_class = GTK_WIDGET_CLASS (klass);
  widget_class->realize = video_conf_account_realize;

  // Install properties
  g_object_class_install_property (object_class, PROP_ACCOUNT,
      g_param_spec_pointer ("account", "account",
          "The account on which to configure settings for.",
          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  // Install private data
  g_type_class_add_private (klass, sizeof(VideoConfAccountPrivate));
}

static void
video_conf_account_init (VideoConfAccount* self)
{
  VideoConfAccountPrivate* priv = GET_PRIVATE(self);

  // Message bar
  priv->message_bar = sfl_message_bar_new();
  gtk_widget_set_no_show_all (GTK_INFO_BAR(priv->message_bar), TRUE);

  // Codec List is created on realization as the account property is not yet available.
  gnome_main_section_new (_("Codecs"), &priv->codecs_section);

  priv->always_offer_vdo_checkbox = gtk_check_button_new_with_label (
      _("Always offer video when establishing phone call"));

  // Video devices
  gnome_main_section_new (_("Devices"), &priv->devices_section);

  priv->video_conf_device = video_conf_device_new();
  gtk_widget_show(GTK_WIDGET(priv->video_conf_device));
  gtk_container_add (GTK_CONTAINER (priv->devices_section), priv->video_conf_device);

  // Calls
  gnome_main_section_new (_("Calls"), &priv->calls_section);

  gtk_container_add (GTK_CONTAINER (priv->calls_section),
      priv->always_offer_vdo_checkbox);

  // Pack everything up
  gtk_box_pack_start (GTK_BOX(self), priv->message_bar, TRUE, TRUE,
      DEFAULT_SPACING);

  gtk_box_pack_start (GTK_BOX(self), priv->codecs_section, TRUE, TRUE,
      DEFAULT_SPACING);

  gtk_box_pack_start (GTK_BOX(self), priv->calls_section, TRUE, TRUE,
      DEFAULT_SPACING);

  gtk_box_pack_start (GTK_BOX(self), priv->devices_section, TRUE, FALSE,
      DEFAULT_SPACING);

  gtk_widget_show_all (GTK_WIDGET(self));
}

VideoConfAccount*
video_conf_account_new (account_t* account)
{
  return g_object_new (SFL_TYPE_VIDEO_CONF_ACCOUNT, "account", account, NULL);
}

void
video_conf_account_save (VideoConfAccount* self)
{
  VideoConfAccountPrivate* priv = GET_PRIVATE(self);

  codec_list_save (SFL_CODEC_LIST(priv->video_codec_list));
}
