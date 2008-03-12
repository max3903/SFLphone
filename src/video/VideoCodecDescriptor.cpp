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

    
    int VideoCodecDescriptor::setDefaultOrder(){
    
    
    //Set the default order of the codec list
    return 1;
    }
    
    void VideoCodecDescriptor::init(){
    
    //Create lists
    //Register codecs
    }
	
   
    bool VideoCodecDescriptor::isActive(enum CodecID id){
    
    return true;
    }

   
    int VideoCodecDescriptor::removeCodec(enum CodecID id){
    
    
    return 1;
    }

   
    int VideoCodecDescriptor::addCodec(enum CodecID id){
    return 1;}
	
    VideoCodecOrder& VideoCodecDescriptor::getActiveCodecs() { return activeCodecs; }
	
    void VideoCodecDescriptor::setActiveCodecs(VideoCodecOrder& activeCodecs){}
	
    void VideoCodecDescriptor::setCodecMap(VideoCodecMap& codec){}
    
    
