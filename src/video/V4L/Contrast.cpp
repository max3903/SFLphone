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

#include "Contrast.h"

Contrast::Contrast(){}

Contrast::~Contrast(){}

bool Contrast::increase(){
  	
  	if( Command::videoDevice == NULL )
		return false;
		
  	// Get Contrast Control
  	Control* tmpCtrl= this->videoDevice->getConfigSet()->getControl( Control::CONTRAST );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->increment();
  		
  	ptracesfl("Contrast control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
    
}

bool Contrast::decrease(){
    
    if( Command::videoDevice == NULL )
		return false;
		
    // Get Contrast Control
    Control* tmpCtrl= this->videoDevice->getConfigSet()->getControl( Control::CONTRAST );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->decrement();
  		
  	ptracesfl("Contrast control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
    
}

bool Contrast::setTo(__u16 value){

	if( Command::videoDevice == NULL )
		return false;
		
	// Get Contrast Control
    Control* tmpCtrl= this->videoDevice->getConfigSet()->getControl( Control::CONTRAST );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->setValue(value);
  		
  	ptracesfl("Contrast control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
}

bool Contrast::reset(){

	if( Command::videoDevice == NULL )
		return false;
		
	// Get Contrast Control
    Control* tmpCtrl= this->videoDevice->getConfigSet()->getControl( Control::CONTRAST );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->reset();
  		
  	ptracesfl("Contrast control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
}
  
int Contrast::getContrast(){

	if( Command::videoDevice == NULL )
		return false;
		
	// Get Contrast Control
    Control* tmpCtrl= this->videoDevice->getConfigSet()->getControl( Control::CONTRAST );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->getValue();
  		
  	ptracesfl("Contrast control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
  	
}
  
CmdDesc Contrast::getCmdDescriptor(){
	
	CmdDesc tmpDesc= {-1,-1,-1,-1};
	
	if( Command::videoDevice == NULL )
		return tmpDesc;
	
	// Get Contrast Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::CONTRAST );
  	
  	if( tmpCtrl != NULL ){
  		tmpDesc.Max=  tmpCtrl->getMax();
  		tmpDesc.Min=  tmpCtrl->getMin();
  		tmpDesc.Step=  tmpCtrl->getStep();
  		tmpDesc.Current=  tmpCtrl->getValue();
  		return tmpDesc;
  	}
  		
  	ptracesfl("Contrast control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return tmpDesc;
		
}
