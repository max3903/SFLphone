/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean-Francois Blanchard-Dionne <jean-francois.blanchard-dionne@polymtl.ca>
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

#include "MemSpace.h"
#include "MemKey.h"
#include "MemData.h"
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 


MemSpace::MemSpace()
{
}

MemSpace::MemSpace(MemKey* key)
{
	this->theKey = new MemKey( key );
}

MemKey* MemSpace::getMemKey()
{
	return new MemKey(this->theKey);
}

bool MemSpace::putData(unsigned char* Data, int size, int width, int height)
{
	int flag= -1;

	do{
		memcpy( &flag, this->baseAddress, sizeof(int) );
		usleep(10);
	}while(flag != 0);
			
	// Putting image size in shared memory
	memcpy(this->baseAddress + sizeof(int) , &width, sizeof(int));
	memcpy(this->baseAddress + (sizeof(int) * 2) , &height, sizeof(int));
	
	// Putting payload in shared memory
	memcpy(this->baseAddress+ (sizeof(int) * 3),Data, size);
	
	// Putting Size of payload in shared memory
	memcpy(this->baseAddress, &size, sizeof(int));
		
	//this->theKey->setSize(size);
	
	return true;
}

MemData* MemSpace::fetchData()
{
	MemData* newMemData = new MemData();
	newMemData->putData(this->baseAddress,this->theKey->getSize());
	return newMemData;
}

MemSpace::MemSpace(MemSpace* space)
{
	this->baseAddress = space->baseAddress;
	this->theKey = new MemKey( space->theKey );
}

MemSpace::MemSpace(MemSpace& space)
{
	
	this->baseAddress = space.baseAddress;
	this->theKey = new MemKey( space.theKey );
}

MemSpace::~MemSpace()
{
	delete theKey;
}


unsigned char* MemSpace::getBaseAddress(){
	return this->baseAddress;
}

void MemSpace::setBaseAddress(unsigned char* Address){
	this->baseAddress = Address;
}

void MemSpace::setKey( MemKey* key ){
	
	if( this->theKey != NULL )
		delete this->theKey;
	
	this->theKey= new MemKey(key);
}

