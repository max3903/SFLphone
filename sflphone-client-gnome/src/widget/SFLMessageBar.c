/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoifairelinux.com>
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

#include "SFLMessageBar.h"

static const int DEFAULT_SPACING = 5;
G_DEFINE_TYPE (SFLMessageBar, sfl_message_bar, GTK_TYPE_INFO_BAR)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_MESSAGE_BAR, SFLMessageBarPrivate))

typedef struct _SFLMessageBarPrivate SFLMessageBarPrivate;

struct _SFLMessageBarPrivate
{
  GtkWidget* info_bar_title;

  GtkWidget* info_bar_message;
};

static void
show_message (SFLMessageBar* self, gchar* title, gchar* message,
    GtkMessageType type)
{
  SFLMessageBarPrivate* priv = GET_PRIVATE((SFLMessageBar*) self);

  char* markup = g_markup_printf_escaped ("<b>%s</b>", title);
  gtk_label_set_markup (GTK_LABEL (priv->info_bar_title), markup);
  g_free (markup);

  markup = g_markup_printf_escaped ("<i>%s</i>", message);
  gtk_label_set_markup (GTK_LABEL (priv->info_bar_message), markup);
  g_free (markup);

  gtk_info_bar_set_message_type (GTK_INFO_BAR (self), type);
}

void
sfl_message_bar_show_info (SFLMessageBar *self, gchar* title, gchar* message)
{
  show_message (self, title, message, GTK_MESSAGE_INFO);
}

void
sfl_message_bar_show_question (SFLMessageBar *self, gchar* title, gchar* message)
{
  show_message (self, title, message, GTK_MESSAGE_QUESTION);
}

void
sfl_message_bar_show_warning (SFLMessageBar *self, gchar* title, gchar* message)
{
  show_message (self, title, message, GTK_MESSAGE_WARNING);
}

void
sfl_message_bar_show_error (SFLMessageBar *self, gchar* title, gchar* message)
{
  show_message (self, title, message, GTK_MESSAGE_ERROR);
}

void
sfl_message_bar_show_other (SFLMessageBar *self, gchar* title, gchar* message)
{
  show_message (self, title, message, GTK_MESSAGE_OTHER);
}

static void
sfl_message_bar_init (SFLMessageBar *self)
{
  GtkWidget* vbox = gtk_vbox_new (FALSE, 0);
  SFLMessageBarPrivate* priv = GET_PRIVATE((SFLMessageBar*) self);
  priv->info_bar_title = gtk_label_new ("");
  gtk_label_set_line_wrap(GTK_LABEL(priv->info_bar_title), TRUE);
  priv->info_bar_message = gtk_label_new ("");
  gtk_label_set_line_wrap(GTK_LABEL(priv->info_bar_message), TRUE);

  gtk_box_pack_start (GTK_BOX(vbox), priv->info_bar_title, TRUE, TRUE, DEFAULT_SPACING);
  gtk_box_pack_start (GTK_BOX(vbox), priv->info_bar_message, TRUE, TRUE, DEFAULT_SPACING);
  gtk_widget_show_all (GTK_WIDGET(vbox));

  //gtk_info_bar_add_button (GTK_INFO_BAR(self), GTK_STOCK_OK, GTK_RESPONSE_OK);
  //g_signal_connect (self, "response", G_CALLBACK (gtk_widget_hide), NULL);

  GtkWidget* content_area = gtk_info_bar_get_content_area (GTK_INFO_BAR (self));
  gtk_container_add (GTK_CONTAINER (content_area), vbox);
}

static void
sfl_message_bar_class_init (SFLMessageBarClass *klass)
{
  GtkInfoBarClass* base_class = GTK_INFO_BAR_CLASS (klass);

  g_type_class_add_private (klass, sizeof(SFLMessageBarPrivate));
}

SFLMessageBar*
sfl_message_bar_new ()
{
  return g_object_new (SFL_TYPE_MESSAGE_BAR, NULL);
}

