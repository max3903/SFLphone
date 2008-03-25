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

#ifndef COLOUR_H
#define COLOUR_H

#include "Command.h"

//! Luminosity
/*!
 * This class is used to set and get the colour value of the video device
 */ 
class Colour : public Command {
public:

    //! Constructor
    Colour();

    //! Destructor
    ~Colour();

    //! Method to increase the value of the colour
    /*!
     * \return a bool representing the success of the operation
     */
    virtual bool increase();


    //! Method to decrease the value of the colour
    /*!
     * \return a bool representing the success of the operation
     */
    virtual bool decrease();


    //! Method to set the colour to an exact value
    /*!
     * \param value that to witch the hue will be set
     * \return a bool representing the success of the operation
     */
    virtual bool setTo(__u16 value);


    //! Method to reset the colour to its initial value
    /*!
     * \return a bool representing the success of the operation
     */
    virtual bool reset();


    //! Method to get the colour value
    /*!
     * \return the value of the hue
     */
    int getColour();
    
    //! Method that returns a description of the properties of the command
    /*!
     * \return a CmdDesc that contains all the values of the device
     */
    virtual CmdDesc getCmdDescriptor();
};
#endif //COLOUR_H
