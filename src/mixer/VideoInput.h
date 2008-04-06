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

#ifndef VIDEOINPUT_H
#define VIDEOINPUT_H

#include <list>
#include <semaphore.h>
#include <string.h>

#include "TimeInfo.h"
#include "../tracePrintSFL.h"

#define VIDEOINPUT_TRACE	10

//! This struct contains all information relative to the data
struct VideoPacket{
	int size;
	unsigned char* data;
	TimeInfo* timeStamp;
};

//!  Video input buffer
/*!
 * This class is the video input buffer of a mixer InputStreams
 */
class VideoInput {
public:

	//! Default Constructor
	/*!
	 * 	Initialise the buffer
	 */
    VideoInput();

    //! Destructor
    /*
     *	Cleanly disposes of the buffer
     */
    ~VideoInput();

    //! Access the data contained the the buffer
    /*!
     * This method returns the data. Buffer access is protected to prevent simultanious access. 
     * The pointer points to data with the flowing format: [w][h][payload]. 
     * So the size returned is the sum of the size of two int and the payload.
     * 
     * \param data a pointer to where the data must be copied
     * \param w the width of the image
     * \param h the height of the image
     * \return the size of the fetched data or -1 if an error occured
     */
    virtual int fetchData(unsigned char* data);

    //! Changes the data contained in the buffer
    /*!
     * This method changes the data contained in the buffer. Buffer access is protected to prevent simultanious access.
     * 
     * \param data a pointer to a data buffer
     * \param size the size of the buffer
     * \param timeStamp timing information of the video packet
     * \param w the width of the image
     * \param h the height of the image
     */
    virtual void putData(char * data, int size, int timeStamp, int w, int h);

	//! Method to get the current size of the buffer
	/*!
	 * \return the the total size(ie: see fetchData) of the head of the queue or 0 if the queue is empty
	 */	 
    virtual int getSizeBuffer();
    
    //! Access the timing information
    /*!
     * This method returns the timestamp for the data contained in the buffer. The data is used by the synchonization manager to mix the streams 
     */
    virtual TimeInfo* fetchTimeInfo();

private:
    
    //! Queue access protection
    sem_t sem_putData;
    
    //! The data queue
	std::list< VideoPacket* > fifo;
    
};
#endif //VIDEOINPUT_H
