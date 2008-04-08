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

#include "StraightThrough_AT.h"

StraightThrough_AT::StraightThrough_AT()
{
}

StraightThrough_AT::StraightThrough_AT(InternalBuffer* input, OutputStream* output)
{
  inputBuffer = input;
  outputBuffer = output;
  
  Active=false;
  OkToKill=false;
}

StraightThrough_AT::~StraightThrough_AT()
{
}

void StraightThrough_AT::run()
{
  Active=true;
  OkToKill=false;
  
  int size;
  short* data= NULL;
 
  ptracesfl("StraightThrough_AT - pause(): Starting ...",MT_INFO,STRAIGHTTHROUGH_TRACE); 
  while(Active)
  {
  	
  	ptracesfl("StraightThrough_AT - run(): Getting internal audio buffer size",MT_INFO,STRAIGHTTHROUGH_TRACE);
    int sizeBuffer = inputBuffer->getSizeBuffer();
    
    if( sizeBuffer != 0){

    	//data= new short[ sizeBuffer ];
    	
     	ptracesfl("StraightThrough_AT - run(): Fetching data from internal audio buffer ...",MT_INFO,STRAIGHTTHROUGH_TRACE);
    	data = (short*)(inputBuffer->fetchData(size));
	    if( size != -1 ){
	    	ptracesfl("StraightThrough_AT - run(): Sending data to audio output buffer ...",MT_INFO,STRAIGHTTHROUGH_TRACE);
		    outputBuffer->putData(data,sizeBuffer);
	    }else{
   			ptracesfl("StraightThrough_AT - run(): Cannot fetch data from internal buffer",MT_ERROR,STRAIGHTTHROUGH_TRACE);
	    }
	    
	    data= NULL;
   			
    }else
    	ptracesfl("StraightThrough_AT - run(): No Data to fetch", MT_WARNING, STRAIGHTTHROUGH_TRACE);
    
    // This thread must sleep longer than the sync thread to create a continious stream of information for the output.
    usleep(51);
    
  }
  
//  ptracesfl("StraightThrough_AT - run(): The run thread has stopped cleanly",MT_INFO,STRAIGHTTHROUGH_TRACE);
  
  OkToKill=true;
  
}

void StraightThrough_AT::pause()
{ 
  ptracesfl("StraightThrough_AT - pause(): Pausing ...",MT_INFO,STRAIGHTTHROUGH_TRACE);
  
  Active=false;
  OkToKill=false;
  
  //Watting for the thread to stop
  while(!OkToKill);
}

void StraightThrough_AT::restart()
{
  ptracesfl("StraightThrough_AT - restart(): Restarting ...",MT_INFO,STRAIGHTTHROUGH_TRACE);
  run();
}

void StraightThrough_AT::stop()
{ 
  ptracesfl("StraightThrough_AT - stop(): Stopping ...",MT_INFO,STRAIGHTTHROUGH_TRACE);
  
  Active=false;
  OkToKill=false;
  
  //Watting for the thread to stop
  while(!OkToKill);
  
  terminate();
}

