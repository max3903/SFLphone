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

#ifndef VIDEODEVICEMANAGER_H
#define VIDEODEVICEMANAGER_H

#include <string>
#include <dbus/dbus.h>
#include <hal/libhal.h>

#include "Command.h"
#include "Colour.h"
#include "Resolution.h"
#include "Brightness.h"
#include "Capture.h"
#include "Contrast.h"
#include "VideoDevice.h"

class VideoDevice;

//! VideoDeviceManager
/*!
 * This class manages the video device, it make the link between the device and his properties objects (brightness, luminosity...). It also creates the DeviceManger object and it can change it too. Only one instance of the manager can exist at one time
 */
class VideoDeviceManager {
public:
	
	/** Enum that lists the commands supported by the framework
	 * */
	enum TCommand{	CONTRAST= 	0x1,	/** Command to change the Contrast*/
					BRIGHTNESS= 0x2,	/** Command to change the Brightness*/
					COLOR= 		0x3,	/** Command to change the Hue*/
					CAPTURE=	0x4,	/** Command to Capture from the device*/
					RESOLUTION= 0x5		/** Command to change image formats*/		
					};

    //! Constructor.
    VideoDeviceManager();

    //! Destructor
    ~VideoDeviceManager();

    //! Method to get a new command (propriety)
    /*!
     * This method creates a new command (a propriety of the video device) and returns a pointer to this command 
     * 
     * \param ref the reference of the command
     * \return a pointer to the created command
     */
    Command* getCommand(TCommand ref);

    //! Method to create a new VideoDevice.
    /*!
     * \param srcName the name of the camera that the new VideoDevice will use as a source.
     * \return the success of the operation
     */
   bool createDevice( const char* srcName);

    //! Method to change the VideoDevice.
    /*!
     * \param srcName the pointer to a char array containing the path to the device
     * \return a bool representing the success of the VideoDevice change
     */
    bool changeDevice( const char* srcName);

    //! Method to get the instance of the VideoDeviceManager.
    /*!
     * If it does not exist, this method will create it
     * 
     * \return an instance of the VideoDeviceManager
     */
    static VideoDeviceManager* getInstance();
    
    //! Method to enumerate all video device connected to the system
    /*!
     * \return A vector containing all the video devices
     */
    vector<string> enumVideoDevices();
    
    //! Method to close the Video device manager.
    /*!
     * Cleans up the Video Device Manager, Its destroys the Video Device dans puts the Command:videoDevice to NULL. So commands will still work.
     */
    void Terminate();

private:

    //! The instance of the VideoDeviceManager
    static VideoDeviceManager* instance;

};
#endif //VIDEODEVICEMANAGER_H
