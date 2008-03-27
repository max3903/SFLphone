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

#include "CopyMode.h"

CopyMode::CopyMode(){
	this->buffers= NULL;
	this->reqbuff= NULL;
}

CopyMode::~CopyMode(){
	// \TODO: Assure clean destoy
}

bool CopyMode::init( int fd ){
  	
  	int ret;
  	char buff[100];

  	ptracesfl( "Initialising Copy mode",MT_INFO, CAPTUREMODE_INIT_TRACE);  	

	ptracesfl( "Creating buffers request",MT_INFO, CAPTUREMODE_INIT_TRACE +1); 
	this->reqbuff= (v4l2_requestbuffers*)calloc(1,sizeof(v4l2_requestbuffers));
	this->reqbuff->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	this->reqbuff->memory = V4L2_MEMORY_MMAP;
	this->reqbuff->count = MAXBUFFERS;
  	
  	ptracesfl( "Requesting buffers",MT_INFO, CAPTUREMODE_INIT_TRACE +1, false);
    ret = ioctl(fd, VIDIOC_REQBUFS, reqbuff);
    if (ret < 0 && this->reqbuff->count > 2) {
    	ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_INIT_TRACE +1);
		ptracesfl( "Unable to allocate buffers",MT_ERROR, CAPTUREMODE_INIT_TRACE );
		return false;
    }else
    	ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE +1);
        
    int tmp= MAXBUFFERS;
    sprintf(buff, "Creating %d allowed buffers of %d asked\0", this->reqbuff->count, tmp);
    ptracesfl( buff,MT_INFO, CAPTUREMODE_INIT_TRACE +1);
    this->buffers= (DataBuffers*)calloc( this->reqbuff->count, sizeof(DataBuffers));
    
    ptracesfl( "Mapping buffers ...",MT_INFO, CAPTUREMODE_INIT_TRACE +1);
    for (int i = 0; i < reqbuff->count; i++) {
    	
    	sprintf(buff, "\t#%d\0", i + 1);
    	
    	ptracesfl( "Initialising buffer ... ",MT_INFO, CAPTUREMODE_INIT_TRACE +2, false);
    	ptracesfl( buff,MT_NONE, CAPTUREMODE_INIT_TRACE +2,false);
    	ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE +2);
    	
		v4l2_buffer tmpBuff;
		memset (&tmpBuff, 0, sizeof (tmpBuff));
		tmpBuff.type = reqbuff->type;
		tmpBuff.memory = V4L2_MEMORY_MMAP;
		tmpBuff.index = i;
		
		ptracesfl( "Querrying buffer ...\t",MT_INFO, CAPTUREMODE_INIT_TRACE +2, false);
    	ptracesfl( buff,MT_NONE, CAPTUREMODE_INIT_TRACE +2, false);
		if ( ioctl (fd, VIDIOC_QUERYBUF, &tmpBuff) == -1 ) {
			ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_INIT_TRACE +2);
			ptracesfl( "Unable to query buffer",MT_ERROR, CAPTUREMODE_INIT_TRACE);
			return false;
		}else
			ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE +2);
		

		ptracesfl( "Mapping buffer ...\t",MT_INFO, CAPTUREMODE_INIT_TRACE +2, false);
		ptracesfl( buff,MT_NONE, CAPTUREMODE_INIT_TRACE +2, false);
		buffers[i].length = tmpBuff.length;
		buffers[i].start = mmap (NULL, tmpBuff.length, PROT_READ, MAP_SHARED, fd, tmpBuff.m.offset);

		if (MAP_FAILED == buffers[i].start) {
			ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_INIT_TRACE +2);
			ptracesfl( "Cannot map buffer to memory",MT_FATAL, CAPTUREMODE_INIT_TRACE);
			exit (-1);
		}else
			ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE +2);
		
	}
    	
    this->startCapture(fd);
	
	ptracesfl( "Copy mode initialised",MT_INFO, CAPTUREMODE_INIT_TRACE);
 		
  	return true;
  	
}
 
