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

#include "ReadMode.h"

ReadMode::ReadMode(){}

ReadMode::~ReadMode(){}

bool ReadMode::init(int fd){
  	// \ NOTE: This method is not used in the capture mode
  	return true;
}
 
bool ReadMode::close(int fd){
  	// \ NOTE: This method is not used in the capture mode
  	return true;
}
  
bool ReadMode::startCapture(int fd){
	// \ NOTE: This method is not used in the capture mode
	return true;
}    

bool ReadMode::stopCapture(int fd){
	// \ NOTE: This method is not used in the capture mode
    return true;
}
  
unsigned char* ReadMode::capture(VideoDevice* device){
	
	char buff[100];
	v4l2_format* vFormat = device->getVideoFormat();

  	ptracesfl( "Begining Read Mode i/o capture:",MT_INFO, CAPTUREMODE_TRACE - 1); 
	
	ptracesfl( "Getting image buffer size: ",MT_INFO, CAPTUREMODE_TRACE, false);
	
	int imageSize= vFormat->fmt.pix.sizeimage;
	
    sprintf(buff, "%u\0", imageSize );
	ptracesfl( buff,MT_NONE, CAPTUREMODE_TRACE);
		
	ptracesfl( "Getting width and height of picture: ",MT_INFO, CAPTUREMODE_TRACE, false);
	
	int width= vFormat->fmt.pix.width;
	int height= vFormat->fmt.pix.height;
	
    sprintf(buff, "%hd x %hd\0", width, height );
	ptracesfl( buff,MT_NONE, CAPTUREMODE_TRACE);
	
	// Initiating raw_data buffer
	ptracesfl( "Allocating memory for raw data buffer: ",MT_INFO, CAPTUREMODE_TRACE, false);
	unsigned char* raw_data= (unsigned char*)malloc(imageSize);
	
	if( raw_data == NULL ){
		ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_TRACE);
    	ptracesfl( "Unable to allocate space for buffer.",MT_ERROR, CAPTUREMODE_TRACE);
		return NULL;
	}else
		ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_TRACE);
	
	// Reteving the data from the web cam
	ptracesfl( "Reading data from webcam: ",MT_INFO, CAPTUREMODE_TRACE, false);
	int n = read( device->getFileDescript(), raw_data, imageSize);
	
	if(n==-1){
		printf("\nError, can't read the webcam\n");
		ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_TRACE);
    	ptracesfl( "Can't read the webcam.",MT_ERROR, CAPTUREMODE_TRACE);
		return NULL;
	}else{
    	sprintf(buff, "\tOK %d bytes readed\0", n );
		ptracesfl( buff,MT_NONE, CAPTUREMODE_TRACE);		
	}
	
	// Allocate memory for the final image
	ptracesfl( "Allocating memory for rbg image buffer: ",MT_INFO, CAPTUREMODE_TRACE, false);
	unsigned char* img_data = (unsigned char*)malloc(width*height*3*sizeof(unsigned char));
	
	if( img_data == NULL ){
		ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_TRACE);
    	ptracesfl( "Unable to allocate space for buffer.",MT_ERROR, CAPTUREMODE_TRACE);
		return NULL;
	}else
		ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_TRACE);
	
	// launch image format conversion
	ptracesfl( "Converting Data with ",MT_INFO, CAPTUREMODE_TRACE, false);
	ptracesfl( device->getFormat()->getStringType(), MT_NONE, CAPTUREMODE_TRACE);
	device->getFormat()->convert(raw_data, img_data, width, height);
	
	free(raw_data);
	
	free(vFormat);
	
	ptracesfl( "End of read Mode i/o capture",MT_INFO, CAPTUREMODE_TRACE - 1);
	
    return img_data;
    
}
