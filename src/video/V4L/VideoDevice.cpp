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

#include "VideoDevice.h"

VideoDevice::VideoDevice(char* srcName){
     
    initDevice(srcName);
    
    if( !openDevice() )
    	throw("Error oppenning device");
  
}

VideoDevice::~VideoDevice(){
}

void VideoDevice::initDevice(char* srcName){
    
    ptracesfl("Initializing device ...", MT_INFO, VIDEODEVICE_TRACE);
    // initiate the path (i.e. "/dev/video0" )
    path = new char[strlen(srcName)+1];
    strcpy(path, srcName);
    
    ptracesfl("\tPath: ", MT_INFO, VIDEODEVICE_TRACE, false);
    ptracesfl(this->path, MT_NONE, VIDEODEVICE_TRACE);
    
    this->name= NULL;
    
    ptracesfl("Device Initialized ...", MT_INFO, VIDEODEVICE_TRACE);
	
}
  
bool VideoDevice::openDevice(){

	// open the webcam device, like a file, return a file descriptor
	ptracesfl("Opening video device ", MT_INFO, VIDEODEVICE_TRACE, false);
	ptracesfl(this->path, MT_NONE, VIDEODEVICE_TRACE, false);
	this->fileDescript = open(path, O_RDONLY);
	
	if(fileDescript < 0){
		ptracesfl(": \tNO", MT_NONE, VIDEODEVICE_TRACE);
		ptracesfl("Can't open device", MT_ERROR, VIDEODEVICE_TRACE);
		return false;	
	}else
		ptracesfl(": \tOK", MT_NONE, VIDEODEVICE_TRACE);

	this->videoCapability= (struct v4l2_capability*)calloc(1, sizeof(struct v4l2_capability));
	
	// fill the v4l2_capability struct by a ioctl call (control device)
	ptracesfl("Querying the camera for capabilities: ", MT_INFO, VIDEODEVICE_TRACE, false);
	if(ioctl(fileDescript, VIDIOC_QUERYCAP, videoCapability)==-1){
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE);
		ptracesfl("Can't query device's capabilities", MT_ERROR, VIDEODEVICE_TRACE);
		return false;
	}else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE);
	
	// Getting device name
	ptracesfl("\tName: ", MT_INFO, VIDEODEVICE_TRACE, false);
    ptracesfl(this->getName(), MT_NONE, VIDEODEVICE_TRACE);
    
    // Getting driver name
    ptracesfl("\tDriver: ", MT_INFO, VIDEODEVICE_TRACE, false);
    ptracesfl((char*)this->videoCapability->driver, MT_NONE, VIDEODEVICE_TRACE);
    
    // Getting bus info
    ptracesfl("\tBus Info: ", MT_INFO, VIDEODEVICE_TRACE, false);
    ptracesfl((char*)this->videoCapability->bus_info, MT_NONE, VIDEODEVICE_TRACE);

	// Check if the device is able to capture
	ptracesfl("\tIs capture supported: ", MT_INFO, VIDEODEVICE_TRACE, false);
	if ( (videoCapability->capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0) {
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE);
		ptracesfl("Capture not supported on device", MT_ERROR, VIDEODEVICE_TRACE);
		return false;
    }else
    	ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE);
    
    // Check what i/o mode is supported by the camera
    ptracesfl("\tChecking for supported i/o mode: ", MT_INFO, VIDEODEVICE_TRACE, false);
    if ((videoCapability->capabilities & V4L2_CAP_READWRITE)) {
	    this->capMode= new ReadMode();
	    ptracesfl("\tRead i/o", MT_NONE, VIDEODEVICE_TRACE);
    }else if((videoCapability->capabilities & V4L2_CAP_STREAMING)) {
    	this->capMode= new CopyMode();	
    	ptracesfl("\tCopy i/o", MT_NONE, VIDEODEVICE_TRACE);    
    }else{
    	ptracesfl("\tNone", MT_NONE, VIDEODEVICE_TRACE);
    	ptracesfl("Device does not support read i/o or streaming i/o!", MT_ERROR, VIDEODEVICE_TRACE);
    	return false;
    }
    
    // Initialising capture mode
    if( !this->capMode->init(this->fileDescript) )
    		return false;
      
	// Check Format configuration
	ptracesfl("Initialising configuration ... ", MT_INFO, VIDEODEVICE_TRACE);
	this->config= new ConfigSet(this->fileDescript);
	
	//Apply default configuration
	this->videoFormat= (struct v4l2_format*)calloc(1, sizeof(struct v4l2_format));
	
	ptracesfl("Getting current format information ... ", MT_INFO, VIDEODEVICE_TRACE, false);
	Format* tmp_format= config->getCurrentFormat();
	if( tmp_format == NULL){
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE);
		ptracesfl("No default format defined or supported", MT_ERROR, VIDEODEVICE_TRACE);
		return false;
	}
	else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE);
		
	ptracesfl("Applying default configuration ", MT_INFO, VIDEODEVICE_TRACE);
	videoFormat->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	videoFormat->fmt.pix.field= V4L2_FIELD_ANY;
	videoFormat->fmt.pix.height= tmp_format->getCurrentImageSize()->getHeight();
	videoFormat->fmt.pix.width= tmp_format->getCurrentImageSize()->getWidth();
	videoFormat->fmt.pix.pixelformat= tmp_format->getType();

	ptracesfl("\tQuering the camera to set video format: ", MT_INFO, VIDEODEVICE_TRACE , false);
	if(ioctl(fileDescript, VIDIOC_S_FMT, videoFormat)==-1){
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE);
		ptracesfl("Can't set the capture image format", MT_ERROR, VIDEODEVICE_TRACE);
		return false;
	}else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE);
	
    return true;
}

  bool VideoDevice::closeDevice(){

	ptracesfl("Stopping capture ... ", MT_INFO, VIDEODEVICE_TRACE, false);
	if( !this->capMode->close( this->fileDescript ) ){
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE);
		ptracesfl("Error while stoppping capture.", MT_ERROR, VIDEODEVICE_TRACE);
	}else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE);
	
	ptracesfl("Closing device ... ", MT_INFO, VIDEODEVICE_TRACE, false);
	if( close(this->fileDescript) ){
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE);
		ptracesfl("Error while closing device.", MT_ERROR, VIDEODEVICE_TRACE);
	}else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE);
	
    return true;
  }
  
