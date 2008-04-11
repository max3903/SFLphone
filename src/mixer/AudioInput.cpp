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

#include "AudioInput.h"

AudioInput::AudioInput(){
  sem_init(&sem_putData,0,1);
  this->fifo.push_back( std::list<AudioPacket*>() );
}

AudioInput::~AudioInput(){
}

void AudioInput::putData(short *data, int size, int timeStamp)
{
  if (data!=NULL && size > 0)
  {
    for( int i= 0; i < this->fifo.size(); i++ ){
	    // Creating an audio data packet 
	    AudioPacket * tmpPak= new AudioPacket;
	    tmpPak->data= new short[size];
	    tmpPak->size= size;
	    tmpPak->timeStamp= new TimeInfo(timeStamp); 
	    
	    memcpy(tmpPak->data, data, tmpPak->size);
	
		// Adding data to the fifo
		sem_wait(&sem_putData); 
	    	this->fifo[i].push_back(tmpPak);
	    sem_post(&sem_putData);
    }
   
  }
  else{
    ptracesfl("AudioInput - putData(): Parameter error NULL pointer passed or size < 0",MT_ERROR, AUDIOINPUT_TRACE);
  }   
}

short* AudioInput::fetchData(int &size, int index) 
{ 
  short* data;
  
  if(this->fifo[index].size() != 0)
  {
  	// Getting reference to head in queue
  	sem_wait(&sem_putData);
    AudioPacket * tmpPak= this->fifo[index].front();
    sem_post(&sem_putData);
        
    data = tmpPak->data;
    size = tmpPak->size;
    
    // Removing head
    sem_wait(&sem_putData); 
    this->fifo[index].pop_front();
    sem_post(&sem_putData);
    
    delete tmpPak;
    
    return data;
  }
  else
  {
    if( data == NULL )
  		ptracesfl("AudioInput - fetchData(): bad paramteter NULL pointer passed",MT_ERROR, AUDIOINPUT_TRACE);
  	else
    	ptracesfl("AudioInput - fetchData(): empty buffer",MT_WARNING, AUDIOINPUT_TRACE);
    
  }

  size = -1;
  return NULL;
}

int AudioInput::getSizeBuffer(int index){
		
	if( this->fifo[index].size() == 0)
		return 0;
	
	sem_wait(&sem_putData);
	AudioPacket * tmpPak= this->fifo[index].front();
	int sizeBuffer= tmpPak->size;
	sem_post(&sem_putData);
	return sizeBuffer;
	
}

TimeInfo* AudioInput::fetchTimeInfo(int index){
 
    if( this->fifo[index].size() == 0)
		return 0;
	
	sem_wait(&sem_putData);
	AudioPacket * tmpPak= this->fifo[index].front();
	TimeInfo* tmpInfo= tmpPak->timeStamp;
	sem_post(&sem_putData);
	return tmpInfo;
	
}

int AudioInput::AddOutput(){
	this->fifo.push_back( std::list<AudioPacket*>() );
	return this->fifo.size() - 1;
}
    
void AudioInput::RemoveOutput( int index ){
	
	if( index > this->fifo.size() ){
		std::vector< std::list<AudioPacket*> >::iterator pos= this->fifo.begin();	
		this->fifo.erase( pos + index );
	}		
}