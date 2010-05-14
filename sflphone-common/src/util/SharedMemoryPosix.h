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
#ifndef SHAREDMEMORYPOSIX_H_
#define SHAREDMEMORYPOSIX_H_

#include <stdexcept>
#include <iostream>

namespace sfl {

/**
 * This exception is thrown upon various errors that might occur during
 * operations on a shared memory segment.
 */
class SharedMemoryException: public std::runtime_error {
public:
	SharedMemoryException(std::string const& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * This class is a wrapper class
 * around posix shared memory functions (shm_open).
 * It aims at being a quick way to set up
 * a shared memory object.
 */
class SharedMemoryPosix {
public:
	/**
	 * Create a new shared memory segment, and mmap it to to the process address space.
	 * @param name Path name to the shared memory region. It need not be a path to an existing file.
	 * @param mode Access mode for the memory region. Valid modes are in | out, in, and their combination with trunc.
	 * "app", "ate" and "b" don't have any effect and will be ignored.
	 * @param exclusive If set to true, if the shared memory exists, the constructor will fail.
	 */
	SharedMemoryPosix(const std::string& name, bool exclusive, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) throw(SharedMemoryException);

	/**
	 * Shared memory destructor. It will close, release, and remove the shared memory object.
	 */
	virtual ~SharedMemoryPosix();

	/**
	 * This causes the shared memory object to have size of length bytes.
	 * The truncated shared memory file will be re-attached to the process'
	 * address space.
	 * @param length The new size for the shared memory object.
	 */
	void truncate(off_t length) throw (SharedMemoryException);

	/**
	 * Truncate the shared memory object to one unit of the page size.
	 * @sa truncate(off_t length)
	 */
	void truncate() throw (SharedMemoryException);

	/**
	 * @return a pointer to the shared memory region.
	 */
	void * getRegion() {
		return mappedAddr;
	}

	/**
	 * @return The shared memory name.
	 */
	std::string getName() {
		return name;
	}

	/**
	 * @return The size of the current shared memory region in bytes.
	 */
	off_t getSize() throw(SharedMemoryException);

	/**
	 * Remove the shared memory object.
	 */
	void remove() throw (SharedMemoryException);

protected:
	/**
	 * Establish a mapping between a process' address space and a file and the shared memory object.
	 * This cannot be done before truncation. Therefore it's up to the client to handle this manually.
	 */
	void attach() throw (SharedMemoryException);

	/**
	 * Removes any mappings for those entire pages containing any part of the address space of the process.
	 */
	void release() throw (SharedMemoryException);

	/**
	 * Close the shared memory through its file descriptor.
	 */
	void close() throw (SharedMemoryException);

	/**
	 * @return The file status flags and file access modes of the open file description.
	 */
	int getOflags(std::ios_base::openmode mode);

	/**
	* @return Read, write, execute, or some combination of accesses are permitted to the data being mapped.
	*/
	int getProtFlags(std::ios_base::openmode mode);
private:
	int fd;
	off_t size;
	bool exclusive;
	void * mappedAddr;

	std::string name;
	std::ios_base::openmode mode;
};

}

#endif /*SHAREDMEMORYPOSIX_H_*/
