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

#ifndef AUDIOOUPUT_H
#define AUDIOOUPUT_H

#include <list>
#include "OutputStream.h"
#include "../tracePrintSFL.h"

#define AUDIOOUTPUT_TRACE	9

//! This struct contains all information relative to the data
struct AudioOutPacket{
	short* data;
	int size;
};

//! Audio Ouput Buffer
/*!
 * This class acts as an Audio output buffer. It implements the OutputStream interface
 */
class AudioOutput : public OutputStream {
public:

	//! Default constructor
	/*!
	 * Initialises the internal buffer
	 */
    AudioOutput();

    //! Destructor
    /*!
     * Cleanly diposes of the buffer
     */
    ~AudioOutput();

    //! Method to get the data contained by the buffer
    /*!
     * \param data a pointer to the where the data will be put
     * \return the size of the buffer
     */
//    virtual int fetchData(short *data);
    virtual short* fetchData(int &size);

    //! Method to change the data contained in the buffer
    /*!
     * \param data a pointer to the data to copy
     * \param size th size of the buffer
     */ 
    virtual void putData(short * data, int size);
    
    //! Method to get the size of the current buffer
    /*!
     * \return The size of the buffer
     */
    virtual int getSize();
    
    //! Method to get the current Type of ouput buffer
    /*
     * \return A string containing the description of the buffer
     */
    virtual const char* getType() const;

protected:
		   
    //! Put protection
    sem_t sem_putData;
    
    // The queue in wich data is stored
    std::list<AudioOutPacket*> fifo;   
    
private:
	
	//! Method to change the data in the buffer
	/*!
	 * This method is implemented as private and \b should \b not \b be \b used in AudioOutput. This method is inherited from OutputStream. putData(int16 * data, int size) instead. 
	 */
    virtual void putData(unsigned char* data, int size) __attribute__ ((deprecated));
    
    //! Method to get the data in the buffer
	/*!
	 * This method is implemented as private and \b should \b not \b be \b used in AudioOutput. This method is inherited from OutputStream. Use fetchData(int16 *data) instead. 
	 */
    virtual unsigned char* fetchData(int &size, int &width, int &height) __attribute__ ((deprecated));

};
#endif //AUDIOOUPUT_H
