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

#ifndef CAPTURE_H
#define CAPTURE_H

#include <vector>

#include "Command.h"
#include "CopyMode.h"
#include "ReadMode.h"

using namespace std;

//! Capture
/*!
 * This class is used to get a capture from the video device
 */ 
class Capture : public Command {
public:

    //! Constructor
    Capture();

    //! Destructor
    ~Capture();

    //! Method to capture from the video source
    /*!
     * \param size The size (in bytes) of the image 
     * \return a pointer to the captured data
     */
    unsigned char* GetCapture( int& size);

private:
    
    //! Method that returns a description of the properties of the command
    /*!
     * This method is not supported by this Command. It returns a CmdDesc filled with -1.
     * \return a CmdDesc that contains all the values of the device
     */
    virtual CmdDesc getCmdDescriptor();

};
#endif //CAPTURE_H

