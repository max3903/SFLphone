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

#ifndef __SHM_H__
#define __SHM_H__

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

typedef struct {
  char* path;
  int fd;
  void* addr;
  off_t size;
} sflphone_shm_t;

/**
 * Create a new sflphone_shm_t structure.
 * @return The new structure, or NULL.
 */
sflphone_shm_t* sflphone_shm_new();

/**
 * Create a new sflphone_shm_t structure.
 * @param path The location for the shm (eg /dev/shm/sflphone)
 * @return The new structure, or NULL.
 */
sflphone_shm_t* sflphone_shm_new_with_path(char* path);

/**
 * Set the path and only that.
 * @param shm The sflphone_shm_t structure in which to set the path attribute.
 * @param path The path for the shm (eg /dev/shm/sflphone)
 */
int sflphone_shm_set_path(sflphone_shm_t* shm, char* path);

/**
 * @param shm The sflphone_shm_t structure to free.
 * @return 0 on success, <0 on failure.
 */
int sflphone_shm_free(sflphone_shm_t* shm);

/**
 * Open and attach to a existing shared memory segment.
 * @param An existing shared memory segment structure that reprents the shm to open.
 * @return 0 on success, <0 on failure.
 */
int sflphone_shm_open (sflphone_shm_t* shm);

/**
 * Close a previously opened shared memory segment.
 * @param An existing shared memory segment structure that reprents the shm to close.
 * @return 0 on success, <0 on failure.
 */
int sflphone_shm_close(sflphone_shm_t* shm);

/**
 * @return The address in the process' address space for this shared memory segment.
 */
void * sflphone_shm_get_addr(sflphone_shm_t* shm);

/**
 * Blocking call.
 * @param The maximum time (in usec) to wait for the file to become non-zero.
 * @return < 0 If the segment is still zero after max_wait usec.
 */
int sflphone_shm_ensure_non_zero (sflphone_shm_t* shm, useconds_t max_wait);

/**
 * @param An existing shared memory segment structure.
 * @return The current size in bytes for the shared memory segment.
 */
off_t sflphone_shm_get_size(sflphone_shm_t* shm);
#endif