v4l2_format* VideoDevice::getVideoFormat(){
    
   	v4l2_format *tmpFormat= (v4l2_format*)calloc(1, sizeof(v4l2_format));
  		
	tmpFormat->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  	
  	ptracesfl("Querying video device to get Video Format ... ", MT_INFO, VIDEODEVICE_TRACE + 9, false);	
	if(ioctl(fileDescript, VIDIOC_G_FMT, tmpFormat)==-1){
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE + 1);
		ptracesfl("Can't get the image format from video device.", MT_ERROR, VIDEODEVICE_TRACE + 9);
		return NULL;
	}else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE + 9);
		
	return tmpFormat;
}
  
bool VideoDevice::setVideoFormat(v4l2_format* videoFormat){
    
    if( videoFormat == NULL  )
    	return false;
    	
	ptracesfl("Querying video device to set Video Format ... ", MT_INFO, VIDEODEVICE_TRACE + 1, false);
	if(ioctl(fileDescript, VIDIOC_S_FMT, videoFormat)==-1){
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE + 1);
		ptracesfl("Can't set the image format.", MT_ERROR, VIDEODEVICE_TRACE + 1);
		return false;
	}else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE + 1);
	
	return true;

}
 
v4l2_streamparm* VideoDevice::getStreamingParam(){
	
	v4l2_streamparm *tmpSParam= (struct v4l2_streamparm*)calloc(1, sizeof(struct v4l2_streamparm));  
    tmpSParam->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	ptracesfl("Querying video device to get Streaming Params ... ", MT_INFO, VIDEODEVICE_TRACE + 1, false);
    if( ioctl(fileDescript, VIDIOC_G_PARM, tmpSParam) == -1){
    	ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE + 1);
		ptracesfl("Can't get the streaming parameters.", MT_WARNING, VIDEODEVICE_TRACE + 1);
    	return NULL;
    }else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE + 1);
    
    return tmpSParam;    
    
}
		
