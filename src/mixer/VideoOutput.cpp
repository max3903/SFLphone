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

#include "VideoOutput.h"

VideoOutput::VideoOutput()
{
  sem_init(&sem_putData,0,1);
}

VideoOutput::~VideoOutput()
{
}

int VideoOutput::fetchData(unsigned char* data, int &width, int &height)
{
	
 if(data ==NULL && this->fifo.size() != 0)
  {
  	// Getting reference to head in queue
  	sem_wait(&sem_putData);
    VideoOutputPacket * tmpPak= this->fifo.front();
    sem_post(&sem_putData);
        
    data= tmpPak->data;
    width= tmpPak->width;
    height= tmpPak->height;
    int sizeBuffer= tmpPak->size;
    
    // Removing head
    sem_wait(&sem_putData); 
    this->fifo.pop_front();
    sem_post(&sem_putData);
        
    return sizeBuffer;
  }
  else
  {
    if( data != NULL )
  		ptracesfl("VideoOutput - fetchData(): bad parameter, data pointer must be NULL",MT_ERROR, VIDEOOUTPUT_TRACE);
  	else
    	ptracesfl("VideoOutput - fetchData(): empty buffer",MT_WARNING, VIDEOOUTPUT_TRACE);
  }
  
}

void VideoOutput::putData(unsigned char * data, int size)
{ 
   if (data!=NULL && size > 0)
  {
    
    // Creating an audio data packet 
    VideoOutputPacket * tmpPak= new VideoOutputPacket;
    
    tmpPak->data= new unsigned char[size - (sizeof(int) * 2)];
    tmpPak->size= size - (sizeof(int) * 2);
    
    memcpy(&tmpPak->width, data,  sizeof(int));
    memcpy(&tmpPak->height, data + sizeof(int), sizeof(int));
    memcpy(tmpPak->data, data + (sizeof(int) * 2), size - (sizeof(int) * 2));

	// Adding data to the fifo
	sem_wait(&sem_putData); 
    	this->fifo.push_back(tmpPak);
    sem_post(&sem_putData);
   
  }
  else{
    ptracesfl("VideoOutput - putData(): Parameter error NULL pointer passed or size < 0",MT_ERROR, VIDEOOUTPUT_TRACE);
  }   
}

const char* VideoOutput::getType() const{
	return "Video Output Buffer";
}

// DEPRECIATED !!!!!
int VideoOutput::fetchData(short *data)
{ 
  return 0; 
}
// DEPRECIATED !!!!!
void VideoOutput::putData(short * data, int size)
{ 
  
}