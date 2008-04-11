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


#include "InternalBuffer.h"

InternalBuffer::InternalBuffer()
{
  sem_init(&sem_putData,0,1);
}

InternalBuffer::~InternalBuffer()
{
}

bool InternalBuffer::putData(void * data, int size)
{
  if (data!=NULL && size > 0)
  {
    
    // Creating new data packet
    DataPacket* tmpPack= new DataPacket;
    tmpPack->data= (unsigned char*)data;//new unsigned char[size];
    tmpPack->size= size;
    
    //memcpy(tmpPack->data,data,tmpPack->size);

	//Adding packet tot the buffer
	sem_wait(&sem_putData);    
    this->fifo.push_back(tmpPack);
    sem_post(&sem_putData);

    return true;
    
  }
  else
  {
    if( data == NULL )
  		ptracesfl("InternalBuffer - fetchData(): bad parameter passed NULL pointer or size < 0",MT_ERROR, INTERNALBUFFER_TRACE);
  	else
    	ptracesfl("InternalBuffer - fetchData(): empty buffer",MT_ERROR, INTERNALBUFFER_TRACE);
  }
  
  return false;
  
}

void* InternalBuffer::fetchData(int &size)
{
  unsigned char* data;

  if(this->fifo.size() != 0)
  {

	// Getting buffer head
	sem_wait(&sem_putData);
    DataPacket * tmpPak= this->fifo.front();
    sem_post(&sem_putData);
    
    //memcpy(data, tmpPak->data, tmpPak->size);
    
    data = (unsigned char*)(tmpPak->data);
    size = tmpPak->size;
    
    // Removing head
    sem_wait(&sem_putData);
    this->fifo.pop_front();
    sem_post(&sem_putData);
    
    delete tmpPak;

    return data;
    
  }
  else
  {
   	if( data == NULL )
  		ptracesfl("InternalBuffer - fetchData(): bad paramteter NULL pointer passed",MT_ERROR, INTERNALBUFFER_TRACE);
  	else
    	ptracesfl("InternalBuffer - fetchData(): empty buffer",MT_ERROR, INTERNALBUFFER_TRACE);
  }
  
  size = -1;
  return NULL;

}

int InternalBuffer::getSizeBuffer()
{
	
   if( this->fifo.size() == 0)
		return 0;
	
	sem_wait(&sem_putData);
	DataPacket * tmpPak= this->fifo.front();
	int sizeBuffer= tmpPak->size;	
	sem_post(&sem_putData);
	return sizeBuffer;
	
}
