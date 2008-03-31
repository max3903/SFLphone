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

#include "ConfigSet.h"

ConfigSet::ConfigSet(int fd){
	
	this->init_enumerateImagesFormats(fd);
	
	this->current= this->formats.begin();
	
	// if no format are supported, there is no point in enumerating supported controls
	if( this->formats.size() != 0 )
		this->init_Controls(fd);
		
}

ConfigSet::~ConfigSet(){
}

Format* ConfigSet::getCurrentFormat(){
	
	if( this->current == this->formats.end() ){
		return NULL;	// No formats are supported
	}
		
	return (*this->current);
}

void ConfigSet::nextFormat(){
	
	this->current++;
	
	// Create return to beginning when at end
	if( this->current == this->formats.end() )
		this->current == this->formats.begin();
}

vector<Format*> ConfigSet::getFormats(){
	return this->formats;
}

Control* ConfigSet::getControl( int type ){
	
	string tmpSearch= "Default";
	switch( type ){
		case Control::BRIGHTNESS:
			tmpSearch= "Brightness";
			break;
		case Control::COLOR:
			tmpSearch= "Hue";
			break;
		case Control::CONTRAST:
			tmpSearch= "Contrast";
			break;
	}
			
	map<string,Control*>::iterator result= this->controlMap.find(tmpSearch);
	if( result != this->controlMap.end() ){
		return (*result).second;
	}	
	return NULL;	// No control of type supported by VideoDevice
}

void ConfigSet::init_enumerateImagesFormats(int fd){
	
	int ret;
	struct v4l2_fmtdesc *FrameFormat= (v4l2_fmtdesc*)calloc(1,sizeof(v4l2_fmtdesc));
	char fourCC[5];
	fourCC[4]='\0';
	
	FrameFormat->index = 0;
	FrameFormat->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	ret = ioctl(fd, VIDIOC_ENUM_FMT, FrameFormat);	
	while(ret == 0){ // Loop until no format are available
		
		// Extracting the fourcc code
		fourCC[0]= FrameFormat->pixelformat & 0xFF;
		fourCC[1]= (FrameFormat->pixelformat >> 8) & 0xFF;
		fourCC[2]= (FrameFormat->pixelformat >> 16) & 0xFF;
		fourCC[3]= (FrameFormat->pixelformat >> 24) & 0xFF;
				
		ptracesfl("\tCamera Supported format: ", MT_INFO, CONFIGSET_TRACE, false);
		ptracesfl(fourCC, MT_NONE, CONFIGSET_TRACE, false);
		Format* tmp= getFormat(fourCC, (char*)FrameFormat->description, fd, FrameFormat->pixelformat );
		
		if(tmp != NULL){
			ptracesfl("\n", MT_NONE, 3, false);
			this->formats.push_back( tmp );
		}else
			ptracesfl(" format not supported by sflphone", MT_NONE, CONFIGSET_TRACE);
			
		FrameFormat->index++;
		ret = ioctl(fd, VIDIOC_ENUM_FMT, FrameFormat);
	}
	
	free(FrameFormat);
	
}

void ConfigSet::init_Controls( int fd ){
	
	v4l2_queryctrl* tmpQuery= (v4l2_queryctrl*)calloc(1,sizeof(v4l2_queryctrl));
	Control* tmpCtrl= NULL;
	string tmpString= "";
	
	ptracesfl("Listing supported controls ...", MT_INFO, CONFIGSET_TRACE );
	for( tmpQuery->id= V4L2_CID_BASE; tmpQuery->id < V4L2_CID_LASTP1; tmpQuery->id++){
		if (0 == ioctl (fd, VIDIOC_QUERYCTRL, tmpQuery)) {
      		if (tmpQuery->flags & V4L2_CTRL_FLAG_DISABLED){
      			ptracesfl("Control ", MT_INFO, CONFIGSET_TRACE+1, false );
      			ptracesfl((char*)tmpQuery->name, MT_NONE, CONFIGSET_TRACE+1, false );
      			ptracesfl(" not supported", MT_NONE, CONFIGSET_TRACE+1, true );
        		continue;
      		}else{
      			tmpString= (char*)tmpQuery->name;
      			tmpCtrl= new Control(tmpQuery->id,fd);
      			ptracesfl("Control ", MT_INFO, CONFIGSET_TRACE+1, false );
      			ptracesfl(tmpString.c_str(), MT_NONE, CONFIGSET_TRACE+1, false );
      			ptracesfl(" supported", MT_NONE, CONFIGSET_TRACE+1, true );
      			this->controlMap[tmpString]= tmpCtrl;
        	}
		}
	}
	
	free(tmpQuery);
	
}

Format* ConfigSet::getFormat(char * pFormat, char * Description, int fd, __u32 theFormat){
	
	switch(theFormat){
		case V4L2_PIX_FMT_YUV420:
			return new YUV420(fd);
			break;
		case V4L2_PIX_FMT_YUYV:
			return new YUV422(fd);
			break;
	}
	
	return NULL;
	
}
