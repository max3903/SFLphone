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
sem_t MemManager::Available;
sem_t MemManager::Active;

MemManager* MemManager::getInstance()
{
	//if no instance made create one,
	//ref. singleton pattern
	if (instance == 0){
		MemManager::instance = new MemManager;
		sem_init(&MemManager::Active, 0, 1);
		sem_init(&MemManager::Available, 0, 1);
	}

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
	ptracesfl("\tInitializing Shared Memory space ...", MT_INFO, MEMMANAGER_TRACE, false );
	newKey->setShmid(shmget(key, size, IPC_CREAT | 0666));
	
	if ( newKey->getShmid() < 0)
	{
		ptracesfl("\tNO", MT_NONE, MEMMANAGER_TRACE);
		ptracesfl("Error cannot get Shared Memory space, Please try again in a few seconds", MT_FATAL, MEMMANAGER_TRACE);
	}else
		ptracesfl("\tOK", MT_NONE, MEMMANAGER_TRACE);
	
	//Attach shared memory to baseAddress shmat returns pointer to the
	// shared memory segment
	ptracesfl("\tAttaching shared memory segment ...", MT_INFO, MEMMANAGER_TRACE, false);
    newSpace->setBaseAddress((unsigned char *)shmat(newKey->getShmid(), (void *) 0, 0));
    
    if ( newSpace->getBaseAddress() == (unsigned char *) -1) {
    	ptracesfl("\tNO", MT_NONE, MEMMANAGER_TRACE);
        ptracesfl("Error cannot attach Shared Memory space", MT_FATAL, MEMMANAGER_TRACE);
    }else
    	ptracesfl("\tOK", MT_NONE, MEMMANAGER_TRACE);
    
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
	ptracesfl("\tInitializing Shared Memory space ...", MT_INFO, MEMMANAGER_TRACE, false );
	key->setShmid(shmget(key->getKey(), key->getSize(), IPC_CREAT | 0666));
	
	if ( key->getShmid() < 0)
	{
		ptracesfl("\tNO", MT_NONE, MEMMANAGER_TRACE);
		ptracesfl("Error cannot get Shared Memory space, Please try again in a few seconds", MT_FATAL, MEMMANAGER_TRACE);
	}else
		ptracesfl("\tOK", MT_NONE, MEMMANAGER_TRACE);
    
    //attach shared memory to baseAddress
    ptracesfl("\tAttaching shared memory segment ...", MT_INFO, MEMMANAGER_TRACE, false);
    newSpace->setBaseAddress((unsigned char *)shmat(key->getShmid(), 0, 0));
    
    if ( newSpace->getBaseAddress() == (unsigned char *) -1) {
    	ptracesfl("\tNO", MT_NONE, MEMMANAGER_TRACE);
        ptracesfl("Error cannot attach Shared Memory space", MT_FATAL, MEMMANAGER_TRACE);
    }else
    	ptracesfl("\tOK", MT_NONE, MEMMANAGER_TRACE);
    
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


MemKey* MemManager::initSpace(int size,  const char* desc)
{
	MemKey *newKey;
	MemSpace *newSpace;
	key_t key = genKey();
	vectMemSpaceIterator MemSpaceLocation;
	
	newKey = new MemKey(size,key, desc);
	
	//create shared memory space
	ptracesfl("\tInitializing Shared Memory space ...", MT_INFO, MEMMANAGER_TRACE, false );
	newKey->setShmid(shmget(key, size, IPC_CREAT | 0666));
	
	if ( newKey->getShmid() < 0)
	{
		ptracesfl("\tNO", MT_NONE, MEMMANAGER_TRACE);
		ptracesfl("Error cannot get Shared Memory space, Please try again in a few seconds", MT_FATAL, MEMMANAGER_TRACE);
	}else
		ptracesfl("\tOK", MT_NONE, MEMMANAGER_TRACE);
    
	newSpace = new MemSpace(newKey);
	
    //attach shared memory to baseAddress
    ptracesfl("\tAttaching shared memory segment ...", MT_INFO, MEMMANAGER_TRACE, false);
    newSpace->setBaseAddress((unsigned char *)shmat(newKey->getShmid(), 0, 0));
    
    if ( newSpace->getBaseAddress() == (unsigned char *) -1) {
    	ptracesfl("\tNO", MT_NONE, MEMMANAGER_TRACE);
        ptracesfl("Error cannot attach Shared Memory space", MT_FATAL, MEMMANAGER_TRACE);
    }else
    	ptracesfl("\tOK", MT_NONE, MEMMANAGER_TRACE);
    
    //Adds the index to the key
	MemSpaceLocation = spaces.end() -1;
	newKey->setIndex(MemSpaceLocation);
	
	newSpace->setKey(newKey);
	
	//add the newly created space to the vector 
	spaces.push_back(newSpace);
	// if it's the first memspace created, we assign the defaultIndex to it's position
	if(spaces.size() == 1)
		defaultIndex = spaces.end() -1;
	
	return newKey;
}

bool MemManager::deleteSpace(MemKey* key)
{	
	vectMemSpaceIterator result= this->search(key);
	
	if( result == this->spaces.end() )
		return false;
	
	this->getManagerControl();
	
	ptracesfl("Detaching shared memory segment", MT_ERROR, MEMMANAGER_TRACE);
	int ret = shmdt((*(key->getIndex()))->getBaseAddress());
	
	if(ret == -1){
		ptracesfl("\tNO", MT_NONE, MEMMANAGER_TRACE);
   		ptracesfl("Cannot detach shared memory segment", MT_ERROR, MEMMANAGER_TRACE);
   		return false;
	}else
  		 ptracesfl("\tOK", MT_NONE, MEMMANAGER_TRACE);
  				
	this->spaces.erase(result);
	
	this->releaseManagerControl();
			
	return true;
}

bool MemManager::CleanSpaces(){

	vector<MemSpace*>::iterator iter;
	
	this->getManagerControl();

	//for each mespace detach memory
	ptracesfl("Cleaning all shared memory space ...", MT_INFO, MEMMANAGER_TRACE);
	for( iter = spaces.begin(); iter != spaces.end() ;iter++)
	{
		ptracesfl("Dettaching shared memory space ... ", MT_INFO, MEMMANAGER_TRACE, false);
		int ret = shmdt((*iter)->getBaseAddress());
		
		if(ret == -1){
			ptracesfl("\tNO", MT_NONE, MEMMANAGER_TRACE);
   			ptracesfl("Cannot detach shared memory segment", MT_ERROR, MEMMANAGER_TRACE);
   			return false;
		}else
  		 	ptracesfl("\tOK", MT_NONE, MEMMANAGER_TRACE);
			
	}
	
	//clean vector
	spaces.clear();
	
	this->releaseManagerControl();
	
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
	if( this->getSpaceConstol() )
		return false;
		
	vector<MemSpace*>::iterator iter;

	//find the memspace containing the key
	for( iter = spaces.begin(); iter != spaces.end() ;iter++){
		if ((*iter)->getMemKey()->getKey() == key)
		{
			// returns a MemData
			this->releaseSpaceConstol();
			return (*iter)->fetchData();
		}
	}
		
	//if no key found return NULL
	this->releaseSpaceConstol();
	return NULL; 
}

MemData* MemManager::fetchData(MemKey* key)
{
	return this->fetchData(key->getKey());
	//return (*(key->getIndex()))->fetchData();
}

bool MemManager::putData(unsigned char * Data, int size, int width, int height)
{
	if(this->getSpaceConstol() )
		return false;
	
	(*defaultIndex)->putData(Data,size, width, height);
	
	this->releaseSpaceConstol();
	
	return true;
}

bool MemManager::putData(key_t key, unsigned char * Data, int size, int width, int height)
{
	
	//if( this->getSpaceConstol() )
		//return false;
		
	vector<MemSpace*>::iterator iter;

	for( iter = spaces.begin(); iter != spaces.end() ;iter++){
		if ((*iter)->getMemKey()->getKey() == key)
		{
			(*iter)->putData(Data,size, width, height);
			this->releaseSpaceConstol();
			return true;
		}
	}
	
	//this->releaseSpaceConstol();
	return false;
	
}

bool MemManager::putData(MemKey* key, unsigned char * Data, int size, int width, int height)
{
	return this->putData(key->getKey(), Data, size, width, height);
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
		ptracesfl("KEY INVALID",MT_FATAL,1,true);

	return tmp;	
		
}

vectMemSpaceIterator MemManager::search(MemKey* refKey){
	
	vectMemSpaceIterator start= this->spaces.begin();
	vectMemSpaceIterator end= this->spaces.end();
	
	MemKey* tmpKey;
	while( start != end ){
		tmpKey= ((MemSpace*)(*start))->getMemKey();
		if( tmpKey->getKey() == refKey->getKey() )
			return start;
		start++;
	}
	
	ptracesfl("Cannot find corresponding Shared memory segment", MT_ERROR, MEMMANAGER_TRACE);
	return end;
	
}


bool MemManager::getManagerControl(){
	sem_wait(&MemManager::Available);
	sem_wait(&MemManager::Active);
	sem_post(&MemManager::Active);
	return true;
}

bool MemManager::releaseManagerControl(){
	sem_post(&MemManager::Available);
	return true;
}

bool MemManager::getSpaceConstol(){
	if( sem_trywait(&MemManager::Available) == -1 ){
		sem_post(&MemManager::Available);
		if( sem_trywait(&MemManager::Active) == -1){
			return true;
		}		
	}
	
	return false;

}

bool MemManager::releaseSpaceConstol(){
	sem_post(&MemManager::Active);
	return true;
}
