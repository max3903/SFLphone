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

#include "ImageSize.h"

ImageSize::ImageSize(){}

ImageSize::ImageSize(int w, int h, int fd, int imgFormat){
	
	this->width= w;
	this->height= h;
	
	this->init_enumerateFPS(fd, imgFormat);
	
}

ImageSize::~ImageSize(){}

int ImageSize::getWidth(){
	return this->width;
}

int ImageSize::getHeight(){
	return this->height;
}

vector<int> ImageSize::getFps(){ 
	return this->fps; 
}

const char* ImageSize::getFpsString(){
	
	stringstream builder;
	
	for( int i= 0; i < this->fps.size(); i++ ){
		builder << fps[i];
		if( i < this->fps.size() - 1)
			builder << ";";
	}
	
	return builder.str().c_str();
	
}

bool ImageSize::isFPSSupported(int fps){
	
	vector<int>::iterator resultat= find( this->fps.begin(), this->fps.end(), fps );
	
	if( fps == (*resultat) )
		return true;
	 
	return false;
}

void ImageSize::init_enumerateFPS(int fd, int imgFormat){
	
	char buff[50];
	struct v4l2_frmivalenum *FrameIntervals= (struct v4l2_frmivalenum *)calloc(1, sizeof(v4l2_frmivalenum));

	FrameIntervals->index = 0;
	FrameIntervals->pixel_format = imgFormat;
	FrameIntervals->width = this->width;
	FrameIntervals->height = this->height;
	int min, max, step;
	
	int ret= ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, FrameIntervals);
	while ( ret == 0 ) {
		switch(FrameIntervals->type){
			case V4L2_FRMIVAL_TYPE_DISCRETE:	// Only specified FPS mode supported
				
				strcpy(buff, "%d/%d ");
				sprintf(buff, buff, FrameIntervals->discrete.numerator, FrameIntervals->discrete.denominator);
				ptracesfl(buff,MT_NONE, IMAGESIZE_TRACE, false);
				
				this->fps.push_back(FrameIntervals->discrete.denominator/FrameIntervals->discrete.numerator);
				break;
			case V4L2_FRMIVAL_TYPE_STEPWISE:	// FPS modes between max and min with step supported
				min= FrameIntervals->stepwise.min.denominator/FrameIntervals->stepwise.min.numerator;
				max= FrameIntervals->stepwise.max.denominator/FrameIntervals->stepwise.max.numerator;
				step= FrameIntervals->stepwise.step.denominator/FrameIntervals->stepwise.step.numerator;
				
				strcpy(buff, "Stepwise -> Min: %d/%d Max: %d/%d Step: %d/%d ");
				sprintf(buff, buff, FrameIntervals->stepwise.min.numerator,
									FrameIntervals->stepwise.min.denominator,
									FrameIntervals->stepwise.max.numerator,
									FrameIntervals->stepwise.max.denominator,
									FrameIntervals->stepwise.step.numerator,
									FrameIntervals->stepwise.step.denominator);
				ptracesfl(buff,MT_INFO, IMAGESIZE_TRACE + 1, false);
				
				for( min ; min <= max ; min + step )
					this->fps.push_back( min );
				break;
			case V4L2_FRMIVAL_TYPE_CONTINUOUS:	// FPS modes between max and min
				min= FrameIntervals->stepwise.min.denominator/FrameIntervals->stepwise.min.numerator;
				max= FrameIntervals->stepwise.max.denominator/FrameIntervals->stepwise.max.numerator;
				
				strcpy(buff, "Continuous -> Min: %d/%d Max: %d/%d ");
				sprintf(buff, buff, FrameIntervals->stepwise.min.numerator,
									FrameIntervals->stepwise.min.denominator,
									FrameIntervals->stepwise.max.numerator,
									FrameIntervals->stepwise.max.denominator);
				ptracesfl(buff,MT_INFO, IMAGESIZE_TRACE + 1, false);
				
				for( min ; min <= max ; min++ )
					this->fps.push_back( min );
				break;
			default:
				break;
		} 
		
		FrameIntervals->index++;
	
		ret= ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, FrameIntervals);
			
	}
	
	currentFps= this->fps.begin();
	
	free(FrameIntervals);
	
}

int ImageSize::getCurrentFps(){
	if( this->currentFps == this->fps.end() )
		return -1;
	
	return (*this->currentFps);
}

bool ImageSize::setCurrent(int fps){
	vector<int>::iterator start= this->fps.begin();
	vector<int>::iterator end= this->fps.end();
	
	while(start != end){
		if( (*start) == fps ){
			this->currentFps= start;
			return true;
		}
			
		start++;
	}
	
	return false;
	
}
