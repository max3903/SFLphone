/*
 *  Copyright (C) 2008 2009 Savoir-Faire Linux inc.
 *
 *  Author: Antoine Reversat <antoine.reversat@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Julien Bonjean <julien.bonjean@savoirfairelinux.com>
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
 */

#include <searchbar.h>
#include <calltree.h>

void searchbar_entry_changed (GtkEntry* entry, gchar* arg1 UNUSED, gpointer data UNUSED) {

    if (active_calltree == contacts) {
      addressbook_search(entry);
    }
    else if (active_calltree == history) {
      history_search(entry);
    }

}

void searchbar_clear_entry_if_default (GtkWidget* widget, gpointer user_data UNUSED) {

    if(g_ascii_strncasecmp(gtk_entry_get_text(GTK_ENTRY(widget)), _("Search"), 6) == 0)
        gtk_entry_set_text(GTK_ENTRY(widget), "");

}

void
searchbar_init(calltab_t *tab)
{
  if (tab == contacts) {
        addressbook_init();
      }
      else if (tab == history) {
        history_init();
      }
}

GtkWidget* searchbar_new(gchar* searchbar_type) {

  GtkWidget * searchbox;
  GtkWidget* image;
  GtkWidget* ret = gtk_hbox_new(FALSE, 0);

  searchbox = sexy_icon_entry_new();
  image = gtk_image_new_from_stock( GTK_STOCK_FIND , GTK_ICON_SIZE_SMALL_TOOLBAR);
  sexy_icon_entry_set_icon( SEXY_ICON_ENTRY(searchbox), SEXY_ICON_ENTRY_PRIMARY , GTK_IMAGE(image) );
  sexy_icon_entry_add_clear_button( SEXY_ICON_ENTRY(searchbox) );
  gtk_entry_set_text(GTK_ENTRY(searchbox), _("Search"));
  g_signal_connect(GTK_ENTRY(searchbox), "changed", G_CALLBACK(searchbar_entry_changed), NULL);
  g_signal_connect(GTK_ENTRY(searchbox), "grab-focus", G_CALLBACK(searchbar_clear_entry_if_default), NULL);

  gtk_box_pack_start(GTK_BOX(ret), searchbox, TRUE, TRUE, 0);

  if(g_strcmp0(searchbar_type,"history") == 0)
    history_set_searchbar_widget(searchbox);

  return ret;
}

void activateWaitingLayer() {
  gtk_widget_show(waitingLayer);
}

void deactivateWaitingLayer() {
  gtk_widget_hide(waitingLayer);
}