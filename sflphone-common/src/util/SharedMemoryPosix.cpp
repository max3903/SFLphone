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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>

#include "logger.h" 

namespace sfl
{

SharedMemoryPosix::SharedMemoryPosix(const std::string& name, bool exclusive, std::ios_base::openmode mode) throw(SharedMemoryException):
	fd(0),
	size(0),
	mappedAddr(NULL),
	name(name),
	exclusive(exclusive),
	mode(mode)
{
	int oflags = getOflags(mode);
	if (exclusive) {
		oflags |= O_CREAT | O_EXCL;
	} else {
		oflags |= O_CREAT;
	}
	
	if ((fd = shm_open(name.c_str(), oflags, 0)) < 0) {
		throw SharedMemoryException(std::string("shm_open(): ") + strerror(errno));
	}
	
	this->truncate();
	this->map();
}

SharedMemoryPosix::~SharedMemoryPosix()
{
	try {
		unmap();
		this->close();
		this->unlink();
	} catch (SharedMemoryException e) {
		_error((std::string("An unrecoverable exception has occured : ") + e.what()).c_str());
	}
	
}

void SharedMemoryPosix::truncate()
{
	int pageSize = sysconf(_SC_PAGE_SIZE);
	truncate (pageSize);
}

void SharedMemoryPosix::truncate(off_t size)
{
	assert(fd);
	
	if (ftruncate(fd, size) < 0) {
		throw SharedMemoryException(std::string("Truncate: ") + strerror(errno));
	}
	
	this->size = size;
}

void SharedMemoryPosix::map() throw(SharedMemoryException)
{
	assert(fd);
	
	int prot = getProtFlags(mode);
	
	mappedAddr = mmap(NULL, (size_t) size, prot, MAP_SHARED, fd, 0);
	if (mappedAddr == MAP_FAILED) {
		this->close();
		this->unlink();		
		throw SharedMemoryException(std::string("mmap(): ") + strerror(errno));
	}
}

void SharedMemoryPosix::unmap() throw(SharedMemoryException)
{
	assert(mappedAddr);
	
	if (munmap(mappedAddr, size) < 0) {
		throw SharedMemoryException(std::string("mumap(): ") + strerror(errno));
	}
	
	mappedAddr = NULL;
	size = 0;
}

void SharedMemoryPosix::close() throw(SharedMemoryException)
{
	assert(fd);

	if (::close(fd) < 0) {
		throw SharedMemoryException(std::string("close(): ") + strerror(errno));
	}
	
	fd = 0;
}

void SharedMemoryPosix::unlink() throw(SharedMemoryException)
{
	if (shm_unlink(name.c_str()) < 0) {
		throw SharedMemoryException(std::string("unlink(): ") + strerror(errno));
	}
}

int SharedMemoryPosix::getOflags(std::ios_base::openmode mode)
{
	int oflags = 0;
	if ((mode & std::ios::in) && (mode & std::ios::out)) {
		oflags |= O_RDWR;
	} else if (mode & std::ios::in){
		oflags |= O_RDONLY;
	}
	
	if (mode & std::ios::trunc) {
		oflags |= O_TRUNC;
	}
	
	return oflags;
}

int SharedMemoryPosix::getProtFlags(std::ios_base::openmode mode)
{
	int prot = PROT_NONE;
	
	if (mode & std::ios::in) {
		prot = PROT_READ | PROT_EXEC;
	}
	
	if (mode & std::ios::out) {
		prot |= PROT_WRITE | PROT_EXEC;
	}
	
	return prot;
}

}
