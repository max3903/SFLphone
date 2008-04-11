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

#ifndef INPUTSTREAMS_H
#define INPUTSTREAMS_H

#include "VideoInput.h"
#include "AudioInput.h"
#include "../tracePrintSFL.h"

//! Video and audio streams container
/*!
 * This class acts as a container to link the audio feed to the wright video feed.
 */
class InputStreams {
public:

	//! Constructor
	/*!
	 * Creates the container
	 * 
	 * \param videoData a pointer to an VideoInput
	 * \param audioData a pointer to an AudioInput
	 */
    InputStreams(VideoInput* videoData, AudioInput* audioData);
    
    //! Descructors
    /*!
     * The destrcutor dos not destroy the input pointers
     */
    ~InputStreams();
   
    //! Access the video stream
    /*!
     * \return the video stream
     */
    VideoInput* fetchVideoStream() const;

    //! Access the audio stream
    /*!
     * \return the audio stream
     */
    AudioInput* fetchAudioStream() const;

private:
	
	//! Default constructor
	/*!
	 * Implemented as privte to prevent a declaration with no streams
	 */
	InputStreams();
	
	//! VideoInput stream pointer
    VideoInput* videoInput;
    
    //! AudioInput stream pointer
    AudioInput* audioInput;
    
};
#endif //INPUTSTREAMS_H
