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
#ifndef __SFL_QUEUED_BUFFER_H__
#define __SFL_QUEUED_BUFFER_H__

#include <stdlib.h>
#include <string.h>
#include <stdexcept>

namespace sfl {

/**
 * Thrown when an operation would have to go out of bound.
 */
class ArrayOutOfBoundException: public std::runtime_error {
public:
	ArrayOutOfBoundException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * Utility object for holding data into a buffer. Writing data into the buffer
 * will cause the current write index to move each time.
 */
template <class T>
class QueuedBuffer {
public:
	/**
	 * @param size Initial size.
	 */
	QueuedBuffer(size_t size) {
		data = (T*) malloc(size);
		index = 0;
		this->size = size;
	}

	~QueuedBuffer() {
		free(data);
	}

	/**
	 * Copy the source data into the internal buffer at the current index.
	 * @param data The data to be put into the buffer.
	 * @param size The data size.
	 */
	void put(const T* data, size_t size) throw(ArrayOutOfBoundException) {
		if (size + index > this->size) {
			throw ArrayOutOfBoundException("Attempting to write outside of permissible range");
		}
		memcpy(this->data + index, data, size);
		index += size;
	}

	/**
	 * Reset the write index to 0.
	 */
	void reset() {
		index = 0;
	}

	/**
	 * Clear the data inside of the internal buffer.
	 */
	void clear() {
		memset(data, 0, size);
	}

	/**
	 * @return the content of the internal buffer.
	 */
	const T* getBuffer() {
		return data;
	}

	/**
	 * @return The number of bytes in the internal buffer.
	 */
	size_t getSize() {
		return size;
	}

private:
	T* data;
	int index;
	size_t size;
};

}

#endif
