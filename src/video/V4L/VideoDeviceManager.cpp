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

#include "VideoDeviceManager.h"

VideoDeviceManager* VideoDeviceManager::instance= 0;

VideoDeviceManager* VideoDeviceManager::getInstance(){

	if( VideoDeviceManager::instance == 0 )
		VideoDeviceManager::instance= new VideoDeviceManager();
		
    return instance; 
}

VideoDeviceManager::VideoDeviceManager(){
}

VideoDeviceManager::~VideoDeviceManager(){
}

bool VideoDeviceManager::changeDevice(char* srcName){

	VideoDevice* tmpDevice= NULL;

	Command::ChangingDevice();
	
	try{
    	tmpDevice= new VideoDevice( srcName );
    }catch(...){
    	Command::DeviceChanged();
    	return false;
    }
    
    if(tmpDevice == NULL){
    	Command::DeviceChanged();
    	return false;
    }
    
    Command::videoDevice->closeDevice();
    delete Command::videoDevice;
    
    Command::videoDevice= tmpDevice;
        
    Command::DeviceChanged();
    
    return true;
    
}

bool VideoDeviceManager::createDevice(char* srcName){
    
    VideoDevice* tmpDevice= NULL;
    
    try{
    	tmpDevice= new VideoDevice( srcName );
    }catch(...){
    	return false;
    }
    
    if(tmpDevice == NULL)
    	return false;
    	
    Command::videoDevice= tmpDevice;
    
    return true;
}

Command* VideoDeviceManager::getCommand(TCommand ref){
    
    Command* tmp= NULL;
    
    switch(ref){
    	case CONTRAST:
    		tmp= new Contrast();
    		break;
    	case BRIGHTNESS:
    		tmp= new Brightness();
    		break;
		case COLOR:
			tmp = new Colour();
			break;
		case CAPTURE:
			tmp= new Capture();
			break;
		case RESOLUTION:
			tmp= new Resolution();
			break;	
    }
    
    return tmp;
    
}



