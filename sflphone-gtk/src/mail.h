/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
 *  Author: Florian Desportes <florian.depsortes@savoirfairelinux.com>
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

#ifdef USE_VOICEMAIL

#ifndef _MAIL_H_
#define _MAIL_H_ 1

#include <gtk/gtk.h>

typedef struct {
	gchar    *name;
	gchar    *folder;
	gchar    *from;
	gchar    *date;
	gboolean isPlaying;
	gchar    *all;
	/** The account used to place/receive the call */
//	gchar * accountID;
	/** Unique identifier of the call */
//	gchar * callID;
} mail_t;

typedef struct {
//	GtkTreeStore *store;
	GtkListStore *store;
	GtkWidget    *view;
	GtkWidget    *tree;

//	GQueue *callQueue;
	mail_t *selectedMail;
} mailtab_t;

mailtab_t * voicemailInbox;
//mailtab_t * voicemailOld;


/**
 * Receiving signal from server that listening to voicemail is started
 */
void mail_is_playing(void);

/**
 * Receiving signal from server that listening to voicemail is stopped 
 */
void mail_is_stopped(void);


void mail_catch_error(gchar *err);


mailtab_t* mailtab_init(void);

void create_mail_view(mailtab_t *);

void mail_list_init(mailtab_t *);

void mail_list_clear_all(mailtab_t *);

#endif // _MAIL_H_

#endif // USE_VOICEMAIL

