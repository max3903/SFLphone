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

#ifndef COPYMODE_H
#define COPYMODE_H

#include <sys/mman.h>

#include "CaptureMode.h"
#include "VideoDevice.h"

class VideoDevice;

#define MAXBUFFERS	4;

//! Structure that links memory map address to size of memory space
struct DataBuffers {
	void *start;
	size_t length;
};

//! CopyMode
/*!
 * This class capture from the video source using a copy mode
 */
class CopyMode : public CaptureMode {
public:

    //! Constructor
    CopyMode();

    //! Destructor
    ~CopyMode();

    //! Initializes the capture mode
    /*!
     * This method is called before the capture mode is used.
     * \param fd The file descriptor to the device
     * \return The succsess of the operation
     */
    virtual bool init( int fd );
    
    //! Shuts down the capture mode
    /*!
     * This method is called when the captude mode changes for an other.
     * \param fd The file descriptor to the device
     * \return The succsess of the operation
     */
    virtual bool close( int fd );
    
    //! Starts the capture
    /*!
     * This method is called when ready to start capture. Normally it is called by init(int fd), but it can be called at any time after stopCapture(int fd) is called.
     * \param fd The file descriptor to the device
     * \return The succsess of the operation
     */
    virtual bool startCapture(int fd);
    
    //! Stops the capture
    /*!
     * This method is called when stoppping the capture is needed. Normally it is called by the destructor, but it can be called at any time after startCapture(int fd) is called.
     * \param fd The file descriptor to the device
     * \return The succsess of the operation
     */
    virtual bool stopCapture(int fd);
        
    //! Method to capture from the video source
    /*!
     * \param device A pointer to the current video device
     * \return the capture data in RBG format
     */
     virtual unsigned char* capture( VideoDevice* device );
     
protected:

	/** A structure containing the dequeued buffer
	 */   
    struct v4l2_buffer fetchBuffer;
    
    /**The srtucture containing the request information
     */
    v4l2_requestbuffers *reqbuff;
    
    /** The array of buffers to queue
     */
    DataBuffers* buffers;
};
#endif //COPYMODE_H
