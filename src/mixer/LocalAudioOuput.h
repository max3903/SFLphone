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


#ifndef LOCALAUDIOOUPUT_H
#define LOCALAUDIOOUPUT_H

#include "AudioOuput.h"
#include "../audio/audiolayer.h"
#include "../tracePrintSFL.h"

#define LOCALAUDIOOUPUT_TRACE	10
//! Local audio ouput
/*!
 * This class overloads the class AudioOuput to send the ouput directly to the sound device. This class is bound to change in a short future.
 */
class LocalAudioOuput : public AudioOutput {
public:

	//! Constructor
	/*!
	 * Initalizes the ouput device
	 */
    LocalAudioOuput();

    //! Destructor
    /*! Closes the device
     */
    virtual ~LocalAudioOuput();

    //! Changes the data in the buffer
    /*!
     * Writes the data to the audio device
     * \param data pointer to the new data
     * \param size the size of data
     */
    virtual void putData(short * data, int size);
    
    //! Method to get the current Type of ouput buffer
    /*
     * \return A string containing the description of the buffer
     */
    virtual const char* getType() const;
    
private:
	
	//! Gets the data in the buffer
    /*!
     * This method is not used in this class
     */
    virtual int fetchData(short *data);
    
    //! Audio Device
    AudioLayer* audioLayer;
};
#endif //LOCALAUDIOOUPUT_H
