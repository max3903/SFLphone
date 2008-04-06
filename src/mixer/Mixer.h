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


#ifndef MIXER_H
#define MIXER_H

#include <vector>

#include "OutputStream.h"
#include "InputStreams.h"

#include "NoSynch.h"
#include "AudioSynch.h"

#include "InternalBuffer.h"

#include "StraightThrough_AT.h"
#include "AudioMixer2Channels.h"

#include "StraightThrough_VT.h"
#include "VideoMixer2Channels.h"

#include "../tracePrintSFL.h"

#define MIXER_TRACE		4

using namespace std;

//! Audio and video mixer
/*!
 * This class implements a modular audio and video stream mixer that can take an arbitrairy number of InputStreams and converts them to 1 audio et 1 video OutputStreams.
 */
class Mixer {
public:

	//! Mixer type
	/*!
	 * Enumerates the possible types of mixers.
	 */
	enum Tmixer {  NOSYNCH_AV_STRAIGHTTHROUGH= 0x0, /*!No Synchronization Manager and audio-video straight Through mixer */
	               SYNCH_AV_STRAIGHTTHROUG= 0x01,   /*!Synchronization Manager and audio-video straight Through mixer */
	               NOSYNCH_AV_MIXER2= 0x02,			/*!No Synchronization Manager and audio-video 2 channels mixer */
	               SYNCH_AV_MIXER2= 0x03,			/*!Synchronization Manager and audio-video 2 channels mixer */
	               NOSYNCH_AUTOSENSE= 0x04,			/*!No Synchronization Manager and auto-detect mixer state from input */
	               SYNCH_AUTOSENSE= 0x05			/*!Synchronization Manager and auto-detect mixer stater from input */
		    };

	//! Constructor
	/*!
	 * Create a mixer from the type, inputs and ouput specified
	 * 
	 * \param type the type of the mixer
	 * \param inputs the InputStreams of the mixer
	 * \param audioOut the audio output stream see OutputStream
	 * \param videoOut the cideo output stream see OutputStream
	 */
	Mixer(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut);

	//! Destructor
	/*
	 * Stops all the running threads
	 */
	~Mixer();
	
	//! Start the mixers operation
	/*!
	 * Starts all the thread that are needed.
	 */
    void start();
    
    //! Method to add an input stream
    /*!
     * Adds if possible an InputStream and creates all the underlying buffers and managers
     */
    bool addStream(InputStreams* input);
    
    //! Method to remove an input stream
    /*!
     * Removes the specified InputStream from the mixer if possible. One stream must be present in the mixer at all time
     */
    bool removeStream(InputStreams* input);
    
    //! Terminates the running mixer and all the threads
    void terminate();

private:

	//! Default constructor
	/*!
	 * Declared private to prevent the creation of an inputless or outputless mixer
	 */
    Mixer();
       
    //! Creates a 2 channel audio-video mixer
    /*!
     * \param type the type of mixer see Tmixer
     * \param inputs a vector of InputSteams
     * \param audioOut the audio ouput buffer
     * \param videoOut the video ouput buffer
     */
    void createMixer2(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut);
    
    //! Creates a straight through audio-video mixer (1 channel)
    /*!
     * \param type the type of mixer see Tmixer
     * \param inputs a vector of InputSteams
     * \param audioOut the audio ouput buffer
     * \param videoOut the video ouput buffer
     */
    void createStraightThrough(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut);
    
    //! Creates a mixer from the information it receives.
    /*!
     * \param type the type of mixer see Tmixer
     * \param inputs a vector of InputSteams
     * \param audioOut the audio ouput buffer
     * \param videoOut the video ouput buffer
     */
    void createAutoSense(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut);
    
    //! Initializes the mixer
    /*!
     * Call the needed create method
     * 
     * \param type the type of mixer see Tmixer
     * \param inputs a vector of InputSteams
     * \param audioOut the audio ouput buffer
     * \param videoOut the video ouput buffer
     * 
     * \sa createAudio1, createMixer2, createStraightThrough, createAutoSense
     */
    void init(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut);

    //! Vector containing the synchronization managers
    vector<SynchManager*> synchManagers;
    
    //! The audio mixer in usage
    AudioMixer* audioTranscoder;
    
    //! The video mixer in usages
    VideoMixer* videoTranscoder;
    
    //! The current type of the mixer
    Tmixer theType;

    //! The vector containning the inputs streams
    vector<InputStreams*> streamsInput;
    
    //! Audio output buffer
    OutputStream* audioOutput;
    
    //! video output buffer
    OutputStream* videoOutput;
    
    //! the vector containing the Internal buffers
    vector<InternalBuffer*> intBuffers;

};
#endif //MIXER_H
