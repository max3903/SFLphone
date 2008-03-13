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

#include "MemKey.h"

MemKey::MemKey()
{
	this->key = this->genKey();
	//TODO get index from memmanager
	//TODO put DEFAULT size
	//TODO put DEFAULT width
	//TODO put DEFAULT height
	this->description = serialize();
}

MemKey::MemKey(char* serializedData)
{
	
	//TODO read serializedData and create MemKey with parameters
	//"%i %i %i %i",size,width,height,key
}

MemKey::MemKey(int size)
{
	this->size = size;

	this->key = this->genKey();
	//TODO get index from memmanager
	this->description = serialize();
	
}


MemKey::MemKey(int size,key_t key)
{
	this->size = size;
	this->key = key;
	this->description = serialize();
}


MemKey::MemKey(MemKey* key)
{	
	this->index = key->index;
	this->size = key->size;
	this->description = serialize();
}


MemKey::MemKey(MemKey& key)
{
	
	this->size = key.size;
	this->index = key.index;
	this->size = key.size;
	this->description = serialize();
}


MemKey::~MemKey()
{
}

int MemKey::getKey()
{
	return this->key;
}

void MemKey::setKey(key_t key)
{
	this->key = key;
}

const char * MemKey::getDescription() const 
{
	return this->description ;
}

vectMemSpaceIterator MemKey::getIndex()
{
	return this->index;
}

void MemKey::setIndex(vectMemSpaceIterator index)
{ 
	this->index = index;
}

int MemKey::getSize()
{
	return this->size;
}

void MemKey::setSize(int size)
{
	this->size = size;
	
}

char* MemKey::serialize()
{
	char* tmp;
	sprintf(tmp,"%i %i",this->size,this->key);
	return tmp;
}

int MemKey::genKey()
{
	return rand();	
}
