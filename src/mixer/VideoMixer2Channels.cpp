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

#include "VideoMixer2Channels.h"
#include "OutputStream.h"


void VideoMixer2Channels::run()
{
	Active=true;
	OkToKill=false;
  
	data1= NULL;
	data2= NULL;
	mixedData= NULL;
 
	ptracesfl("VideoMixer2Channels - pause(): Starting ...",MT_INFO,VIDEOMIXER2CHANNELS_TRACE); 
	while(Active)
	{
		ptracesfl("VideoMixer2Channels - run(): Getting internal audio buffer1 size",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
		this->sizeBuffer1 = inputBuffer1->getSizeBuffer();
		
		ptracesfl("VideoMixer2Channels - run(): Getting internal audio buffer2 size",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
		this->sizeBuffer2 = inputBuffer2->getSizeBuffer();
		
		//TODO : get width1 and 2 and height1 and 2 and change them when necessary
    
		if(( sizeBuffer1 != 0)&&( sizeBuffer2 != 0)){
			
			ptracesfl("VideoMixer2Channels - run(): Fetching data from internal audio buffer ...",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
			
			if(( inputBuffer1->fetchData(data1) != -1 ) && ( inputBuffer1->fetchData(data1) != -1 )){
				
				ptracesfl("VideoMixer2Channels - run(): Mixing data ...",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
				
				//find bigger height
				int biggerHeight;
	
				if(height1 > height2){
					biggerHeight = 	height1;
				}
				else
					biggerHeight = 	height2;
							
				// mixing data : cmposing 2 videos
				this->paddingSize =  this->paddingWidth*biggerHeight*3;				
				this->mixedData = new/*(mixedData)*/ unsigned char[sizeBuffer1+paddingSize+sizeBuffer2];
						
				this->mixVideo(); // 64 is the padding width
				
				ptracesfl("VideoMixer2Channels - run(): Sending mixedData to audio output buffer ...",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
				outputBuffer->putData(this->mixedData, sizeBuffer1/* TODO, (width1+width2+64), biggerHeight*/);
				//delete mixedData;
				 
			}else{
				ptracesfl("VideoMixer2Channels - run(): Cannot fetch data from internal buffer",MT_ERROR,VIDEOMIXER2CHANNELS_TRACE);
				//delete mixedData; 
			}
			
			mixedData= NULL;
   			
		}else
			ptracesfl("VideoMixer2Channels - run(): No Data to fetch", MT_WARNING, VIDEOMIXER2CHANNELS_TRACE);
    	
    	usleep(2);
    }
	
	ptracesfl("VideoMixer2Channels - run(): The run thread has stopped cleanly",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
	OkToKill=true;
	
	
	
	
	
	
}

void VideoMixer2Channels::mixVideo(/*char* data1, char* data2, int width1, int height1, int width2, int height2, int paddingWidth, char* mixedVideo*/){
	
	int biggerHeight;
	char* padding;	
	int sizeData1 = (int)(this->width1*this->height1*3*sizeof(char));
	int sizeData2 = (int)(this->width2*this->height2*3*sizeof(char));
	
	//find bigger height
	if(this->height1 > this->height2){
		biggerHeight = 	this->height1;	
	}
	else
		biggerHeight = 	this->height2;
	
//	this->paddingSize =  paddingWidth*biggerHeight*3;
	padding = new char[paddingSize]; // TODO modifier le new
	
	for(int i=0; i<this->paddingSize; i+=3){
		padding[i] = 0; // RED
		padding[i+1] = 0; // GREEN
		padding[i+2] = 0; // BLUE
	}
//	printf("TTTTTTTTTTTTTTTTTTTTTTTT\n");
	for(int i=0; i<biggerHeight; i++){
		memcpy(this->mixedData+(paddingWidth*3+this->width1*3+this->width2*3)*i, data1+this->width1*3*i, this->width1*3); // image 1 (ligne 1 de l'image)
		memcpy(this->mixedData+(paddingWidth*3+this->width1+this->width2*3)*i+this->width1*3, padding, paddingWidth*3); // ajout de padding
		memcpy(this->mixedData+(paddingWidth*3+this->width1*3+this->width2*3)*i+this->width1*3+paddingWidth*3, data2+this->width2*3*i, this->width2*3); // image 2 (ligne 1 de l'image)
//		printf("TTTTTTTTTTTTTTTTTTTTTTTT  %d  \n", i);
	}
	
	delete [] padding;	
	
}

void VideoMixer2Channels::pause()
{
	ptracesfl("VideoMixer2Channels - pause(): Pausing ...",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
	Active=false;
}

void VideoMixer2Channels::restart()
{
	ptracesfl("VideoMixer2Channels - restart(): Restarting ...",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
	run();
}

void VideoMixer2Channels::stop()
{
	ptracesfl("VideoMixer2Channels - stop(): Stopping ...",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
	Active=false;
	OkToKill=false;
	while(!OkToKill);
	terminate();	
}

VideoMixer2Channels::VideoMixer2Channels(InternalBuffer* video1, InternalBuffer* video2, OutputStream* output, CodecInfo* infos)
{
	ptracesfl("VideoMixer2Channels - VideoMixer2Channels()",MT_INFO,VIDEOMIXER2CHANNELS_TRACE);
	inputBuffer1 = video1;
    inputBuffer2 = video2;
    
    outputBuffer = output;
    
    infoCodecs = infos;
    
    paddingWidth = 64;
    
	Active=true;
	OkToKill=true;
}

VideoMixer2Channels::VideoMixer2Channels()
{
}

VideoMixer2Channels::~VideoMixer2Channels()
{
}