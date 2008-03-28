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

#include "Resolution.h"

Resolution::Resolution(){}

Resolution::~Resolution(){}

bool Resolution::setTo(__u16 valueX, __u16 valueY){
    
bool ret = false;
    
    ptracesfl("Resolution getting device ...", MT_INFO, COMMAND_TRACE);
    this->getVideoDeviceAccess();
    
    ptracesfl("Resolution stopping capture ...", MT_INFO, COMMAND_TRACE);
    Command::videoDevice->getCaptureMode()->stopCapture(Command::videoDevice->getFileDescript());

    ptracesfl("Changing Resolution ...", MT_INFO, COMMAND_TRACE, false);
    ret = Command::videoDevice->setResolution(valueX, valueY);
    
    if(ret)
    	 ptracesfl("\tOK", MT_NONE, COMMAND_TRACE);
   	else
   		ptracesfl("\tNO", MT_NONE, COMMAND_TRACE);
    
    ptracesfl("Resolution starting capture ...", MT_INFO, COMMAND_TRACE);
    Command::videoDevice->getCaptureMode()->startCapture(Command::videoDevice->getFileDescript());    
         
    ptracesfl("Resolution releasing device ...", MT_INFO, COMMAND_TRACE);
    this->releaseVideoDevice();
    
    return ret;
    
}
  
bool Resolution::setTo( char* resolution){
  	  	
  	  	int valx= -1,valy= -1;
  	  	
  	  	sscanf(resolution, "%d x %d", &valx, &valy);
  	  	
  	  	if( valx != -1 && valy != -1 )
  	  		return this->setTo(valx, valy);
  	  	else
  	  		return false;

}

pair<int,int> Resolution::getResolution(){

	ImageSize* tmpSize= Command::videoDevice->getConfigSet()->getCurrentFormat()->getCurrentImageSize();
	
	return pair<int,int>(tmpSize->getWidth(), tmpSize->getHeight());
	
}
  
const char* Resolution::enumResolution(){
  	return Command::videoDevice->getConfigSet()->getCurrentFormat()->getAllSizes();
}
    
const char* Resolution::enumFPS(){
  	return Command::videoDevice->getConfigSet()->getCurrentFormat()->getCurrentImageSize()->getFpsString();
}
  
bool Resolution::setFpsTo( int fps ){
  	
  	ptracesfl("Resolution getting device ...", MT_INFO, COMMAND_TRACE);
    this->getVideoDeviceAccess();
    
    ptracesfl("Resolution stopping capture ...", MT_INFO, COMMAND_TRACE);
    Command::videoDevice->getCaptureMode()->stopCapture(Command::videoDevice->getFileDescript());
    
    ptracesfl("Changing fps rate ...", MT_INFO, COMMAND_TRACE, false);
  	bool ret= Command::videoDevice->setFPS(fps);
  	
  	if(ret)
    	 ptracesfl("\tOK", MT_NONE, COMMAND_TRACE);
   	else
   		ptracesfl("\tNO", MT_NONE, COMMAND_TRACE);
  	
  	ptracesfl("Resolution starting capture ...", MT_INFO, COMMAND_TRACE);
  	Command::videoDevice->getCaptureMode()->startCapture(Command::videoDevice->getFileDescript());    
         
    ptracesfl("Resolution releasing device ...", MT_INFO, COMMAND_TRACE);
    this->releaseVideoDevice();
    
    return ret;
  	
}
