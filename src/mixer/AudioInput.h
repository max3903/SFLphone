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

#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <list>
#include <semaphore.h>

#include "TimeInfo.h"
#include "../tracePrintSFL.h"

#define AUDIOINPUT_TRACE	10

//! This struct contains all information relative to the data
struct AudioPacket{
	int size;
	short* data;
	TimeInfo* timeStamp;	
};

//!  Audio input buffer
/*!
  This class is the audio input buffer of a mixer InputStreams. It is managed as a first in first out queue.
*/
class AudioInput {
public:

	//! Default constructor
	/*!
	 * 	Initialise the buffer
	 */
    AudioInput();

    //! Destructor
    /*
     *	Cleanly disposes of the buffer
     */
    virtual ~AudioInput();

    //! Changes the data contained in the buffer
    /*!
     * This method changes the data contained in the buffer. Buffer access is protected to prevent simultanious access.
     * 
     * \param data a pointer to a data buffer
     * \param size the size of the buffer
     */
    virtual void putData(short *data, int size, int timeStamp= 0);

    //! Access the data contained the the buffer
    /*!
     * This method returns the data. Buffer access is protected to prevent simultanious access. Once you fetch data from the buffer the data, size and time stamp no longuer exits
     * 
     * \param data a pointer to where the data must be copied
     * \return The size of the fetched data or -1 if an error occured 
     */
    virtual short* fetchData(int &size);

	//! Method to get the current size of the buffer
	/*!
	 * \return The size of the head of the buffer. Returns 0 if the queue is empty
	 */	 
    virtual int getSizeBuffer();
    
    //! Access the timing information for current buffer
    /*!
     * This method returns the timestamp for the data contained in the head of the queue. The data is used by the synchonization manager to mix the streams 
     */
    virtual TimeInfo* fetchTimeInfo();

private:
    
    //! Put protection
    sem_t sem_putData;
   
    // The queue in wich data is stored
    std::list<AudioPacket*> fifo;
    
};
#endif //AUDIOINPUT_H
