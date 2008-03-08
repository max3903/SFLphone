/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
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

const MemKey* MemManager::initSpace(key_t key, int size,char * description,int width,int height)
{
	int shmid;
	MemKey *newKey = new MemKey(size,description,key,width,height);
	MemSpace *newSpace = new MemSpace(newKey);
	
	
	if ( (shmid = shmget(newKey->getKey(), newKey->getSize(), IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
        exit(1);
	}
	
	//attach shared memory to baseAddress
    newSpace->setBaseAddress((char *)shmat(shmid, NULL, 0));
    
    if ( newSpace->getBaseAddress() == (char *) -1) {
        perror("shmat");
        exit(1);
    } 
	
	
	spaces.push_back(newSpace);
	return newKey;
}

const MemKey* MemManager::initSpace(MemKey* key)
{
	int shmid;
	MemSpace *newSpace; 
	newSpace = new MemSpace(key);
	
	//create shared memory space
	if ( (shmid = shmget(key->getKey(), key->getSize(), IPC_CREAT | 0666)) < 0)
	{
        perror("shmget");
        exit(1);
    }
    
    //attach shared memory to baseAddress
    newSpace->setBaseAddress((char *)shmat(shmid, NULL, 0));
    
    if ( newSpace->getBaseAddress() == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    
	spaces.push_back(newSpace);
	
	return key;
}

const MemKey* MemManager::initSpace(int size,char * description,int width, int height)
{
	MemKey *newKey;
	int shmid;
	MemSpace *newSpace;
	key_t key = genKey();
	
	newKey = new MemKey(size,description,key,width,height);
	newSpace = new MemSpace(newKey);
	
	//create shared memory space
	if ( (shmid = shmget(newKey->getKey(), newKey->getSize(), IPC_CREAT | 0666)) < 0)
	{
        perror("shmget");
        exit(1);
    }
    
    //attach shared memory to baseAddress
    newSpace->setBaseAddress((char *)shmat(shmid, NULL, 0));
    
    if ( newSpace->getBaseAddress() == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    
    
	spaces.push_back(newSpace);
	
	return newKey;
}

bool MemManager::setDefaultSpace(MemKey* key)
{

	for( iter = spaces.begin(); iter != spaces.end() ;iter++);
	{
		if ((*iter)->getMemKey()->getKey() == key->getKey()){
		defaultIndex = iter;
		return true;
		}
	
	}
	
return false;
}

void MemManager::nextSpace()
{
	if(defaultIndex == spaces.end()){
	defaultIndex == spaces.begin();
	}
	else
	defaultIndex++;
	
}

void MemManager::previousSpace()
{
	if(defaultIndex == spaces.begin()){
	defaultIndex == spaces.end();
	}
	else
	defaultIndex--;
	
}

MemData* MemManager::fetchData()
{

	MemData* newMemData = new MemData();
	
	newMemData->putData((*defaultIndex)->getBaseAddress(),
	(*defaultIndex)->getMemKey()->getSize(),(*defaultIndex)->getMemKey()->getWidth(),
	(*defaultIndex)->getMemKey()->getHeight());
	
	
	return newMemData;
}

MemData* MemManager::fetchData(key_t key)
{
	vector<MemSpace*>::iterator i;
	MemData* newMemData = new MemData();
	
	for( iter = spaces.begin(); iter != spaces.end() ;iter++);
	{
		if ((*iter)->getMemKey()->getKey() == key){
		i = iter;
	
		}
	
	}
	
	newMemData->putData((*i)->getBaseAddress(),
	(*i)->getMemKey()->getSize(),(*i)->getMemKey()->getWidth(),
	(*i)->getMemKey()->getHeight());
	
	
	return newMemData;
	
}

MemData* MemManager::fetchData(MemKey* key)
{
	vector<MemSpace*>::iterator i;
	MemData* newMemData = new MemData();
	
	//find memspace 
	for( iter = spaces.begin(); iter != spaces.end() ;iter++);
	{
		if ((*iter)->getMemKey() == key){
		i = iter;
		}
	
	}
	
	newMemData->putData((*i)->getBaseAddress(),
	(*i)->getMemKey()->getSize(),(*i)->getMemKey()->getWidth(),
	(*i)->getMemKey()->getHeight());
	
	
	return newMemData;
	
}

bool MemManager::putData(char * Data, int size)
{
	//TODO CHANGE width and height ???
	//TODO return false???
	(*defaultIndex)->setBaseAddress(Data);
	(*defaultIndex)->getMemKey()->setSize(size);
	
	return true;
}

bool MemManager::putData(int key, char * Data, int size)
{
	vector<MemSpace*>::iterator i;
	
	for( iter = spaces.begin(); iter != spaces.end() ;iter++);
	{
		if ((*iter)->getMemKey()->getKey() == key){
		i = iter;
	
		}
	}
	
	(*i)->setBaseAddress(Data);
	(*i)->getMemKey()->setSize(size);
	return true;
}

bool MemManager::putData(MemKey* key, char * Data, int size)
{
	vector<MemSpace*>::iterator i;
	
	//find memspace 
	for( iter = spaces.begin(); iter != spaces.end() ;iter++);
	{
		if ((*iter)->getMemKey() == key){
		i = iter;
		}
	
	}
	
	(*i)->setBaseAddress(Data);
	(*i)->getMemKey()->setSize(size);
	
	return true;
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
