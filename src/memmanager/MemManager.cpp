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
// References http://www.ibm.com/developerworks/aix/library/au-ipc/index.html
#include "MemManager.h"

MemManager* MemManager::instance= 0;

MemManager* MemManager::getInstance()
{
	//if no instance made create one,
	//ref. singleton pattern
	if (instance == 0)
	MemManager::instance = new MemManager;

	return instance;
}

MemManager::MemManager()
{
	
}

MemManager::~MemManager()
{
	CleanSpaces();

}

MemKey* MemManager::initSpace(key_t key,int size)
{
	//THE KEY GIVEN MUST HAVE BEEN GENERATED WITH ftok function
	
	MemKey *newKey = new MemKey(size,key);
	MemSpace *newSpace = new MemSpace(newKey);
	vectMemSpaceIterator MemSpaceLocation;
	
	
	//Get the IPC-specific identifier associated with the IPC key using shmget(2)
	// for shared memory
	newKey->setShmid(shmget(key, size, IPC_CREAT | 0666));
	
	if ( newKey->getShmid() < 0)
	{
		perror("shmget");
        exit(1);
	}
	
	//Attach shared memory to baseAddress shmat returns pointer to the
	// shared memory segment
    newSpace->setBaseAddress((unsigned char *)shmat(newKey->getShmid(), (void *) 0, 0));
    
    if ( newSpace->getBaseAddress() == (unsigned char *) -1) {
        perror("shmat");
        exit(1);
    } 
    
	//add the newly created space to the vector 
	spaces.push_back(newSpace);
	// if it's the first memspace created, we assign the defaultIndex to it's position
	if(spaces.size() == 1)
		defaultIndex = spaces.end() -1;
		
	//Adds the index to the key
	MemSpaceLocation = spaces.end() -1;
	newKey->setIndex(MemSpaceLocation);
	
	return newKey;
}

MemKey* MemManager::initSpace(MemKey* key)
{
	MemSpace *newSpace; 
	newSpace = new MemSpace(key);
	vectMemSpaceIterator MemSpaceLocation;
	
	
	//create shared memory space
	key->setShmid(shmget(key->getKey(), key->getSize(), IPC_CREAT | 0666));
	
	if ( key->getShmid() < 0)
	{
        perror("shmget");
        exit(1);
    }
    
    //attach shared memory to baseAddress
    newSpace->setBaseAddress((unsigned char *)shmat(key->getShmid(), 0, 0));
    
    if ( newSpace->getBaseAddress() == (unsigned char *) -1) {
        perror("shmat");
        exit(1);
    }
    
    //add the newly created space to the vector 
	spaces.push_back(newSpace);
	// if it's the first memspace created, we assign the defaultIndex to it's position
	if(spaces.size() == 1)
		defaultIndex = spaces.end() -1;
		
	//Adds the index to the key
	MemSpaceLocation = spaces.end() -1;
	key->setIndex(MemSpaceLocation);
	
	return key;
}


MemKey* MemManager::initSpace(int size)
{
	MemKey *newKey;
	MemSpace *newSpace;
	key_t key = genKey();
	vectMemSpaceIterator MemSpaceLocation;
	
	newKey = new MemKey(size,key);
	newSpace = new MemSpace(newKey);
	
	//create shared memory space
	newKey->setShmid(shmget(key, size, IPC_CREAT | 0666));
	
	if ( newKey->getShmid() < 0)
	{
        perror("shmget");
        exit(1);
    }
    

    //attach shared memory to baseAddress
    newSpace->setBaseAddress((unsigned char *)shmat(newKey->getShmid(), 0, 0));
    
    if ( newSpace->getBaseAddress() == (unsigned char *) -1) {
        perror("shmat");
        exit(1);
    }
    
    //add the newly created space to the vector 
	spaces.push_back(newSpace);
	// if it's the first memspace created, we assign the defaultIndex to it's position
	if(spaces.size() == 1)
		defaultIndex = spaces.end() -1;
		
	//Adds the index to the key
	MemSpaceLocation = spaces.end() -1;
	newKey->setIndex(MemSpaceLocation);
	
	ptracesfl("MemSpace Created : ",MT_INFO,1,false);
	ptracesfl(newKey->getDescription().c_str(),MT_NONE,1,true);
	
	return newKey;
}

bool MemManager::deleteSpace(MemKey* key)
{

	int i;
	
	i = shmdt((*(key->getIndex()))->getBaseAddress());
	
	if(i == -1) 
   		perror("shmop: shmdt failed");
    	 else 
  		 fprintf(stderr, "shmop: shmdt returned %d\n", i);
  				
			//delete memspace
			delete (*(key->getIndex()));
			
			return true;
}

bool MemManager::CleanSpaces(){

vector<MemSpace*>::iterator iter;
int i;

	//for each mes	ptracesfl(newKey->,MT_NONE,1,false);pace detach memory
	for( iter = spaces.begin(); iter != spaces.end() ;iter++)
	{
		i = shmdt((*iter)->getBaseAddress());
		
		if(i == -1) 
		{
   		perror("shmop: shmdt failed");
   		return false;
		}
			
	}
	
	//clean vector
	spaces.clear();
	
		return true;

}

bool MemManager::setDefaultSpace(MemKey* key)
{

		defaultIndex = key->getIndex();
		return true;
	
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
	// returns a MemData from current default index
	return (*defaultIndex)->fetchData();
}

MemData* MemManager::fetchData(key_t key)
{
	vector<MemSpace*>::iterator iter;

	//find the memspace containing the key
	for( iter = spaces.begin(); iter != spaces.end() ;iter++)
		if ((*iter)->getMemKey()->getKey() == key)
		{
			// returns a MemData
			return (*iter)->fetchData();
		}
		
		//if no key found return NULL
		return NULL; 
}

MemData* MemManager::fetchData(MemKey* key)
{
	return (*(key->getIndex()))->fetchData();
}

bool MemManager::putData(unsigned char * Data, int size)
{
	(*defaultIndex)->putData(Data,size);
	return true;
}

bool MemManager::putData(key_t key, unsigned char * Data, int size)
{
	vector<MemSpace*>::iterator iter;

	
	for( iter = spaces.begin(); iter != spaces.end() ;iter++)
		if ((*iter)->getMemKey()->getKey() == key)
		{
			(*iter)->putData(Data,size);
			return true;
		}
		
		return false;
}

bool MemManager::putData(MemKey* key, unsigned char * Data, int size)
{
	(*(key->getIndex()))->putData(Data,size);

			return true;
	
}

vector<MemKey*> MemManager::getAvailSpaces() 
{
	vector<MemKey*> MemKeys;
	vector<MemSpace*>::iterator iter;
	MemKey *tmp;

	for( iter = spaces.begin(); iter != spaces.end() ;iter++){
		tmp = (*iter)->getMemKey();
		MemKeys.push_back(tmp);
	}
		
	return MemKeys;
}

key_t MemManager::genKey()
{
	key_t tmp;
	tmp =  ftok("/tmp",rand());
	if(tmp == (key_t) -1)
		ptracesfl("ERROR : KEY INVALID",MT_FATAL,1,true);

	return tmp;	
		
}
