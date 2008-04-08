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

#ifndef VIDEOOUTPUT_H
#define VIDEOOUTPUT_H

#include <list>
#include "OutputStream.h"
#include "../tracePrintSFL.h"

#define VIDEOOUTPUT_TRACE	10

//! This struct contains all information relative to the data
struct VideoOutputPacket{
	int size;
	int width;
	int height;
	unsigned char* data;
};

//! Video Ouput Buffer
/*!
 * This class acts as an Video output buffer. It implements the OutputStream interface
 */
class VideoOutput : public OutputStream {
public:

	//! Default constructor
	/*!
	 * Initialises the internal buffer
	 */
    VideoOutput();

    //! Destructor
    /*!
     * Cleanly diposes of the buffer
     */
    ~VideoOutput();

    //! Method to get the data contained by the buffer
    /*!
     * \param data a pointer to the where the data will be put
     * \param width The width of the output image
     * \param height The heigh of the output image
     * \return the size of the buffer
     */
    virtual unsigned char* fetchData( int &size, int &width, int &height);

    //! Method to change the data contained in the buffer
    /*!
     * The format of the data pointer is the same as the format made by VideoInput::fetchData.
     * \param data a pointer to the data to copy
     * \param size th size of the buffer
     */ 
    virtual void putData( unsigned char * data, int size);
    
    //! Method to get the current Type of ouput buffer
    /*
     * \return A string containing the description of the buffer
     */
    virtual const char* getType() const;


protected:
	
    //! Put protection
    sem_t sem_putData;
   
    // The queue in wich data is stored
    std::list<VideoOutputPacket*> fifo;
    
private:
	
	//! Method to change the data in the buffer
	/*!
	 * This method is implemented as private and \b should \b not \b be \b used in VideoOutput. This method is inherited from OutputStream. putData(char * data, int size) instead. 
	 */
    virtual void putData(short * data, int size) __attribute__ ((deprecated));
    
    //! Method to get the data in the buffer
	/*!
	 * This method is implemented as private and \b should \b not \b be \b used in VideoOutput. This method is inherited from OutputStream. Use fetchData(char* data) instead. 
	 */
    virtual short* fetchData(int &size) __attribute__ ((deprecated));

};
#endif //VIDEOOUTPUT_H
