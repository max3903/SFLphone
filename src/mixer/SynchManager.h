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

#ifndef SYNCHMANAGER_H
#define SYNCHMANAGER_H

#include <cc++/thread.h> 
#include "InternalBuffer.h"
#include "InputStreams.h"

/** @interface */
//! Synchronization manager interface
/*!
 * Supplies all the methods and attributes needed for a synchmanager
 */
class SynchManager : public ost::Thread {
public:

	SynchManager(){}
	
	virtual ~SynchManager(){}

    //! Thread main method
	/*!
	 * Actually does the work in the thread. Must be implemeted by the child class.
	 */
    virtual void run() = 0;

    //! Stop and Kill the thread
    /*!
     * Must be implemeted by the child class.
     */
    virtual void stop()= 0;

protected:
	
	//! The inputs to the synchronization manager
    InputStreams* input;
    
    //! The InternalBuffer corresponding to the video ouput of this sychmanager
    InternalBuffer* videoBuffer;
    
    //! The InternalBuffer corresponding to the audio ouput of this sychmanager
    InternalBuffer* audioBuffer;
};
#endif //SYNCHMANAGER_H
