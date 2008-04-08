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


#include "LocalAudioOuput.h"
#include "../manager.h"

LocalAudioOuput::LocalAudioOuput(){
}

LocalAudioOuput::~LocalAudioOuput()
{
}

void LocalAudioOuput::putData(short * data, int size)
{		
	if( data != NULL && size > 0 ){
		
		audioLayer = Manager::instance().getAudioDriver();
		if( audioLayer ){
			ptracesfl("LocalAudioOuput - putData () : playing Sample", MT_INFO, LOCALAUDIOOUPUT_TRACE);
			audioLayer->playSamples( data, size );
//			audioLayer->putUrgent( data, size );			
			ptracesfl("LocalAudioOuput - putData () : Finnish playing Sample", MT_INFO, LOCALAUDIOOUPUT_TRACE);
			
		}else{
			ptracesfl("LocalAudioOuput - putData () : Cannot get audio layer audio driver", MT_ERROR, LOCALAUDIOOUPUT_TRACE);
		}
	}else
		ptracesfl("LocalAudioOutput - putData (): Parameter error, passed NULL pointer or size < 0.", MT_ERROR, LOCALAUDIOOUPUT_TRACE);
}

int LocalAudioOuput::fetchData(short *data)
{
	ptracesfl("LocalAudioOuput - fetchData () : This function should never be called !", MT_FATAL, LOCALAUDIOOUPUT_TRACE);
	return -1;
}

const char* LocalAudioOuput::getType() const{
	return "Local Audio Output Buffer";
}
