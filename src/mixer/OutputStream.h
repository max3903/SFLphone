/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean Tessier <jean.tessier@polymtl.ca> 
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

#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <stdio.h>
#include <string.h>
#include <semaphore.h>

/** @interface */
//! Interface for the output buffers.
/*
 * Supplies all that is needed to use the ouput buffers.
 */
class OutputStream {
public:

	//! Method to get the data in the buffer
	/*!
	 * Must be implemented in child class. This version is for the video buffer.
	 * 
	 * \param data a pointer to where the data is to be copied
	 * \return the size of data
	 */
    virtual int fetchData(char* data) =0;

	//! Method to get the data in the buffer
	/*!
	 * Must be implemented in child class. This version is for the audio buffer.
	 * 
	 * \param data a pointer to where the data is to be copied
	 * \return the size of data
	 */
    virtual int fetchData(short* data) =0;

    //! Method to change the data in the buffer
	/*!
	 * Must be implemented in child class. This version is for the video buffer.
	 * 
	 * \param data a pointer to the new data
	 * \param size the size of data
	 */
    virtual void putData(char * data, int size) =0;

    //! Method to change the data in the buffer
	/*!
	 * Must be implemented in child class. This version is for the audio buffer.
	 * 
	 * \param data a pointer to the new data
	 * \param size the size of data
	 */
    virtual void putData(short * data, int size) =0;
    
    //! Method to get the current Type of ouput buffer
    /*!
     * Must be implemented in child class. This version is for the audio buffer.
     * \return A string containing the description of the buffer
     */
    virtual const char* getType() const= 0;

};
#endif //OUTPUTSTREAM_H