bool CopyMode::close( int fd ){
  	
  	char buff[100];
  	
  	ptracesfl( "Closing copy mode ...",MT_INFO, CAPTUREMODE_INIT_TRACE);
  	
  	ptracesfl( "Unmapping buffers",MT_INFO, CAPTUREMODE_INIT_TRACE  + 1);
  	for( int i= 0; i < this->reqbuff->count; i++ ){
    	sprintf(buff, "#%d\0", i + 1);
		ptracesfl( "Unmapping buffer ",MT_INFO, CAPTUREMODE_INIT_TRACE + 2, false);
		ptracesfl( buff,MT_NONE, CAPTUREMODE_INIT_TRACE + 2, false);
		ptracesfl(": ",MT_NONE, CAPTUREMODE_INIT_TRACE + 2, false);
		
  		if(munmap(buffers[i].start, buffers[i].length) == -1 ){
  			ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_INIT_TRACE + 2);
			ptracesfl( "Cannot unmap buffer",MT_ERROR, CAPTUREMODE_INIT_TRACE);
  		}else
  			ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE + 2);
  	}
  	  		
  	return true;
  	
}

unsigned char* CopyMode::capture(VideoDevice* device){
   
   char buff[100];
   
    ptracesfl( "Begining Copy Mode i/o capture:",MT_INFO, CAPTUREMODE_TRACE - 1);
    	
    int width= device->getVideoFormat()->fmt.pix.width;
	int height= device->getVideoFormat()->fmt.pix.height;
	
	ptracesfl( "Getting width and height of picture: ",MT_INFO, CAPTUREMODE_TRACE, false);
	strcpy(buff, "%hd x %hd\0");
    sprintf(buff, buff, width, height );
	ptracesfl( buff,MT_NONE, CAPTUREMODE_TRACE);
	
	ptracesfl( "Getting image buffer size: ",MT_INFO, CAPTUREMODE_TRACE, false);
	this->imageSize= width * height << 1;
	
    sprintf(buff, "%d\0", imageSize );
	ptracesfl( buff,MT_NONE, CAPTUREMODE_TRACE);
		
	ptracesfl( "Deqeuing buffer ...",MT_INFO, CAPTUREMODE_TRACE, false);
	memset(&fetchBuffer, 0, sizeof(struct v4l2_buffer));
    fetchBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fetchBuffer.memory = V4L2_MEMORY_MMAP;
       
    if( ioctl(device->getFileDescript(), VIDIOC_DQBUF, &fetchBuffer) == -1 ){
    	ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_TRACE);
    	ptracesfl( "Cannot dequeue buffer ...",MT_ERROR, CAPTUREMODE_TRACE);
    	return NULL;
    }else
    	ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_TRACE);
    
	ptracesfl( "Allocating memory for raw data buffer ...",MT_INFO, CAPTUREMODE_TRACE, false);
	unsigned char* raw_data= (unsigned char*)malloc(imageSize);
	
	if( raw_data == NULL ){
		ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_TRACE);
    	ptracesfl( "Unable to allocate space for buffer.",MT_ERROR, CAPTUREMODE_TRACE);
		return NULL;
	}else
		ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_TRACE);
	
    
	// Reteving the data from the web cam
	ptracesfl( "Reading data from webcam: ",MT_INFO, CAPTUREMODE_TRACE, false);
	if( fetchBuffer.bytesused > imageSize){
		memcpy(raw_data, buffers[ fetchBuffer.index ].start, imageSize);
		
    	sprintf(buff, "%d bytes\0",imageSize );
		ptracesfl( buff,MT_NONE, CAPTUREMODE_TRACE);
	}
	else{
		memcpy(raw_data, buffers[ fetchBuffer.index ].start, fetchBuffer.bytesused);

    	sprintf(buff, "%d bytes\0", fetchBuffer.bytesused );
		ptracesfl( buff,MT_NONE, CAPTUREMODE_TRACE);
	}
	
	if( ioctl(device->getFileDescript(), VIDIOC_QBUF, &fetchBuffer) == -1 ){
		ptracesfl( "Unable to requeue buffer",MT_ERROR, CAPTUREMODE_TRACE);
		return NULL;
    }
   
	// Allocate memory for the final image
	ptracesfl( "Allocating memory for RBG image buffer ... ",MT_INFO, CAPTUREMODE_TRACE, false);
	unsigned char* img_data = (unsigned char*)malloc(width*height*3*sizeof(unsigned char));
	
	if( img_data == NULL ){
		ptracesfl( "\tNO",MT_NONE, CAPTUREMODE_TRACE);
    	ptracesfl( "Unable to allocate space for buffer",MT_ERROR, CAPTUREMODE_TRACE);
		return NULL;
	}else
		ptracesfl( "\tOK",MT_NONE, CAPTUREMODE_TRACE);
	
	// Launch image format conversion
	ptracesfl( "Converting Data with ",MT_INFO, CAPTUREMODE_TRACE, false);
	ptracesfl( device->getFormat()->getStringType(), MT_NONE, CAPTUREMODE_TRACE);
	device->getFormat()->convert(raw_data, img_data, width, height);
	
	free(raw_data);
	
	ptracesfl( "End of Copy Mode i/o capture",MT_INFO, CAPTUREMODE_TRACE - 1);
    return img_data;

}
  
