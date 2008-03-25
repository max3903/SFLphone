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

#include "Capture.h"

Capture::Capture(){}

Capture::~Capture(){}

unsigned char* Capture::GetCapture(){
    
	ptracesfl("Capture getting device ...", MT_INFO, COMMAND_TRACE);
    this->getVideoDeviceAccess();
    
    ptracesfl("Getting current capture mode:", MT_INFO, COMMAND_TRACE, false);
    CaptureMode *capMode= Command::videoDevice->getCaptureMode();
    ptracesfl("\tOK\n", MT_NONE, COMMAND_TRACE);
    	
    unsigned char* tmp= capMode->capture(Command::videoDevice);
    
    ptracesfl("Capture releasing device ...\n", MT_INFO, COMMAND_TRACE);
    this->releaseVideoDevice();
    	
    return tmp;
}

CmdDesc Capture::getCmdDescriptor(){
	
	CmdDesc tmpDesc= {-1,-1,-1,-1};
		  		
  	ptracesfl("Capture releasing device ...\n", MT_WARNING, COMMAND_TRACE);
  	return tmpDesc;
		
}
