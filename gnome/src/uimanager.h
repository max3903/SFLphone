/*
 *  Copyright (C) 2004, 2005, 2006, 2008, 2009, 2010, 2011 Savoir-Faire Linux Inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "icons/icon_factory.h"

G_BEGIN_DECLS

guint holdConnId;     //The hold_menu signal connection ID

GtkWidget *historyButton;
GtkWidget *contactButton;

GtkAction *volumeToggle;

GtkUIManager *uimanager_new (void);

void update_voicemail_status (void);

void update_actions (void);

void show_popup_menu (GtkWidget *my_widget, GdkEventButton *event);

void show_popup_menu_history (GtkWidget *my_widget, GdkEventButton *event);

void show_popup_menu_contacts (GtkWidget *my_widget, GdkEventButton *event);

void create_menus (GtkUIManager *ui_manager, GtkWidget **widget);

void create_toolbar_actions (GtkUIManager *ui_manager, GtkWidget **widget);

G_END_DECLS

#endif