bool CopyMode::startCapture(int fd){
	
	char buff[100];
	ptracesfl( "Starting capture ...",MT_INFO, CAPTUREMODE_INIT_TRACE + 1);
	
	// Queue buffer
	ptracesfl( "Queueing buffers ... ",MT_INFO, CAPTUREMODE_INIT_TRACE + 2);
	for( int i= 0; i < this->reqbuff->count; i++ ){
		
		strcpy(buff, "#%d\0");
    	sprintf(buff, buff, i + 1);
		ptracesfl( "Queuing buffer ",MT_INFO, CAPTUREMODE_INIT_TRACE + 3, false);
		ptracesfl( buff,MT_NONE, CAPTUREMODE_INIT_TRACE + 3,false);
		ptracesfl(": ",MT_NONE, CAPTUREMODE_INIT_TRACE + 3,false);
		
		v4l2_buffer tmpBuff;
		memset (&tmpBuff, 0, sizeof (tmpBuff));
		tmpBuff.type = reqbuff->type;
		tmpBuff.memory = V4L2_MEMORY_MMAP;
		tmpBuff.index = i;
			
		if ( ioctl (fd, VIDIOC_QBUF, &tmpBuff) == -1 ) {
			ptracesfl("\tNO",MT_NONE, CAPTUREMODE_INIT_TRACE+3);
			ptracesfl( "Unable to queue buffer ",MT_ERROR, CAPTUREMODE_INIT_TRACE);
			return false;
		}else
			ptracesfl("\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE + 3);
	}
		
	ptracesfl( "Starting video stream: ",MT_INFO, CAPTUREMODE_INIT_TRACE + 2, false);
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if( ioctl(fd, VIDIOC_STREAMON, &type) ){
    	ptracesfl("\tNO",MT_NONE, CAPTUREMODE_INIT_TRACE+1);
		ptracesfl( "Unable to start streaming",MT_FATAL, CAPTUREMODE_INIT_TRACE);
		exit(-1);
    }else
    	ptracesfl("\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE + 2);
    	
    ptracesfl( "Capture Started",MT_INFO, CAPTUREMODE_INIT_TRACE + 1);
    	
    return true;
	
}
    
bool CopyMode::stopCapture(int fd){
	
	char buff[100];
	
	ptracesfl( "Stopping capture ...",MT_INFO, CAPTUREMODE_INIT_TRACE + 1);
	
	ptracesfl( "Stoppping video stream: ",MT_INFO, CAPTUREMODE_INIT_TRACE + 2, false);
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if( ioctl(fd, VIDIOC_STREAMOFF, &type) ){
    	ptracesfl("\tNO",MT_NONE, CAPTUREMODE_INIT_TRACE + 2);
		ptracesfl( "Unable to stop streaming",MT_WARNING, CAPTUREMODE_INIT_TRACE);
    }else
    	ptracesfl("\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE + 2);
    
    ptracesfl( "Dequeueing buffers ... ",MT_INFO, CAPTUREMODE_INIT_TRACE + 2);
    for( int i= 0; i < this->reqbuff->count; i++ ){
	    
	    strcpy(buff, "#%d\0");
    	sprintf(buff, buff, i + 1);
		ptracesfl( "Queuing buffer ",MT_INFO, CAPTUREMODE_INIT_TRACE + 3, false);
		ptracesfl( buff,MT_NONE, CAPTUREMODE_INIT_TRACE + 3, false);
		ptracesfl(": ",MT_NONE, CAPTUREMODE_INIT_TRACE + 3,false);
		
		memset(&fetchBuffer, 0, sizeof(struct v4l2_buffer));
	    fetchBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	    fetchBuffer.memory = V4L2_MEMORY_MMAP;
	       
	    if( ioctl(fd, VIDIOC_DQBUF, &fetchBuffer) == -1 ){
	    	ptracesfl("\tNO",MT_NONE, CAPTUREMODE_INIT_TRACE + 3);
			ptracesfl( "Unable to dequeue buffer",MT_WARNING, CAPTUREMODE_INIT_TRACE + 3);
	    }else
	    	ptracesfl("\tOK",MT_NONE, CAPTUREMODE_INIT_TRACE + 3);
    }
    
    ptracesfl( "Capture Stopped...",MT_INFO, CAPTUREMODE_INIT_TRACE + 1);
    
    return true;
    	
}
  
