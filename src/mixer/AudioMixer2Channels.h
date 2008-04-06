/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Ala Eddine Limame <ala-eddine.limame@polymtl.ca>
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

#ifndef AUDIOMIXER2CHANNELS_H
#define AUDIOMIXER2CHANNELS_H

#include "AudioMixer.h"
#include "InternalBuffer.h"
#include "OutputStream.h"
#include "../tracePrintSFL.h"

#define AUDIOMIXER2CHANNELS_TRACE	9

//! 2 Channel audio mixer implementation
/*!
 * This class implements a mixer that takes in 2 audio sources and outputs only one.
 */
class AudioMixer2Channels : public AudioMixer {
public:

	//! Contructor
	/*! 
	 * Assigns the input and output buffers
	 * 
	 * \param input1 an input buffer see InternalBuffer
	 * \param input2 an input buffer see InternalBuffer
	 * \param output an output buffer see OutputStream
	 */
    AudioMixer2Channels(InternalBuffer* input1,  InternalBuffer* input2, OutputStream* output);


    //! Destructor
    /*!
     * Terminates the thread, you can also call the terminate method explicitly
     */
    ~AudioMixer2Channels();

	//! Thread main method
	/*!
	 * Actually does the work in the thread. It mixes the 2 inputs into one output.
	 */
    virtual void run();
    
    void pause();
    
    void restart();
    
    void stop();
    
    short mixData(short Data1, short Data2);

private:
	
	//! Default Constructor
	/*!
	 * Implemented as private to prevent declaration without the proper buffers
	 */	
    AudioMixer2Channels();
    
    InternalBuffer* inputBuffer1;
    InternalBuffer* inputBuffer2;
    
    OutputStream* outputBuffer;

    int sizeBuffer1;
    int sizeBuffer2;
    int sizeBufferMixedData;
    
    bool Active;
    bool OkToKill;
    
};
#endif //AUDIOMIXER2CHANNELS_H
