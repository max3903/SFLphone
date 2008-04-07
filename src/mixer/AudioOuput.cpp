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

#include "AudioOuput.h"

AudioOutput::AudioOutput()
{
  sem_init(&sem_putData,0,1);
}

AudioOutput::~AudioOutput()
{
}

int AudioOutput::fetchData(short *data)
{ 
  if(data!=NULL && this->fifo.size() != 0)
  {
  	
  	//Getting queue head
  	sem_wait(&sem_putData);
    AudioOutPacket * tmpPak= this->fifo.front();
    sem_post(&sem_putData);
        
    memcpy(data, tmpPak->data, tmpPak->size);
    
    int sizeBuffer= tmpPak->size;
    
    // Removing head
    sem_wait(&sem_putData); 
    this->fifo.pop_front();
    sem_post(&sem_putData);
        
    return sizeBuffer;
  		
  }
  else
  {
    if( data == NULL )
  		ptracesfl("AudioOuput - fetchData(): bad parameter passed NULL pointer",MT_ERROR, AUDIOOUTPUT_TRACE);
  	else
    	ptracesfl("AudioOuput - fetchData(): empty buffer",MT_WARNING, AUDIOOUTPUT_TRACE);
    	
    return -1;
    
  }
}

void AudioOutput::putData(short * data, int size)
{ 
  if (data!=NULL && size > 0)
  {
  		
  		// Creating new data packet
   		AudioOutPacket * tmpPak= new AudioOutPacket;
	    tmpPak->data= new short[size];
	    tmpPak->size= size;
	    
	    memcpy(tmpPak->data, data, tmpPak->size);
	
		// inserting data packet
		sem_wait(&sem_putData); 
	    	this->fifo.push_back(tmpPak);
	    sem_post(&sem_putData);
 
  }
  else
    ptracesfl("AudioOuput - putData(): parameter error passed NULL pointer or size < 0",MT_ERROR,AUDIOOUTPUT_TRACE);
}

// DEPRECIATED !!!!!
int AudioOutput::fetchData(unsigned char* data, int &width, int &height)
{
  ptracesfl("AudioOuput - fetchData(char* data): This method should never be called in an AudioOutput Buffer",MT_FATAL,AUDIOOUTPUT_TRACE);
  return -1; 
}
// DEPRECIATED !!!!!
void AudioOutput::putData(unsigned char * data, int size)
{ 
  ptracesfl("AudioOuput - fetchData(char* data): This method should never be called in an AudioOutput Buffer",MT_FATAL,AUDIOOUTPUT_TRACE);
}

int AudioOutput::getSize(){
	
	if( this->fifo.size() == 0)
		return 0;
	
	sem_wait(&sem_putData);
	AudioOutPacket * tmpPak= this->fifo.front();
	int sizeBuffer= tmpPak->size;
	sem_post(&sem_putData);
	
	return sizeBuffer;
		
}

const char* AudioOutput::getType() const{
	return "Audio Ouput Buffer";
}
