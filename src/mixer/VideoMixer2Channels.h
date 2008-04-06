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

#ifndef VIDEOMIXER2CHANNELS_H
#define VIDEOMIXER2CHANNELS_H
#include "VideoMixer.h"
#include "../tracePrintSFL.h"

#define VIDEOMIXER2CHANNELS_TRACE 9

//! 2 Channel video mixer implementation
/*!
 * This class implement a mixer that takes in 2 video sources and outputs only one.
 */
class VideoMixer2Channels : public VideoMixer {
public:

	//! Contructor
	/*! 
	 * Assigns the input and output buffers
	 * 
	 * \param video1 an input buffer see InternalBuffer
	 * \param video2 an input buffer see InternalBuffer
	 * \param output an output buffer see OutputStream
	 * \param infos a pointer to a CodecInfo object
	 */
    VideoMixer2Channels(InternalBuffer* video1, InternalBuffer* video2, OutputStream* output, CodecInfo* infos);

    //! Destructor
    /*!
     * Terminates the thread, you can also call the terminate method explicitly
     */
    ~VideoMixer2Channels();

    //! Thread main method
	/*!
	 * Actually does the work in the thread. It mixes the 2 inputs into one output.
	 */
    virtual void run();

    //! Pauses the work done by the run method
    virtual void pause();
    
    //! Resumes the work done by the run method
    /*!
     * Normally used after a pause.
     */
    virtual void restart();
    
    virtual void stop();
    
    void mixVideo(/*char* data1, char* data2, int width1, int height1, int width2, int height2, int paddingWidth, char* mixedVideo*/);

private:
	
	//! Default Constructor
	/*!
	 * Implemented as private to prevent declaration without the proper buffers
	 */
    VideoMixer2Channels();
    
    InternalBuffer* inputBuffer1;
    InternalBuffer* inputBuffer2;
    
    OutputStream* outputBuffer;
    CodecInfo* infoCodecs;

    char* data1;
    char* data2;
    char* mixedData;
    
    int width1;
    int height1;
    
    int width2;
    int height2;
    
    int paddingWidth;// pour la bande noire de 10 pix
    
    int sizeBuffer1;
    int sizeBuffer2;
    int sizeBufferMixedData;
    int paddingSize; // pour la bande noire de 10 pix
    
    bool Active;
    bool OkToKill;
    
};
#endif //VIDEOMIXER2CHANNELS_H
