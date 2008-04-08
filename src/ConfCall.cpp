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
 
#include "ConfCall.h"
#include "manager.h"

ConfCall::ConfCall(){
}

ConfCall::ConfCall( Call* call1, Call* call2 ){
	this->call_1= call1;
	this->call_2= call2;
	
	this->ConfId= Manager::instance().getNewCallID();
	
	this->init();
	
	
}

ConfCall::~ConfCall(){
}

Call* ConfCall::getCall1(){
	return this->call_1;
}
	
Call* ConfCall::getCall2(){
	return this->call_2;
}
	
void ConfCall::endConf(){
	
	this->localMixer->terminate();
	
	// Reset the Call's mixers
	this->call_1->setConfMode( NULL, NULL );
	this->call_1->setConfId( "" );
	this->call_2->setConfMode( NULL, NULL );
	this->call_2->setConfId( "" );
	
	delete audioOut;
	delete videoOut;
	
}

ConfID ConfCall::getConfId() const{
	return this->ConfId;
}
	
void ConfCall::init(){
	
	// Distribute the counter part buffers
	this->call_1->setConfMode( this->call_2->getLocal_Video_Input(), this->call_2->getLocal_Audio_Input() );
	this->call_1->setConfId(this->ConfId);
	this->call_2->setConfMode( this->call_1->getLocal_Video_Input(), this->call_1->getLocal_Audio_Input() );
	this->call_2->setConfId(this->ConfId);
	
	// Create The input streams
	vector<InputStreams*> tmpInput;
	
	tmpInput.push_back( new InputStreams( this->call_1->getLocal_Video_Input(), this->call_1->getLocal_Audio_Input() ) );
	tmpInput.push_back( new InputStreams( this->call_2->getLocal_Video_Input(), this->call_2->getLocal_Audio_Input() ) );
	
	// Create the local outputs
	this->audioOut= new LocalAudioOuput();
	this->videoOut= new LocalVideoOuput();
	
	//Create and start the new local mixer
	this->localMixer= new Mixer( Mixer::NOSYNCH_AV_MIXER2, tmpInput, this-> audioOut, this->videoOut);
	
	this->localMixer->start();
	
}


