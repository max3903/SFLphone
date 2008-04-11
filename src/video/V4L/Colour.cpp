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

#include "Colour.h"

Colour::Colour(){}

Colour::~Colour(){}

bool Colour::increase(){
  	
  	if( Command::videoDevice == NULL )
		return false;
		
  	// Get Hue Control
	Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::COLOR );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->increment();
  		
  	ptracesfl("Hue control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false; 		
    
}

bool Colour::decrease(){
    
    if( Command::videoDevice == NULL )
		return false;
		
    // Get Hue Control
	Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::COLOR );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->decrement();
  		
  	ptracesfl("Hue control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
    
}

bool Colour::setTo(__u16 value){

	if( Command::videoDevice == NULL )
		return false;
		
	// Get Hue Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::COLOR );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->setValue(value);
  		
  	ptracesfl("Hue control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
    
}

bool Colour::reset(){

	if( Command::videoDevice == NULL )
		return false;
		
	// Get Hue Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::COLOR );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->reset();
  		
  	ptracesfl("Hue control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return false;
    
}
  
int Colour::getColour(){

	if( Command::videoDevice == NULL )
		return -1;
		
	// Get Hue Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::COLOR );
  	
  	if( tmpCtrl != NULL )
  		return tmpCtrl->getValue();
  		
  	ptracesfl("Hue control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return -1;
  	
}
  
CmdDesc Colour::getCmdDescriptor(){
	
	CmdDesc tmpDesc= {-1,-1,-1,-1};
	
	if( Command::videoDevice == NULL )
		return tmpDesc;
	
	// Get Hue Control
    Control* tmpCtrl= Command::videoDevice->getConfigSet()->getControl( Control::COLOR );
  	
  	if( tmpCtrl != NULL ){
  		tmpDesc.Max=  tmpCtrl->getMax();
  		tmpDesc.Min=  tmpCtrl->getMin();
  		tmpDesc.Step=  tmpCtrl->getStep();
  		tmpDesc.Current=  tmpCtrl->getValue();
  		return tmpDesc;
  	}
  		
  	ptracesfl("Hue control is not supported by ", MT_ERROR, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getPath(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" (", MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(Command::videoDevice->getName(), MT_NONE, COMMAND_TRACE, false);
  	ptracesfl(" )", MT_NONE, COMMAND_TRACE);
  	return tmpDesc;
		
}
