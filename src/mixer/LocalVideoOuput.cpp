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

#include "LocalVideoOuput.h"

LocalVideoOuput::LocalVideoOuput()
{
  // create the manager (or get the instance)
  this->manager = MemManager::getInstance();
  
  vector<MemKey*> tmpKeys= this->manager->getAvailSpaces();
  
  vector<MemKey*>::iterator start= tmpKeys.begin();
  vector<MemKey*>::iterator end= tmpKeys.end();
  std:string searchString= "remote";
  this->key= NULL;
  
  // Search for local Memkey
  while( start != end ){
  	if( (*start)->getDescription() == searchString  ){
  		key= (*start);
  		break;
  	}
  	start++;
  }
  
  if( start == end )
  	ptracesfl("LocalVideoOuput: Could not find local memory space.", MT_FATAL, LOCALVIDEOOUTPUT_TRACE);
  
}

LocalVideoOuput::~LocalVideoOuput()
{
  
}

void LocalVideoOuput::putData(unsigned char * data, int size)
{   
	
	int width= 0, height= 0;
	
	memcpy(&width, data, sizeof(int));
	memcpy(&height, data + sizeof(int) , sizeof(int));
	
	if( this->key != NULL )
		if( !this->manager->putData(this->key, data + ( 2* sizeof(int) ), size - ( 2* sizeof(int) ), width, height))
  			ptracesfl("LocalVideoOutput - putData(): Can't put data", MT_ERROR, LOCALVIDEOOUTPUT_TRACE);
}

// This method is not used in this class
short* LocalVideoOuput::fetchData(int &size){
  ptracesfl("LocalVideoOuput: Fetch Data shoud not be called on local output", MT_FATAL, LOCALVIDEOOUTPUT_TRACE);
  return 0;
}

const char* LocalVideoOuput::getType() const{
	return "Local Video Output Buffer";
}