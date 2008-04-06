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

#ifndef STRAIGHTTHROUGH_AT_H
#define STRAIGHTTHROUGH_AT_H

#include "AudioMixer.h"
#include "../tracePrintSFL.h"

#define STRAIGHTTHROUGH_TRACE	9

//! Straight Through audio mixer
/*!
 * Used when only one input is specified to the mixer
 */
class StraightThrough_AT : public AudioMixer {
public:

	//! Constructor
	/*!
	 * Initializes the audio mixer
	 * 
	 * \param input a pointer to the InternalBuffer acting as input.
	 * \param output a pointer to where the ouput is to be placed.
	 */
    StraightThrough_AT(InternalBuffer* input, OutputStream* output);

    //! Destructor
    /*!
     * Terminates the thread, you can also call the terminate method explicitly
     */
    ~StraightThrough_AT();
    
    //! Thread main method
	/*!
	 * Actually does the work in the thread. It essentially copies the input to the ouput.
	 */
    virtual void run();

    //! Pauses the work done by the run method
    virtual void pause();

    //! Resumes the work done by the run method
    /*!
     * Normally used after a pause.
     */
    virtual void restart();

    //! Stop and Kill the thread
    virtual void stop();

private:
	
	//! Default Constructor
	/*!
	 * Implemented as private to prevent declaration without the proper buffers
	 */
    StraightThrough_AT();

	//! Internal input buffer
    InternalBuffer* inputBuffer;
    
    //! Output buffer, can be either local or normal output (see LocalAudioOutput or AudioOutput)
    OutputStream* outputBuffer;
    
    //! Weither or not the thread is running
    bool Active;
    
    //! Attribut marking the safe end of the thread.
    bool OkToKill;
    
};
#endif //STRAIGHTTHROUGH_AT_H
