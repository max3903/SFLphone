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

#ifndef CAPTUREMODE_H
#define CAPTUREMODE_H

class VideoDevice;

using namespace std;

#define CAPTUREMODE_TRACE		10
#define CAPTUREMODE_INIT_TRACE	1

//! CaptureMode
/*!
 * This abstract class acts as an interface for multiple types of capture mode
 */
class CaptureMode {
public:

    //! Constructor
    CaptureMode();

    //! Destructor
    ~CaptureMode();
    
    //! Initializes the capture mode
    /*!
     * Must be implemented by the child class. This method is called before the capture mode is used.
     * \param fd The file descriptor to the device
     * \return the success of the operation 
     */
    virtual bool init(int fd)= 0;
    
    //! Shuts down the capture mode
    /*!
     * Must be implemented by the child class. This method is called when the captude mode changes for an other.
     * \param fd The file descriptor to the device
     * \return the success of the operation 
     */
    virtual bool close(int fd)= 0;
    
    //! Starts the video device for capture
    /*!
     * Must be implemented by the child class. This method is called when ready to start capture. Normally it is called by init(int fd), but it can be called at any time after stopCapture(int fd) is called.
     * \param fd The file descriptor to the device
     * \return the success of the operation 
     */
    virtual bool startCapture(int fd)= 0;
    
    //! Stops the video device for capture
    /*!
     * Must be implemented by the child class. This method is called when stoppping the capture is needed. Normally it is called by the destructor, but it can be called at any time after startCapture(int fd) is called.
     * \param fd The file descriptor to the device
     * \return the success of the operation 
     */
    virtual bool stopCapture(int fd)= 0;

    //! The capture method
    /*!
     * Must be implemented by the child class
     * \param device A pointer to the current video device
     * \return the capture data in RBG format
     */
    virtual unsigned char* capture(VideoDevice* device)=0;
	
};
#endif //CAPTUREMODE_H

