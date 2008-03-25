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

#include "Format.h"
#include "ImageSize.h"

Format::Format(){}

Format::~Format(){}

bool Format::isFPSSupported(int w, int h, int fps){
	
	vector<ImageSize*>::iterator start= this->sizes.begin();
	vector<ImageSize*>::iterator end= this->sizes.end();
	
	while( start != end )
	{
		if( ((ImageSize*)(*start))->getWidth() == w )
			if( ((ImageSize*)(*start))->getHeight() == h )
				if( ((ImageSize*)(*start))->isFPSSupported(fps) )
					return true;		
		start++;
	}
	
	return false; 
}

bool Format::isRessolutionSupported(int w, int h){ 

	vector<ImageSize*>::iterator start= this->sizes.begin();
	vector<ImageSize*>::iterator end= this->sizes.end();
	
	while( start != end )
	{
		if( ((ImageSize*)(*start))->getWidth() == w )
			if( ((ImageSize*)(*start))->getHeight() == h )
				return true;		
		start++;
	}
	
	return false; 

}



void Format::write_rgb(unsigned char **out, int Y, int U, int V)
{
	int R,G,B;
	R=(76284*Y+104595*V)>>16;
	G=(76284*Y -25625*U-53281*V)>>16;
	B=(76284*Y+132252*U)>>16;

	*(*out)=clip(R);
	*(*out+1)=clip(G);
	*(*out+2)=clip(B);
	*out+=3;
	
}
void Format::YUV2RGB_init(void)
{
	int i;

	/* init Lookup tables */
	for (i = 0; i < 256; i++) {
		ng_yuv_gray[i] = i * LUN_MUL >> 8;
		ng_yuv_red[i]  = (RED_ADD    + i * RED_MUL)    >> 8;
		ng_yuv_blue[i] = (BLUE_ADD   + i * BLUE_MUL)   >> 8;
		ng_yuv_g1[i]   = (GREEN1_ADD + i * GREEN1_MUL) >> 8;
		ng_yuv_g2[i]   = (GREEN2_ADD + i * GREEN2_MUL) >> 8;
	}
	for (i = 0; i < CLIP; i++)
		ng_clip[i] = 0;
	for (; i < CLIP + 256; i++)
		ng_clip[i] = i - CLIP;
	for (; i < 2 * CLIP + 256; i++)
		ng_clip[i] = 255;
}

void Format::enumerateFrameSizes(int fd)
{
	int ret;
	char buff[50];
	struct v4l2_frmsizeenum *frameSize= (v4l2_frmsizeenum*)calloc(1, sizeof(v4l2_frmsizeenum));

	frameSize->index = 0;
	frameSize->pixel_format = this->getType();
	
	ptracesfl("Enumerating Frame size ...", MT_INFO, FORMAT_TRACE);
	ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, frameSize);
	while ( ret == 0 ){
		switch( frameSize->type ){
			case V4L2_FRMSIZE_TYPE_DISCRETE:
				
				ptracesfl("Found: ", MT_INFO, FORMAT_TRACE + 1, false);
				strcpy(buff, "%hd x %hd @ \0");
				sprintf(buff, buff, frameSize->discrete.width, frameSize->discrete.height );
				ptracesfl(buff, MT_NONE, FORMAT_TRACE + 1, false);

				this->sizes.push_back( new ImageSize(frameSize->discrete.width, 
										 frameSize->discrete.height,
										 fd,
										 this->getType()) );
				ptracesfl(" \n", MT_NONE, FORMAT_TRACE + 1, false);					 
				break;
			case V4L2_FRMSIZE_TYPE_CONTINUOUS:
				// \TODO Enable support
				ptracesfl("Continuous frame size mode not Supported by sflphone at this moment.", MT_ERROR, FORMAT_TRACE); 
				break;
			case V4L2_FRMSIZE_TYPE_STEPWISE:
				// \TODO Enable support
				ptracesfl("Stepwise frame size mode not Supported by sflphone at this moment.", MT_ERROR, FORMAT_TRACE);
				break;
			default:
				break;			
		}
		
		frameSize->index++;
		
		ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, frameSize);
	}
	
	this->currentSize= this->sizes.begin()+1;
	
	free(frameSize);
	
}

ImageSize* Format::getCurrentImageSize(){
	
	if( this->currentSize == this->sizes.end() )
		return NULL;
		
	return (*this->currentSize);
	
} 

bool Format::setCurrentImageSize(  __u16 valueX, __u16 valueY )
{
	vector<ImageSize*>::iterator start= this->sizes.begin();
	vector<ImageSize*>::iterator end= this->sizes.end();
	
	while( start != end ){
		
		if( (*start)->getWidth() == valueX ){
			if( (*start)->getHeight() == valueY ){
				this->currentSize= start;
				return true;
			}
		}
		
		start++;
	}
	
	return false;
}

const char* Format::getAllSizes(){
	
	stringstream builder;
	
	for( int i= 0; i < this->sizes.size(); i++ ){
		builder << sizes[i]->getWidth() << "x" << sizes[i]->getHeight();
		if( i < this->sizes.size() - 1)
			builder << ";";
	}
	
	return builder.str().c_str();
}

