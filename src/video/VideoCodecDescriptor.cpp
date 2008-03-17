/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean-Francois Blanchard-Dionne <jean-francois.blanchard-dionne@polymtl.ca>
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
#include "VideoCodecDescriptor.h"

#include <string>
#include <stdio.h>

using namespace std;

VideoCodecDescriptor* VideoCodecDescriptor::instance= 0;

VideoCodecDescriptor* VideoCodecDescriptor::getInstance()
{
	//if no instance made create one,
	//ref. singleton pattern
	if (instance == 0)
	instance = new VideoCodecDescriptor();

	return instance;
}

 	VideoCodecDescriptor::~VideoCodecDescriptor(){
 	vCodecOrder.clear();
 	vCodecMap.clear();
 	}
	
    VideoCodecDescriptor::VideoCodecDescriptor(){
    	av_register_all();
    	avcodec_init();
    	init();}

    void VideoCodecDescriptor::init()
    {
    VCMIterator mapIter;
    //Create map list
    if (initCodecMap() == false)
    {
    	printf("CodecMap init error");
    	exit(-1);
    }
    //check if user has settings for the active list, if yes load them else setDefault
    //TODO
   
    }
    
     bool VideoCodecDescriptor::initCodecMap()
    {
		FILE *codecFile;
		char *codec;
		AVCodec* tmp;
		//open videoDescriptor File
		codecFile = fopen("videoCodecs.dat","r");
		if (codecFile == NULL)
    		return false;
    	
    	while(fgets(codec,6,codecFile) != NULL)
    	{
    	printf("%s ",codec);
    	//make sure you can encode with codec read in file
    	tmp = avcodec_find_encoder_by_name(codec);
    	
    		if(tmp != NULL)
    		{
    		//make sure you can decode with codec read in file
    		tmp = avcodec_find_decoder_by_name(codec);
    			if(tmp != NULL)
    			{
    			//map Codec
    			vCodecMap[tmp] = avcodec_alloc_context();	
    			}
    		}
    	}
    
    return true;
    }
    
    
    bool VideoCodecDescriptor::setDefaultOrder(){
    
    VCMIterator mapIter;
    //Set the default order of the codec list
    //means setting the exact same codecs as codecMap
    vCodecOrder.clear();
  
    for (mapIter = vCodecMap.begin();mapIter != vCodecMap.end();mapIter++)
    	vCodecOrder.push_back((*mapIter).first);
    	
    return true;
    }
    
   
    bool VideoCodecDescriptor::isActive(enum CodecID id){
   
    VCOIterator iter;
    
    for (iter = vCodecOrder.begin();iter != vCodecOrder.end();iter++)
    	if ((*iter)->id == id)
    	return true;
    
    return false;
    }

   
    bool VideoCodecDescriptor::removeCodec(enum CodecID id){
    
    VCOIterator iter;
    
    for (iter = vCodecOrder.begin();iter != vCodecOrder.end();iter++)
    	if ((*iter)->id == id)
    	{
    	vCodecOrder.erase(iter);
    	return true;
    	}
    
    return false;
    }

   
    bool VideoCodecDescriptor::addCodec(enum CodecID id){
    	
    	//find codec
    	VCMIterator mapIter;
    	
    	for (mapIter = vCodecMap.begin();mapIter != vCodecMap.end();mapIter++)
    	if ((*mapIter).first->id == id)
    	{
    	vCodecOrder.push_back((*mapIter).first);
    	return true;
    	}
    	
    return false;}
    
    char* VideoCodecDescriptor::serialize()
    {
    //return
    //TODO
    return "doh!";
    }
    
	AVCodecContext* VideoCodecDescriptor::getCodecContext(AVCodec* Codec)
	{
		VCMIterator tmp;
	
	tmp = vCodecMap.find(Codec);
	
	return (*tmp).second;
	
	}
    VideoCodecOrder VideoCodecDescriptor::getActiveCodecs() { return vCodecOrder; }
    
	
    void VideoCodecDescriptor::setActiveCodecs(VideoCodecOrder vCodecOrder)
    {
    	this->vCodecOrder = vCodecOrder;
    }
	
    void VideoCodecDescriptor::setCodecMap(VideoCodecMap codec){this->vCodecMap = codec;}
    
    
