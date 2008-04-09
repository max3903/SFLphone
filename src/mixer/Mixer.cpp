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


#include "Mixer.h"

Mixer::Mixer()
{
}

Mixer::Mixer(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut)
{
  
  audioTranscoder=NULL;
  videoTranscoder=NULL;
  audioOutput=NULL;
  videoOutput=NULL;
    
  init(type,inputs,audioOut,videoOut);
}

Mixer::~Mixer()
{
}

void Mixer::init(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut)
{
  theType=type;
  
  switch(theType)
  {
    case NOSYNCH_AV_STRAIGHTTHROUGH:
    	this->createStraightThrough(type, inputs, audioOut, videoOut);      
        break;
    case SYNCH_AV_STRAIGHTTHROUG: //TODO:
        break;
    case NOSYNCH_AV_MIXER2:
    	this->createMixer2(type, inputs, audioOut, videoOut);
        break;
    case SYNCH_AV_MIXER2: //TODO:
        break;
    case NOSYNCH_AUTOSENSE: //TODO:
        break;
    case SYNCH_AUTOSENSE: //TODO:
        break;
  }
}

void Mixer::start( bool startVideo )
{
  // Starting all synchManagers
  for(int i=0; i<synchManagers.size();i++)
  {
    synchManagers[i]->start();
  }
  
  //Starting Audio Mixer
  audioTranscoder->start();
  
  // Starting Video Mixer
  if( startVideo )
  	videoTranscoder->start();
  
}

void Mixer::terminate()
{
  // Stopping all synchManagers
  for(int i=0; i < synchManagers.size() ;i++)
  {
    synchManagers[i]->stop();
  }
  
  // Stopping Audio mixer
  audioTranscoder->stop();
  
  // Stopping Video mixer
  videoTranscoder->stop();

}

void Mixer::createAutoSense(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut)
{
  	// \TODO: To implement: Must create a mixer with the supplied paramaters (without nowing the type)
}

void Mixer::createStraightThrough(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut)
{
	ptracesfl("Creating StraightThrough audio and video mixer ...", MT_INFO, MIXER_TRACE);
	this->streamsInput.push_back(inputs[0]);
	
	ptracesfl("Affecting outputs", MT_INFO, MIXER_TRACE);
	// Setting outputs
  	audioOutput=audioOut;
  	videoOutput=videoOut;
  	
	// Create 2 internal buffer, one for video and the other for audio	
	this->intBuffers.push_back( new InternalBuffer() );
	this->intBuffers.push_back( new InternalBuffer() );
	
	int index1= this->intBuffers.size() - 1;
	int index2= this->intBuffers.size() - 2;
	
	// Creating 2 StraightThrough Mixer
    videoTranscoder = new StraightThrough_VT(this->intBuffers[index1],videoOutput);  
    audioTranscoder = new StraightThrough_AT(this->intBuffers[index2],audioOutput);
         
    // Creating NoSynch SynchManager
     synchManagers.push_back( new NoSynch(this->streamsInput[0] ,this->intBuffers[index1],this->intBuffers[index2]) );
 
}

void Mixer::createMixer2(Tmixer type, vector<InputStreams*> inputs, OutputStream* audioOut, OutputStream* videoOut)
{
  	ptracesfl("Creating 2 channels audio and video mixer ...", MT_INFO, MIXER_TRACE);
  	
	this->streamsInput.push_back(inputs[0]);
	this->streamsInput.push_back(inputs[1]);
	
	ptracesfl("Affecting outputs", MT_INFO, MIXER_TRACE);
	// Setting outputs
  	audioOutput=audioOut;
  	videoOutput=videoOut;
  	
	// Create 4 internal buffer, one for video and the other for audio	
	this->intBuffers.push_back( new InternalBuffer() );
	this->intBuffers.push_back( new InternalBuffer() );
	this->intBuffers.push_back( new InternalBuffer() );
	this->intBuffers.push_back( new InternalBuffer() );
	
	int index1= this->intBuffers.size() - 1;
	int index2= this->intBuffers.size() - 2;
		
	// Creating 2 StraightThrough Mixer
    videoTranscoder = new VideoMixer2Channels(this->intBuffers[index1], this->intBuffers[index1 - 2], videoOutput);  
    audioTranscoder = new AudioMixer2Channels(this->intBuffers[index2], this->intBuffers[index2 - 2], audioOutput);
         
    // Creating NoSynch SynchManagers
    synchManagers.push_back( new NoSynch(this->streamsInput[0] ,this->intBuffers[index1],this->intBuffers[index2]) );
    synchManagers.push_back( new NoSynch(this->streamsInput[1] ,this->intBuffers[index1-2],this->intBuffers[index2-2]) );
}

bool Mixer::addStream(InputStreams* input)
{
		
	if( this->streamsInput.size() < 2 ){
		
		// Adding input stream
		this->streamsInput.push_back(input);
		
		// Adding corresponfing internal buffers
		this->intBuffers.push_back( new InternalBuffer() );
		this->intBuffers.push_back( new InternalBuffer() );
		
		int index1= this->intBuffers.size() - 1;
		int index2= this->intBuffers.size() - 2;
		
		// Adding conrecponding SynchManager
		switch(theType)
		{
		    case NOSYNCH_AV_STRAIGHTTHROUGH:
		    	this->synchManagers.push_back( new NoSynch( this->streamsInput[1] ,this->intBuffers[index1],this->intBuffers[index2] ) ); 
		        break;
		    case SYNCH_AV_STRAIGHTTHROUG: //TODO:
		        break;
		}
				
		this->terminate();
		
		delete this->videoTranscoder;
		delete this->audioTranscoder;
		
		// Creating new Mixers
		videoTranscoder = new VideoMixer2Channels(this->intBuffers[index1], this->intBuffers[index1 - 2], videoOutput);  
    	audioTranscoder = new AudioMixer2Channels(this->intBuffers[index2], this->intBuffers[index2 - 2], audioOutput);
    	
    	this->start( false );		
		
	}else{
		ptracesfl("Mixer - addStream(): Cannot add more than 1 input to the mixer !", MT_FATAL, MIXER_TRACE);
		return false;
	}
	
 	return true;
}

bool Mixer::removeStream(InputStreams* input){
	
	// \TODO: Add support for convertion to something other than straithrough 
	
	if( this->streamsInput.size() > 1 ){
		
		this->terminate();
		
		// Removing input
		this->streamsInput.pop_back();
		
		// Removing associated interball buffers
		this->intBuffers.pop_back();
		this->intBuffers.pop_back();
		
		// Removing synch manager
		this->synchManagers.pop_back();
		
		// Removing mixers
		delete this->videoTranscoder;
		delete this->audioTranscoder;
		
		
		int index1= this->intBuffers.size() - 1;
		int index2= this->intBuffers.size() - 2;
	
		// Creating new mixers
	    videoTranscoder = new StraightThrough_VT(this->intBuffers[index1],videoOutput);  
	    audioTranscoder = new StraightThrough_AT(this->intBuffers[index2],audioOutput);
		
		
		this->start();
		
	}else{
		ptracesfl("Mixer - removeStream(): Mixer must have a minimum of 1 input buffer !", MT_FATAL, MIXER_TRACE);
		return false;
	}
		
	return true;
}

