/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Ala Eddine Limame <ala-eddine.limame@polymtl.ca>
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


#include "AudioMixer2Channels.h"

AudioMixer2Channels::AudioMixer2Channels()
{	
}

AudioMixer2Channels::AudioMixer2Channels(InternalBuffer* input1,  InternalBuffer* input2, OutputStream* output)
{
	inputBuffer1 = input1;
	inputBuffer2 = input2;
	outputBuffer = output;
	Active=false;
	OkToKill=false;	
}

AudioMixer2Channels::~AudioMixer2Channels()
{
}

void AudioMixer2Channels::run()
{
	Active=true;
	OkToKill=false;
  	
  	int size1;
  	int size2;
  	
	short* data1= NULL;
	short* data2= NULL;
	short* mixedData= NULL;
 
	ptracesfl("AudioMixer2Channels - pause(): Starting ...",MT_INFO,AUDIOMIXER2CHANNELS_TRACE); 
	while(Active)
	{
		ptracesfl("AudioMixer2Channels - run(): Getting internal audio buffer1 size",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
		this->sizeBuffer1 = inputBuffer1->getSizeBuffer();
		
		ptracesfl("AudioMixer2Channels - run(): Getting internal audio buffer2 size",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
		this->sizeBuffer2 = inputBuffer2->getSizeBuffer();
    
		if(( sizeBuffer1 != 0)&&( sizeBuffer2 != 0)){
			
//			ptracesfl("StraightThrough_AT - run(): Data1 Size is: %d",MT_INFO,AUDIOMIXER2CHANNELS_TRACE, true, 1, sizeBuffer1);
			//data1= new short[ this->sizeBuffer1 ];
			
//			ptracesfl("StraightThrough_AT - run(): Data2 Size is: %d",MT_INFO,AUDIOMIXER2CHANNELS_TRACE, true, 1, sizeBuffer2);
			//data2= new short[ this->sizeBuffer2 ];
			
			ptracesfl("AudioMixer2Channels - run(): Fetching data from internal audio buffer ...",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
			data1 = (short*)(inputBuffer1->fetchData(size1));
			data2 = (short*)(inputBuffer2->fetchData(size2));
			if(( size1 != -1 ) && ( size2 != -1 )){
				
				ptracesfl("AudioMixer2Channels - run(): Mixing data ...",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
				
				// chose the mixedData size ( size of buffer1 or 2 ) 
				if(this->sizeBuffer1 < this->sizeBuffer2)
					this->sizeBufferMixedData = this->sizeBuffer1;
				else
					this->sizeBufferMixedData = this->sizeBuffer2;
				
				// mixing data
				mixedData = new short[ this->sizeBufferMixedData ];
				
				*mixedData = mixData(*data1, *data2);
				
				ptracesfl("AudioMixer2Channels - run(): Sending mixedData to audio output buffer ...",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
				outputBuffer->putData(mixedData,this->sizeBufferMixedData);
				//delete mixedData;
				 
			}else{
				ptracesfl("AudioMixer2Channels - run(): Cannot fetch data from internal buffer",MT_ERROR,AUDIOMIXER2CHANNELS_TRACE);
				//delete mixedData; 
			}
			
			mixedData= NULL;
   			
		}else
			ptracesfl("AudioMixer2Channels - run(): No Data to fetch", MT_WARNING, AUDIOMIXER2CHANNELS_TRACE);
    	
    	usleep(2);
    }
	
	ptracesfl("AudioMixer2Channels - run(): The run thread has stopped cleanly",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
	OkToKill=true;
	
}

short AudioMixer2Channels::mixData(short data1, short data2){
	short mixedData = data1 + data2 - (data1*data2)/65536; // short is 16 bits -> 2^16 = 65536
	
	return mixedData;
}

void AudioMixer2Channels::pause()
{ 
  ptracesfl("AudioMixer2Channels - pause(): Pausing ...",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
  
  if(!Active)
  	return;
  	
  Active=false;
  
  while(!OkToKill);
}

void AudioMixer2Channels::restart()
{
  ptracesfl("AudioMixer2Channels - restart(): Restarting ...",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
  if( !Active )
  	run();
}

void AudioMixer2Channels::stop()
{ 
  ptracesfl("AudioMixer2Channels - stop(): Stopping ...",MT_INFO,AUDIOMIXER2CHANNELS_TRACE);
  
  if(!Active)
  	return;
  	
  Active=false;
  
  while(!OkToKill);
}
