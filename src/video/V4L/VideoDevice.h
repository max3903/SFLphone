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

#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "Format.h"
#include "CopyMode.h"
#include "ReadMode.h"
#include "ConfigSet.h"

#define VIDEODEVICE_TRACE	1

class CaptureMode;

//! VideoDevice
/*!
 * This class uses V4L to mange the webcam: capture, access to webcam proprieties...
 */

class VideoDevice {
public:


    //! Constructor
    VideoDevice(const char* srcName);

    //! Destructor
    ~VideoDevice();

    //! Method to get the name of the video device
    /*!
     * \return the name of the video device
     */
    char* getName();
    
    //! Method to get the path to the video source (i.e. "/dev/video0" ).
    /*!
     * \return the name of the video source
     */
    char* getPath();

    //! Method to get file descriptor
    /*!
     * \return the file descriptor int
     */
    int getFileDescript();

    //! Method to initiate the device
    /*!
     * \param srcName the name of the video source
     * \return the file descriptor int
     */
    void initDevice(const char* srcName);    
    
    //! Method to open the video source (query camera)
    /*!
     * \return a bool representing the success of the opening of the video source 
     */
    bool openDevice();
 
    //! Method to close the video source
    /*!
     * \return a bool representing the success of the closing of the video source 
     */
    bool closeDevice();

    //! Method to get all video capabilities
    /*!
     * \return structure representing the video capabilities
     */
    v4l2_capability* getVideoCapability();

    //! Method to get video formats
    /*!
     * \return structure representing the video formats
     */
    v4l2_format* getVideoFormat();


    //! Method to set video formats
    /*!
     * \param videoFormat the attribute to change
     * \return a bool representing the success of parameters changing
     */
    bool setVideoFormat(v4l2_format* videoFormat);

	//! Method to get streaming information
    /*!
     * \return A pointer to a v4l2_streamparm structure
     */
	v4l2_streamparm* getStreamingParam();
	
	//! Method to set streaming information
    /*
     * \param A pointer to a v4l2_streamparm structure
     * \return a bool representing the success of parameters changing
     */
	bool setStreamingParam(v4l2_streamparm * streamParam);
    
    //! Method to call to change the resolution to a specific size
    /*!
     * \param valueX The X resolution
     * \param valueY The Y resolution
     * \return a bool representing the success of parameters changing
     */
    bool setResolution( __u16 valueX, __u16 valueY );
    
    //! Method to call to change the fps rate to a specific value
    /*!
     * Not all devices have the capacity of changing the FPS value 
     * \param fps The new fps rate
     * \return a bool representing the success of parameters changing
     */
    bool setFPS( int fps);
    
    //! Method to call to get the fps rate
    /*!
     * Not all devices have the capacity of enumerating the FPS value
     * \return The current Value of the FPS rate
     */
    int getFPS();

	//! Access Method to get the current Image Format
	/*!
	 * \return A pointer to a Format object
	 */
    Format* getFormat() const;

	//! Access Method to get the current Capture mode
	/*!
	 * \return A pointer to a CaptureMode object
	 */
    CaptureMode* getCaptureMode();    

	//! Method to get all the availlable Image formats
	/*!
	 * \return A vector containing all the Images Format supported by the device and sflphone
	 */
    vector<Format*> enumConfigs();

	//! Access method to the configuration set
	/*!
	 * \return The configuration set of the video device (see ConfigSet)
	 */
    ConfigSet* getConfigSet();

private:

    //! The name of the actual video source
    char* name;
    
    //! The name of the actual video source
    char* path;

    //! The actual file descriptor of the video source
    int fileDescript;

    //! The actual video formats (contains width and height)
    v4l2_format* videoFormat;

    //! The actual videoCapability (contains ..)
    v4l2_capability* videoCapability;

    //! The actual videoPicture parameters (contains ..)
    video_picture* videoPicture;
    
    v4l2_streamparm *streamingParam;
    
    CaptureMode* capMode;
    
    ConfigSet* config;
    
};
#endif //VIDEODEVICE_H

