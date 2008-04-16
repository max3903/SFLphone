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

#ifndef INTERNALBUFFER_H
#define INTERNALBUFFER_H

#include <list>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include "../tracePrintSFL.h"

#define INTERNALBUFFER_TRACE	10

//! This struct contains all information relative to the data that goes in the internal buffer
struct DataPacket{
	unsigned char* data;
	int size;
};

//! Internal data sharing class
/*!
 * Acts as an Internal buffer that can be accessed from multiple source.
 */
class InternalBuffer {
public:

	//! Default constructor
	/*!
	 * Initiates the internal buffer
	 */
    InternalBuffer();

    //! Destructor
    /*!
     * Disposes cleanly of the buffer
     */
    ~InternalBuffer();

    //! Changes the data in the buffer
    /*!
     * \param data a pointer to the data to put in the buffer
     * \param size the size of the data
     * \return the success of the operation
     */
    bool putData(void * data, int size);

    //! Gets the data contained in the buffer
    /*!
      \param size size of the data
      \return a pointer to the data
     */
    void* fetchData(int &size);

    //! Method to get the current buffer size
    /*!
     * \return The size of the head of the buffer. Returns 0 if the queue is empty
     */
    int getSizeBuffer();

private:
	
    //! Put protection
    sem_t sem_putData;

	//! The queue of the buffer    
    std::list<DataPacket*> fifo;
    
};
#endif //INTERNALBUFFER_H
