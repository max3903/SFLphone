/*
 *  Copyright (C) 2006-2010 Savoir-Faire Linux inc.
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

#include "SharedMemoryPosix.h"

#include <iostream>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>

#include "logger.h" 

namespace sfl {

SharedMemoryPosix::SharedMemoryPosix(const std::string& name, bool exclusive,
		std::ios_base::openmode mode) throw (SharedMemoryException) :
	fd(0), size(0), mappedAddr(NULL), name(name), exclusive(exclusive), mode(
			mode) {
	int oflags = getOflags(mode);

	if (exclusive == true) {
		oflags |= O_CREAT | O_EXCL; //  If O_EXCL is set and O_CREAT is not set, the result is undefined.
	} else {
		oflags |= O_CREAT;
	}

	if ((fd = shm_open(name.c_str(), oflags, 0666)) < 0) {
		throw SharedMemoryException(std::string("shm_open(): ") + strerror(
				errno));
	}

	if ((mode & std::ios::in) || (mode & std::ios::trunc)) {
		// this->truncate();
	} else {
		// We are in read-only mode and we need to figure out the existing size of the shm before mmap().
		this->size = getFileSize();
	}

	// this->attach();
}

SharedMemoryPosix::~SharedMemoryPosix() {
	try {
		_debug("Releasing ...");
		release();
		_debug("Closing ...");
		this->close();
	} catch (SharedMemoryException e) {
		_error((std::string("An unrecoverable exception has occured : ") + e.what()).c_str());
	}
}

void SharedMemoryPosix::truncate() throw (SharedMemoryException) {
	int pageSize = sysconf(_SC_PAGE_SIZE);
	truncate(pageSize);
}

void SharedMemoryPosix::truncate(off_t size) throw (SharedMemoryException) {
	assert(fd);

	if (ftruncate(fd, size) < 0) {
		throw SharedMemoryException(std::string("Truncate: ") + strerror(errno));
	}

	// We have to re-attach for the new size, else we'll run into segfaults. Detach with old size.
	if (mappedAddr != NULL) {
		release();
	}

	// Update size, and attach with new size value.
	this->size = size;
	attach();
}

void * SharedMemoryPosix::getRegion() {
	return mappedAddr;
}

std::string SharedMemoryPosix::getName() {
	return name;
}

off_t SharedMemoryPosix::getFileSize() throw (SharedMemoryException) {
	assert(fd);

	struct stat buffer;
	if (fstat(fd, &buffer) < 0) {
		throw SharedMemoryException(std::string("getFileSize: ") + strerror(
				errno));
	}

	_debug("shm (%s) is %d bytes long", name.c_str(), buffer.st_size);
	return buffer.st_size;
}

off_t SharedMemoryPosix::getSize() {
	return size;
}

void SharedMemoryPosix::attach() throw (SharedMemoryException) {
	assert(fd);

	mappedAddr = mmap(NULL, (size_t) size, getProtFlags(mode), MAP_SHARED, fd,
			(off_t) 0);
	if (mappedAddr == MAP_FAILED) {
		this->close();
		throw SharedMemoryException(std::string("mmap(): ") + strerror(errno));
	}
}

void SharedMemoryPosix::release() throw (SharedMemoryException) {
	assert(mappedAddr);

	if (munmap(mappedAddr, size) < 0) {
		throw SharedMemoryException(std::string("munmap(): ") + strerror(errno));
	}

	mappedAddr = NULL;
	size = 0;
}

void SharedMemoryPosix::close() throw (SharedMemoryException) {
	assert(fd);

	if (::close(fd) < 0) {
		throw SharedMemoryException(std::string("close(): ") + strerror(errno));
	}

	fd = 0;
}

void SharedMemoryPosix::remove() throw (SharedMemoryException) {
	if (shm_unlink(name.c_str()) < 0) {
		throw SharedMemoryException(std::string("shm_unlink(): ") + strerror(
				errno));
	}
}

int SharedMemoryPosix::getOflags(std::ios_base::openmode mode) {
	int oflags = 0;

	// From the spec : "Applications specify exactly one of the first two values".
	if ((mode & std::ios::in) && (mode & std::ios::out)) {
		oflags = O_RDWR;
	} else if (mode & std::ios::out) {
		oflags = O_RDONLY;
	}

	if (mode & std::ios::trunc) {
		oflags |= O_TRUNC; // The result of using O_TRUNC with O_RDONLY is undefined.
	}

	return oflags;
}

int SharedMemoryPosix::getProtFlags(std::ios_base::openmode mode) {
	int prot = 0;

	if (mode & std::ios::in) {
		prot = PROT_WRITE;
	}

	if (mode & std::ios::out) {
		prot |= PROT_READ;
	}

	return prot;
}

}
