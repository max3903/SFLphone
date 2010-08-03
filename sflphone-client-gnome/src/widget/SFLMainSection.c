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

#include "SFLMainSection.h"

static const float ALIGN_CENTER = 0.5;
static const int SCALE = 1;
static const int DEFAULT_SPACING = 10;
G_DEFINE_TYPE (SFLMainSection, sfl_main_section, GTK_TYPE_FRAME)

static void
sfl_main_section_init (SFLMainSection *self)
{
  gtk_frame_set_shadow_type(GTK_FRAME(self), GTK_SHADOW_NONE);
}

static void
sfl_main_section_class_init (SFLMainSectionClass *klass)
{
  /* Nothing */
}

SFLMainSection*
sfl_main_section_new_with_label (const gchar* title)
{
  SFLMainSection* section = g_object_new (SFL_TYPE_MAIN_SECTION, NULL);

  // Set the label
  gtk_frame_set_label (GTK_FRAME(section), title);

  // Display the title in bold characters
  GtkWidget* label = gtk_frame_get_label_widget (GTK_FRAME(section));

  PangoAttrList* attribute_list = pango_attr_list_new ();
  PangoAttribute* attribute = pango_attr_weight_new (PANGO_WEIGHT_BOLD);

  attribute->start_index = 0;
  attribute->end_index = -1;

  pango_attr_list_insert (attribute_list, attribute);

  gtk_label_set_attributes (GTK_LABEL(label), attribute_list);
  pango_attr_list_unref (attribute_list);

  return section;
}

void sfl_main_section_set_content(SFLMainSection* section, GtkWidget* widget)
{
  GtkWidget* vbox = gtk_vbox_new(TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), DEFAULT_SPACING);
  gtk_box_pack_start(GTK_BOX(vbox), widget, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(section), vbox);
}