bool VideoDevice::setStreamingParam(v4l2_streamparm * streamParam){
	
	if( streamParam == NULL )
		return false;
	
	ptracesfl("Querying video device to set Streaming Params ... ", MT_INFO, VIDEODEVICE_TRACE + 1, false);
    if( ioctl(fileDescript, VIDIOC_S_PARM, streamingParam) == -1){
    	ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE + 1);
		ptracesfl("Can't set the streaming parameters.", MT_WARNING, VIDEODEVICE_TRACE + 1);
    	return false;
    }else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE + 1);
    
    return true;
    
}
  
v4l2_capability* VideoDevice::getVideoCapability(){

	v4l2_capability *tmpBuff= (struct v4l2_capability*)calloc(1, sizeof(struct v4l2_capability));
	
	// fill the v4l2_capability struct by a ioctl call (control device)
	ptracesfl("Querying the camera for capabilities ... ", MT_INFO, VIDEODEVICE_TRACE + 1, false);
	if(ioctl(fileDescript, VIDIOC_QUERYCAP, tmpBuff)==-1){
		ptracesfl("\tNO", MT_NONE, VIDEODEVICE_TRACE + 1);
		ptracesfl("Can't query device's capabilities.", MT_WARNING, VIDEODEVICE_TRACE + 1);
    	return NULL;
	}else
		ptracesfl("\tOK", MT_NONE, VIDEODEVICE_TRACE + 1);
	
	return tmpBuff;
	
}
  
  char* VideoDevice::getName(){
		
	if( this->name == NULL ){
		
		this->name= new char[ 50 ];
		
		sprintf(this->name, "%s", this->videoCapability->card);

  	}
  	
    return this->name;
  }
  
  char* VideoDevice::getPath(){
  	return this->path;
  }

  int VideoDevice::getFileDescript(){

    return this->fileDescript;
  }
  
  Format* VideoDevice::getFormat() const {
  	return this->config->getCurrentFormat(); 
  }
  
  CaptureMode* VideoDevice::getCaptureMode(){
  	return this->capMode;
  }
  
  vector<Format*> VideoDevice::enumConfigs(){
  	this->config->getFormats();
  }
  
  ConfigSet* VideoDevice::getConfigSet(){
  	return this->config;
  }
  
  bool VideoDevice::setResolution( __u16 valueX, __u16 valueY ){
  	
  	if( this->config->getCurrentFormat()->setCurrentImageSize( valueX, valueY ) ){
  	
  		v4l2_format *tmpFormat= this->getVideoFormat();
		
		tmpFormat->fmt.pix.height= valueY;
		tmpFormat->fmt.pix.width= valueX;
			
		this->setVideoFormat(tmpFormat);
		
		free(tmpFormat);
		
		return true;
		
  	}
  	
  	return false;
  }
  
   bool VideoDevice::setFPS( int fps){
   	   		
   		if( this->config->getCurrentFormat()->getCurrentImageSize()->setCurrent(fps) ){
   			
   			v4l2_streamparm* tmpSParam = (v4l2_streamparm*)calloc(1,sizeof(v4l2_streamparm)); //this->getStreamingParam();
   			if ( tmpSParam != NULL ){
	   			
	   			tmpSParam->parm.capture.capability= V4L2_CAP_TIMEPERFRAME;
	   			tmpSParam->parm.capture.capturemode= V4L2_MODE_HIGHQUALITY;
	    		tmpSParam->parm.capture.timeperframe.numerator= 1;
	    		tmpSParam->parm.capture.timeperframe.denominator= this->config->getCurrentFormat()->getCurrentImageSize()->getCurrentFps();
	    		
	    		if( this->setStreamingParam(tmpSParam) )
	    			return true;
	    			
   			}
   		}
   		
   		return false;
   		
   }
