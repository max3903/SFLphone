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

#ifndef NOSYNCH_H
#define NOSYNCH_H

#include "SynchManager.h"
#include "../tracePrintSFL.h"

#define NOSYNCH_TRACE	9

//! Synchonization manager that does not synchronizes
/*!
 * Used when no synchronization is nedded. (ie: 1 input only)
 */
class NoSynch : public SynchManager {
public:

	//! Constructor
	/*!
	 * Initialises the Synchronization Manager.
	 * \param Streams An InputStreams contaning the data to synch
	 * \param video The Internal buffer into wich the synch video data will be sent
	 * \param audio The Internal buffer into wich the synch audio data will be sent
	 */
    NoSynch(InputStreams* Streams, InternalBuffer* video, InternalBuffer* audio, int index);

    //! Destructor
    virtual ~NoSynch();

    //! Thread main method
	/*!
	 * Actually does the work in the thread.
	 */
    virtual void run();

    //! Stop and Kill the thread
    virtual void stop();
    
private:
	
	//! Default Constructor
	/*!
	 * Implemented as private to prevent a declaration without the proper buffers
	 */
    NoSynch();
    
    //! Input stream to the synch manager
    InputStreams* inputStreams;
        
    //! Internal video output
    InternalBuffer* bufferVideo;
    
    //! Internal audio output
    InternalBuffer* bufferAudio;

    //! Attribute representing the state of the thread
    bool Active;
    
    //! Attribute representing the safe stop of the thread
    bool OkToKill;
    
    //! Attribute representing the buffer to query in the input
    int index;

};
#endif //NOSYNCH_H
