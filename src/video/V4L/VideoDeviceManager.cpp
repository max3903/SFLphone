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
		VideoDeviceManager::instance= new VideoDeviceManager;
		
    return instance; 
}

VideoDeviceManager::VideoDeviceManager(){
	createCommand= false;
}

VideoDeviceManager::~VideoDeviceManager(){
}

bool VideoDeviceManager::changeDevice(const char* srcName){

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
    
    if(Command::videoDevice != NULL)
    {
    	Command::videoDevice->closeDevice();
    	delete Command::videoDevice;
    }
    
    Command::videoDevice= tmpDevice;
        
    Command::DeviceChanged();
    
    return true;
    
}

bool VideoDeviceManager::createDevice(const char* srcName){
    
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
    
    createCommand= true;
    
    return tmp;
    
}

vector<string> VideoDeviceManager::enumVideoDevices(){

  vector<string> ret;
  LibHalContext *hal_context= NULL;
  DBusConnection *dbus= dbus_bus_get( DBUS_BUS_SYSTEM, NULL );
  
  // Initialisation of the dbus connection
  if( dbus ){
  	hal_context= libhal_ctx_new();
  	if(hal_context)
  		libhal_ctx_set_dbus_connection(hal_context,dbus);
  	else
  		ptracesfl("Cannot Initialise HAL Connection", MT_ERROR, VIDEODEVICE_TRACE);
  }else
  	ptracesfl("Cannot Initialise HAL Connection", MT_ERROR, VIDEODEVICE_TRACE);
  	
  //Getting devices
  int numDevices= 0;
  ptracesfl("Enumerating V4L Capable devices ...", MT_INFO, VIDEODEVICE_TRACE);
  char** devices= libhal_find_device_by_capability(hal_context,"video4linux", &numDevices, NULL);
  
  for(int i= 0; i < numDevices; i++){
  	char* aDevice;
  	
  	aDevice= libhal_device_get_property_string (hal_context, devices[i], "video4linux.device", NULL);
  	
  	ptracesfl("\tFound device: ", MT_INFO, VIDEODEVICE_TRACE, false);
  	ptracesfl(aDevice, MT_NONE, VIDEODEVICE_TRACE);
  	ret.push_back(string(aDevice));
  	
  	libhal_free_string(aDevice);
  }
  
  for(int i= 0; i < numDevices; i++)
  	delete devices[i];
  	
  delete devices;
      
  return ret;
  
}


void VideoDeviceManager::Terminate(){
		
	if( Command::videoDevice != NULL ){
		
		if( createCommand )
			Command::ChangingDevice();
		
		Command::videoDevice->closeDevice();
		delete Command::videoDevice;
		Command::videoDevice= NULL;
		
		if( createCommand )
			Command::DeviceChanged();
	}
}