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

#include "MemManager.h"

MemManager* MemManager::instance= 0;

MemManager* MemManager::getInstance()
{
	//if no instance made create one,
	//ref. singleton pattern
	if (instance == 0)
	instance = new MemManager;

	return instance;
}

MemManager::MemManager()
{
}

MemManager::~MemManager()
{
	delete instance;
}

const MemKey* MemManager::initSpace(key_t key, int size,char * description)
{
	
	MemKey *newKey = new MemKey(size,description,key);
	MemSpace *newSpace = new MemSpace(newKey);
	shmget(newKey->getKey(), newKey->getSize(), IPC_CREAT | 0666);
	spaces.push_back(newSpace);
	return newKey;
}

const MemKey* MemManager::initSpace(MemKey* key)
{
	MemSpace *newSpace; 
	
	newSpace = new MemSpace(key);
	
	shmget(key->getKey(), key->getSize(), IPC_CREAT | 0666);
	spaces.push_back(newSpace);
	
	return key;
}

const MemKey* MemManager::initSpace(int size,char * description)
{
	MemKey *newKey;
	MemSpace *newSpace;
	key_t key = genKey();
	newKey = new MemKey(size,description,key);
	newSpace = new MemSpace(newKey);
	shmget(newKey->getKey(), newKey->getSize(), IPC_CREAT | 0666);
	spaces.push_back(newSpace);
	return newKey;
}

bool MemManager::setDefaultSpace(MemKey* key)
{
return false;
}

void MemManager::nextSpace()
{

}

void MemManager::previousSpace()
{

}

MemData* MemManager::fetchData( )
{
}

MemData* MemManager::fetchData(int key)
{
}

MemData* MemManager::fetchData(MemKey* key)
{
}

bool MemManager::putData(void * Data, int size)
{
	return false;
}

bool MemManager::putData(int key, void * Data, int size)
{
	return false;
}

bool MemManager::putData(MemKey* key, void * Data, int size)
{
	return false;
}

vector<MemKey*> MemManager::getAvailSpaces() const 
{
	//TODO vector memkey or memspace ???
	vector<MemKey*> tmp;
	return tmp;

}


int MemManager::genKey()
{
	return rand();
	
}
