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

#include "Command.h"

// Static variables initialisation
sem_t Command::AccessSem;
sem_t Command::AvailSem;
bool Command::init= false;
VideoDevice* Command::videoDevice= NULL;

Command::Command(){
	if( !init ){	// This should be executed only once
		sem_init(&Command::AccessSem, 0, 1);
		sem_init(&Command::AvailSem, 0, 1);
		Command::init= true;
	}
}

Command::~Command(){}

bool Command::increase(){
	return false;
}

bool Command::decrease(){
	return false;
}

bool Command::setTo(__u16 value){
	return false;
}

bool Command::reset(){
	return false;
} 
  
void Command::getVideoDeviceAccess(){
  	sem_wait(&Command::AvailSem);	// Check if device is available
  	sem_post(&Command::AvailSem);
  	sem_wait(&Command::AccessSem);	// Get device access 	
 }
  
void Command::releaseVideoDevice(){
	sem_post(&Command::AccessSem);	// Release device
}
  
void Command::ChangingDevice(){
  	sem_wait(&Command::AvailSem);	// Make device not available
  	sem_wait(&Command::AccessSem);	// Wait to get the device
	sem_post(&Command::AccessSem);  
}

void Command::DeviceChanged(){
	sem_post(&Command::AvailSem);	// Make the device available
}
