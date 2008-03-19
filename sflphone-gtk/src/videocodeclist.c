/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: marilyne Mercier <marilyne.mercier@polymtl.ca> 
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

#include <videocodeclist.h>

#include <string.h>
#include <stdlib.h>

#include "dbus.h"

GQueue * videoCodecQueue = NULL;

gint
is_video_name_codecstruct (gconstpointer a, gconstpointer b)
{
  videoCodec_t * c = (videoCodec_t *)a;
  if(strcmp(c->name, (const gchar *)b)==0)
    return 0;
  else
    return 1;
}

gint
is_video_payload_codecstruct (gconstpointer a, gconstpointer b)
{
  videoCodec_t * c = (videoCodec_t *)a;
  if(c->_payload == (int)b)
    return 0;
  else
    return 1;
}

void
video_codec_list_init()
{
  videoCodecQueue = g_queue_new();
}

void
video_codec_list_clear ()
{
  g_queue_free (videoCodecQueue);
  videoCodecQueue = g_queue_new();
}

void
video_codec_list_add(videoCodec_t * c)
{
  g_queue_push_tail (videoCodecQueue, (gpointer *) c);
}


void 
video_codec_set_active(gchar* name)
{
	videoCodec_t * c = video_codec_list_get(name);
	if(c)
	{
		printf("%s set active\n", c->name);
		c->is_active = TRUE;
	}
}

void
video_codec_set_inactive(gchar* name)
{
  videoCodec_t * c = video_codec_list_get(name);
  if(c)
    c->is_active = FALSE;
}

guint
video_codec_list_get_size()
{
  return g_queue_get_length(videoCodecQueue);
}

videoCodec_t*
video_codec_list_get( const gchar* name)
{
  GList * c = g_queue_find_custom(videoCodecQueue, name, is_video_name_codecstruct);
  if(c)
    return (videoCodec_t *)c->data;
  else
    return NULL;
}

videoCodec_t*
video_codec_list_get_nth(guint index)
{
  return g_queue_peek_nth(videoCodecQueue, index);
}

void
video_codec_set_prefered_order(guint index)
{
  videoCodec_t * prefered = video_codec_list_get_nth(index);
  g_queue_pop_nth(videoCodecQueue, index);
  g_queue_push_head(videoCodecQueue, prefered);
}

/**
 * 
 */
void
video_codec_list_move_codec_up(guint index)
{
	if(index != 0)
	{
		gpointer videoCodec = g_queue_pop_nth(videoCodecQueue, index);
		g_queue_push_nth(videoCodecQueue, videoCodec, index-1);
	}
	
	// DEBUG
	int i;
	printf("\nCodec list\n");
	for(i=0; i < videoCodecQueue->length; i++)
		printf("%s\n", video_codec_list_get_nth(i)->name);
}

/**
 * 
 */
void
video_codec_list_move_codec_down(guint index)
{
	if(index != videoCodecQueue->length)
	{
		gpointer videoCodec = g_queue_pop_nth(videoCodecQueue, index);
		g_queue_push_nth(videoCodecQueue, videoCodec, index+1);
	}

	// PRINT
	int i;
	printf("\nCodec list\n");
	for(i=0; i < videoCodecQueue->length; i++)
		printf("%s\n", video_codec_list_get_nth(i)->name);
}

/**
 * 
 */
void
video_codec_list_update_to_daemon()
{
	// String listing of all codecs payloads
	const gchar** videoCodecList;
	
	// Length of the codec list
	int length = videoCodecQueue->length;
	
	// Initiate double array char list for one string
	videoCodecList = (void*)malloc(sizeof(void*));
	
	// Get all codecs in queue
	int i, c = 0;
	printf("List of active codecs :");
	for(i = 0; i < length; i++)
	{
		videoCodec_t* currentCodec = video_codec_list_get_nth(i);
		// Assert not null
		if(currentCodec)
		{
			// Save only if active
			if(currentCodec->is_active)
			{
				// Reallocate memory each time more than one active codec is found
				if(c!=0)
					videoCodecList = (void*)realloc(videoCodecList, (c+1)*sizeof(void*));
				// Allocate memory for the payload
				*(videoCodecList+c) = (gchar*)malloc(sizeof(gchar*));
			//	char name[10];
				// Put name  in char array
				strcpy((char*)*(videoCodecList+c), currentCodec->name);
				g_print(" %s", *(videoCodecList+c));
				c++;
			}
		}
	}
	
	// Allocate NULL array at the end for Dbus
	videoCodecList = (void*)realloc(videoCodecList, (c+1)*sizeof(void*));
	*(videoCodecList+c) = NULL;

	printf("\n");
		
	// call dbus function with array of strings
	dbus_set_active_video_codec_list(videoCodecList);

	// Delete memory
	for(i = 0; i < c; i++) {
		free((gchar*)*(videoCodecList+i));
	}
	free(videoCodecList);
}
