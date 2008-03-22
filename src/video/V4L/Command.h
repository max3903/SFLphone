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

#ifndef COMMAND_H
#define COMMAND_H

#include <semaphore.h>
#include "VideoDevice.h"

using namespace std;

#define COMMAND_TRACE	2

/** Structure that contains an enumeration of the critical values of a command
 */
struct CmdDesc{
	int Max;		/** Maximum Value*/
	int Min;		/** Minimum Value*/
	int Step;		/** Increment*/
	int Current;	/** Current value of the command*/
};

//! Command
/*!
 * This abstract class acts as an interface for multiple types of commands (to set and get proprieties of the video device)
 */ 
class Command {
public:

    //! Constructor
    Command();

    //! Destructor
    ~Command();

    //! Method to increase the value of the propriety
    /*!
     * \return a bool representing the success of the operation
     */
    virtual bool increase();

    //! Method to decrease the value of the propriety
    /*!
     * \return a bool representing the success of the operation
     */
    virtual bool decrease();

    //! Method to set the propriety to an exact value
    /*!
     * \return a bool representing the success of the operation
     */
    virtual bool setTo(__u16 value);

    //! Method to reset the propriety to its initial value
    /*!
     * \return a bool representing the success of the operation
     */
    virtual bool reset();
    
    //! Method that returns a description of the properties of the command
    /*!
     * Must be implemented by the child class
     * \return a CmdDesc that contains all the values of the device
     */
    virtual CmdDesc getCmdDescriptor()= 0;
    
    //! A static pointer to access to the actual VideoDevice
    static VideoDevice* videoDevice;
    
    //! Static method to notify all commands that the device will be changed
    /*!
     * This is a blocking method that reserves the device and waits for all commands to end.
     */
    static void ChangingDevice();
    
    //! Static method to notify all commands that the device was changed
    /*!
     * This method frees the device after calling ChangingDevice.
     */
    static void DeviceChanged();
    
protected:

	//! Method that must be called before accessing critical device properties
	void getVideoDeviceAccess();
	
	//! Method that must be called to release device after calling getVideoDeviceAccess
    void releaseVideoDevice();

	//! Static semaphore to access the device
	static sem_t AccessSem;
	
	//! Static semaphore to to see if divice is available
	static sem_t AvailSem;
	
	//!Static variable that accounts for the static semaphore initialisation
	static bool init;
    
};
#endif //COMMAND_H

