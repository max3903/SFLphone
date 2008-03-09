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
	this->theKey = key;
}

MemKey* MemSpace::getMemKey()
{
	return this->theKey;
}

bool MemSpace::putData(void * Data, int size)
{
	strcpy(this->baseAddress,(char*)Data);
	this->theKey->setSize(size);
}

MemData* MemSpace::fetchData()
{
	MemData* newMemData = new MemData();
	newMemData->putData(this->baseAddress,this->theKey->getSize(),this->theKey->getWidth(),
	this->theKey->getHeight());
	return newMemData;
}

MemSpace::MemSpace(MemSpace* space)
{
	this->baseAddress = space->baseAddress;
	this->theKey = space->theKey;
}

MemSpace::MemSpace(MemSpace& space)
{
	
	this->baseAddress = space.baseAddress;
	this->theKey = space.theKey;
}

MemSpace::~MemSpace()
{
}

void MemSpace::readSpace(MemData* data)
{
	//TODO
}

void MemSpace::writeSpace(char * data, int size)
{
	//TODO
}

char* MemSpace::getBaseAddress(){
return this->baseAddress;
}

void MemSpace::setBaseAddress(char* Address){
this->baseAddress = Address;
}
