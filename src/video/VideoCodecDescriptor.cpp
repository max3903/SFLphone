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
#include "ffmpeg/avcodec.h"
 	VideoCodecDescriptor::~VideoCodecDescriptor(){}
	
    VideoCodecDescriptor::VideoCodecDescriptor(){
    
    init();
    
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
    
    void VideoCodecDescriptor::init(){
    
    //Create map list
    
    //check if user has settings for the active list, if yes load them else setDefault
    
    //Register codecs
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

   
    int VideoCodecDescriptor::addCodec(enum CodecID id){
    	
    	//find codec
    	
    	
    return 1;}
    
	
    VideoCodecOrder VideoCodecDescriptor::getActiveCodecs() { return vCodecOrder; }
    
	
    void VideoCodecDescriptor::setActiveCodecs(VideoCodecOrder vCodecOrder)
    {
    	this->vCodecOrder = vCodecOrder;
    }
	
    void VideoCodecDescriptor::setCodecMap(VideoCodecMap codec){this->vCodecMap = codec;}
    
    
