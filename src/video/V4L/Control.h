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

#ifndef CONTROL_H_
#define CONTROL_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "../../tracePrintSFL.h"

using namespace std;

#define CONTROL_TRACE	9

/** A class that encapsulate an integer type control of the VideoDevice
 */
class Control
{
public:

	/** Constructor
	 * @param id The id of the control as returned by VIDIOC_QUERYCTRL ioctl call
	 * @param fd The file descriptor to the device
	 */
	Control( int id, int fd );
	
	/** Destructor
	 */
	virtual ~Control();
	
	/** Access Method for the Name of the Control
	 * @return The name of the Control
	 */
	char* getName();
	
	/** Access Method for the Minimum value of the Control
	 * @return The Minimum value of the Control
	 */
	int getMin();
	
	/** Access Method for the Maximum value of the Control
	 * @return The Maximum value of the Control
	 */
	int getMax();
	
	/** Access Method for the Step value of the Control
	 * @return The Step value of the Control
	 */
	int getStep();
	
	/** Access Method for the current value of the Control
	 * @return The current value of the Control
	 */
	int getValue();
	
	/** Method to set the value of the Control
	 * @param value The value to set the Control to.
	 * @return The sucess of the operation
	 */
	bool setValue( int value );
	
	/** Method to increment the value of the Control 
	 * 
	 * Follows the step value of the Control. If the Maximum is reached, the value is not augmented and false is returned
	 * @return The sucess of the operation
	 */
	bool increment();
	
	/** Method to decrement the value of the Control 
	 * 
	 * Follows the step value of the Control. If the Minimum is reached, the value is not decremented and false is returned
	 * @return The sucess of the operation
	 */
	bool decrement();
	
	
	/** Method to reset the Control to it's default value
	 */
	bool reset();
	
	/** Enum listing Control type supported by Sflphone
	 */
	enum TControl{ BRIGHTNESS= 	0x1,	/**Brightness control*/
				   COLOR= 		0x2,	/**Hue control*/
				   CONTRAST=	0x3		/**Contrast control*/
				    };
	
private:

	/** Method to query the device
	 * 
	 * Returns the Contols property, including current value see v4l2_queryctrl structure for more information.
	 * \return The structure containing all the information on the control
	 */
	v4l2_queryctrl* queryDevice();

	/** Default constructor
	 * 
	 * You cannot create a control without a file descriptor and an Id.
	 */
	Control();

	/** The Id of the Control
	 */
	int id;
	
	/** The file descriptor to the device
	 */
	int fd;
	
};

#endif /*CONTROL_H_*/
