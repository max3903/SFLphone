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

#include "Brightness.h"

Brightness::Brightness(){}

Brightness::~Brightness(){}

bool Brightness::increase(){

	// Get Brightness Control
	Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::BRIGHTNESS );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->increment();
  		
  	ptracesfl("Brightness control is not supported by ", MT_WARNING, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(")", MT_NONE, COMMAND_TRACE);
  	return false;
    
}

bool Brightness::decrease(){
    
    // Get Brightness Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::BRIGHTNESS );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->decrement();
  		
  	ptracesfl("Brightness control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
    
}

bool Brightness::setTo(__u16 value){

	// Get Brightness Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::BRIGHTNESS );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->setValue(value);
  		
  	ptracesfl("Brightness control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
}

bool Brightness::reset(){

	// Get Brightness Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::BRIGHTNESS );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->reset();
  		
  	ptracesfl("Brightness control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
}
  
int Brightness::getBrightness(){

	// Get Brightness Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::BRIGHTNESS );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->getValue();
  		
  	ptracesfl("Brightness control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
  	
}
  
CmdDesc Brightness::getCmdDescriptor(){
	
	CmdDesc tmpDesc= {-1,-1,-1,-1};
	
	// Get Brightness Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::BRIGHTNESS );
  	
  	if( tmpCtrl != NULL ){
  		tmpDesc.Max=  tmpCtrl->getMax();
  		tmpDesc.Min=  tmpCtrl->getMin();
  		tmpDesc.Step=  tmpCtrl->getStep();
  		tmpDesc.Current=  tmpCtrl->getValue();
  		return tmpDesc;
  	}
  		
  	ptracesfl("Brightness control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return tmpDesc;
		
}

