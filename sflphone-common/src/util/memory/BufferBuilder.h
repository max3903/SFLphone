/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
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
#ifndef __SFL_BUFFER_BUILDER_H__
#define __SFL_BUFFER_BUILDER_H__

#include <list>
#include <utility>
#include <stdexcept>
#include <string.h>

namespace sfl {

/**
 * Thrown when an operation would have to go out of bound.
 */
class MemoryAllocationException: public std::runtime_error {
public:
	MemoryAllocationException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * Utility object for holding data into a buffer. Writing data into the buffer
 * will cause the current write index to move each time.
 */
template <class T>
class BufferBuilder {
public:
	BufferBuilder() : size(0) {}
	virtual ~BufferBuilder() {};

	/**
	 * Push a piece of data into the queue.
	 * @param data A pointer to the data.
	 * @param size The size corresponding to this buffer.
	 * @postcondition The data will be kept in this object as long as the object exists and be assembled into
	 * a continuous buffer when getContinuousBuffer() is called.
	 * @see BufferBuilder#getContinuousBuffer
	 */
	void push(const T* data, size_t size) {
		slices.push_back(std::pair<const T*, size_t>(data, size));
		this->size += size;
	}

	/**
	 * @return the amount of data that was declared so far to be in all of the buffers.
	 */
	size_t getSize() {
		return size;
	}

	/**
	 * @return A new buffer containing all of the buffers in the order in which they were inserted.
	 */
	T* getContinuousBuffer() throw(MemoryAllocationException){

		T* buffer = new T[size];
		if (buffer == NULL) {
			throw MemoryAllocationException("Could not create buffer.");
		}

		Iterator it;
		size_t index = 0;
		for (it = slices.begin(); it != slices.end(); it++) {
			memcpy(buffer + index, (*it).first, (*it).second);
			index += (*it).second;
		}

		return buffer;
	}

	/**
	 * Remove all of the elements in the queue but does not clear the memory allocated for the data
	 * referred to by the pointers.
	 */
	void clear() {
		slices.clear();
		size = 0;
	}

	/**
	 * Remove and free (calls the constructor) for every element in the queue.
	 */
	void dispose() {
		Iterator it;
		for (it = slices.begin(); it != slices.end(); it++) {
			delete const_cast<T*>((*it).first);
		}

		clear();
	}

private:
	std::list<std::pair<const T*, size_t> > slices;
	typedef typename std::list<std::pair<const T*, size_t> >::iterator Iterator;
	size_t size;
};

}

#endif
