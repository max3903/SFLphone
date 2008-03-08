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
	
	//TODO get index from memmanager
	//TODO put size
	//TODO put description
	
}

MemKey::MemKey(char* serializedData)
{
	
	//TODO ????
}

MemKey::MemKey(int size, char* description,int width, int height)
{
	this->size = size;
	strcpy(this->description,description);
	this->height = height;
	this->width = width;
	//TODO get index from memmanager
	//TODO generate key
}


MemKey::MemKey(int size, char* description,key_t key,int width, int height)
{
	this->size = size;
	strcpy(this->description,description);
	this->key = key;
	this->height = height;
	this->width = width;

}


MemKey::MemKey(MemKey* key)
{	
	
	this->size = key->size;
	strcpy(this->description,key->description);
	this->index = key->index;
	this->size = key->size;
	
}


MemKey::MemKey(MemKey& key)
{
	
	this->size = key.size;
	strcpy(this->description,key.description);
	this->index = key.index;
	this->size = key.size;
}


MemKey::~MemKey()
{
}

int MemKey::getKey()
{
	return this->key;
}

void MemKey::setKey(key_t key){

this->key = key;

}

const char * MemKey::getDescription() const 
{
	return this->description ;
}

int MemKey::getIndex()
{
	return this->index;
}

void MemKey::setIndex(int index)
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

char * MemKey::serialize()
{
	//TODO return char * with all params
	return 0;
}

int MemKey::getHeight()
{

return this->height;

}

void MemKey::setHeight(int height)
{
this->height = height;

}

int MemKey::getWidth()
{
return this->width;
}

void MemKey::setWidth(int width)
{
	
	this->width = width;
	
}
