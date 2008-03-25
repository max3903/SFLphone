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

#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <utility>

#include "Command.h"

using namespace std;

//! Resolution
/*!
 * This class is used to set and get the resolution of the video device
 */ 
class Resolution : public Command {
public:

    //! Constructor
    Resolution();

    //! Destructor
    ~Resolution();

    //! Method to get the resolution value
    /*!
     * \return a pair element with Width as first and Height as second
     */
    virtual pair<int,int> getResolution(); 
         
    //! Method to set the resolution to an exact value 
    /*!
     * \param valueX The X value of the new resolution
     * \param valueY The Y value of the new resolution
     * \return a bool representing the success of the operation
     */
    virtual bool setTo(__u16 valueX, __u16 valueY);
    
    //! Method to set the resolution to an exact value 
    /*!
     * \param resolution A char pointer to the resolution in format "valueX x valueY"
     * \return a bool representing the success of the operation
     */
    virtual bool setTo( char* resolution);
       
    //! Method to set the fps rate to an exact value 
    /*!
     * \param fps The new value of the fps rate
     * \return a bool representing the success of the operation
     */
    virtual bool setFpsTo( int fps );

    //! Method to get all the resolution supported by the current image format
    /*!
     * \return A char pointer to a list of supported resolution seperated by ;
     */
    virtual const char* enumResolution();
    
    //! Method to get all the fps supported by the current Resolution
    /*!
     * \return A char pointer to a list of supported fps rate seperated by ;
     */
    virtual const char* enumFPS();
   
    //! Method that returns a description of the properties of the command
    /*!
     * \return a CmdDesc that contains all the values of the device
     */
    virtual CmdDesc getCmdDescriptor(){}
        
};
#endif //RESOLUTION_H
