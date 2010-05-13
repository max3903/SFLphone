/*
 *  Copyright (C) 2010 Savoir-Faire Linux inc.
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
 */
#include <stdlib.h>
#include <errno.h>

typedef struct {
  char* path;
  int fd;
  void* addr;
  off_t size;
} sflphone_shm_t;

/**
 * Create a new sflphone_shm_t structure.
 * @param location The location for the shm (eg /dev/shm/sflphone)
 * @return The new structure, or NULL.
 */
sflphone_shm_t* sflphone_new_video_shm_type(char* location);

/**
 * @param shm The sflphone_shm_t structure to free.
 * @return 0 on success, <0 on failure.
 */
int sflphone_free_video_shm_type(sflphone_shm_t* shm);

/**
 * Open and attach to a existing shared memory segment.
 * @param An existing shared memory segment structure that reprents the shm to open.
 * @return 0 on success, <0 on failure.
 */
int sflphone_open_video_shm (sflphone_shm_t* shm);

/**
 * Close a previously opened shared memory segment.
 * @param An existing shared memory segment structure that reprents the shm to close.
 * @return 0 on success, <0 on failure.
 */
int sflphone_close_video_shm(sflphone_shm_t* shm);

/**
 * @return The address in the process' address space for this shared memory segment.
 */
void * sflphone_get_video_shm_addr(sflphone_shm_t* shm);
