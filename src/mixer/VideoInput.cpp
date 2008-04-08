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

#include "VideoInput.h"

VideoInput::VideoInput(){
  sem_init(&sem_putData,0,1); 
}

VideoInput::~VideoInput(){
}

void VideoInput::putData(char * data, int size, int timeStamp, int w, int h){
	 
  if (data!=NULL && size>0 && w!= 0 && h!=0){
    
    // Creating an video data packet 
    VideoPacket * tmpPak= new VideoPacket;
    tmpPak->data= new unsigned char[size + (2* sizeof(int))];
    tmpPak->size= size + (2* sizeof(int));
    tmpPak->timeStamp= new TimeInfo(timeStamp); 
    
    // Putting data in the packet in the format described in fetchData
    memcpy(tmpPak->data, &w, sizeof(int));
    memcpy(tmpPak->data + sizeof(int), &h, sizeof(int));
    memcpy(tmpPak->data + (2*sizeof(int)), data, tmpPak->size);

	// Adding data to the fifo
	sem_wait(&sem_putData); 
    	this->fifo.push_back(tmpPak);
    sem_post(&sem_putData);
   
  }else{
    ptracesfl("VideoInput - putData(): Parameter error NULL pointer passed or size < 0",MT_ERROR, VIDEOINPUT_TRACE);
  }
       
}

unsigned char* VideoInput::fetchData(int &size){
	
  unsigned char* data= NULL;
   
  if(this->fifo.size() != 0)
  {
  	// Getting reference to head in queue
  	sem_wait(&sem_putData);
    VideoPacket * tmpPak= this->fifo.front();
    sem_post(&sem_putData);
       
    data= tmpPak->data;         
    size= tmpPak->size;
    
    // Removing head
    sem_wait(&sem_putData); 
    this->fifo.pop_front();
    sem_post(&sem_putData);
        
    return data;
  }
  else
  {
    if( data == NULL )
  		ptracesfl("VideoInput - fetchData(): bad paramteter NULL pointer passed",MT_ERROR, VIDEOINPUT_TRACE);
  	else
    	ptracesfl("VideoInput - fetchData(): empty buffer",MT_WARNING, VIDEOINPUT_TRACE);
  }

  size= -1;
  return NULL;
  
}

int VideoInput::getSizeBuffer(){
	
 	if( this->fifo.size() == 0)
		return 0;
	
	sem_wait(&sem_putData);
	VideoPacket * tmpPak= this->fifo.front();
	int sizeBuffer= tmpPak->size;
	sem_post(&sem_putData);
	
	return sizeBuffer;
	
}

TimeInfo* VideoInput::fetchTimeInfo(){ 
	
	if( this->fifo.size() == 0)
		return 0;
	
	sem_wait(&sem_putData);
	VideoPacket * tmpPak= this->fifo.front();
	TimeInfo* tmpInfo= tmpPak->timeStamp;
	sem_post(&sem_putData);
	
	return tmpInfo;
	
}
