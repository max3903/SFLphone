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

 	VideoCodecDescriptor::~VideoCodecDescriptor()
 	{
 	vCodecOrder.clear();
 	vCodecMap.clear();
 	delete instance;
 	}
	
    VideoCodecDescriptor::VideoCodecDescriptor()
    {
    	av_register_all();
    	avcodec_init();
    	init();
    }

    void VideoCodecDescriptor::init()
    {
    VCMIterator mapIter;
    //Create map list
    if (initCodecMap() == false)
    {
    	ptracesfl("videoCodecInit error",MT_FATAL,2,true);
    }
    //check if user has settings for the active list, if yes load them else setDefault
    //TODO
   
    }
    
     bool VideoCodecDescriptor::initCodecMap()
    {
		FILE *codecFile;
		char *codec;
		AVCodec* tmp;
		//TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		//open videoDescriptor File
//		codecFile = fopen("videoCodecs.dat","b");
//		if (codecFile == NULL){
//			ptrace("Codec File Not found",MT_ERROR,2,false);
//			return false;
//			}
//			
//    	while(fgets(codec,6,codecFile) != NULL)
//    	{
//    	ptrace("Codec : ",MT_NONE,2,false);
//    	ptrace(codec,MT_INFO,2,true);
//    	//make sure you can encode with codec read in file
//    	tmp = avcodec_find_encoder_by_name(codec);
//    	
//    		if(tmp != NULL)
//    		{
//    		//make sure you can decode with codec read in file
    		
    			tmp = avcodec_find_decoder_by_name("h264");
    			if(tmp != NULL)
    			{
    			//map Codec
    			ptracesfl(tmp->name,MT_INFO,2,false);
    			ptracesfl(" Found",MT_NONE,2,true);
    			vCodecMap[tmp] = avcodec_alloc_context();	
    			}
    			tmp = avcodec_find_decoder_by_name("h263");
    			if(tmp != NULL)
    			{
    			//map Codec
    			ptracesfl(tmp->name,MT_INFO,2,false);
    			ptracesfl(" Found",MT_NONE,2,true);
    			vCodecMap[tmp] = avcodec_alloc_context();	
    			}
//    		}
//    	}
    
    return true;
    }
    
    
    bool VideoCodecDescriptor::setDefaultOrder(){
    
    VCMIterator mapIter;
    VCOIterator iter;
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
    
    
    /********************************************
     * Functions for MEMMANAGER
     ********************************************
    */
     
     
    StringVector VideoCodecDescriptor::getStringActiveCodecs()
    {
    	StringVector tmp;
    	VCOIterator iter;
    	
    	for ( iter = this->vCodecOrder.begin(); iter != this->vCodecOrder.end();iter++)
    		tmp.push_back((string)(*iter)->name);

  		return tmp;
    }
    
    bool VideoCodecDescriptor::saveActiveCodecs(StringVector sActiveCodecs)
    {
    	
    	StringVectorIterator iter;
    	AVCodec *tmp;
    	bool saveOk =true;
    	vCodecOrder.clear();
    	
    	for ( iter = sActiveCodecs.begin(); iter != sActiveCodecs.end();iter++)
    		{
    			tmp = avcodec_find_decoder_by_name((*iter).c_str());
    			if(tmp != NULL)
    				vCodecOrder.push_back(tmp);
    			else
    			{
    			ptracesfl("Codec Not Found",MT_ERROR,2,true);
    			saveOk =false;
    			}
    		}
    		return saveOk;
    }
    
    StringVector VideoCodecDescriptor::getStringCodecMap()
    {
    StringVector tmp;
    VCMIterator iter;
    
    for ( iter = this->vCodecMap.begin(); iter != this->vCodecMap.end();iter++)
    		tmp.push_back((string)(*iter).first->name);
  	
  		return tmp;
    
    }
    
 
    bool VideoCodecDescriptor::saveCodecMap(StringVector sCodecMap)
    {
    	StringVectorIterator iter;
    	AVCodec *tmp;
    	bool saveOk =true;
    	
    	vCodecMap.clear();
    
    for ( iter = sCodecMap.begin(); iter != sCodecMap.end();iter++)
    	{
	    	tmp = avcodec_find_decoder_by_name((*iter).c_str());
	    	
	    	if(tmp != NULL)
	    			vCodecMap[tmp] = avcodec_alloc_context();
	    		else
	    		{
	    			ptracesfl("Codec Not Found",MT_ERROR,2,true);
	    			saveOk =false;	
	    		}
    	}
  
    }
    
    
    
