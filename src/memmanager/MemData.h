/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean-Francois Blanchard-Dionne <jean-francois.blanchard-dionne@polymtl.ca>
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

#ifndef MEMDATA_H
#define MEMDATA_H

#include <string>
#include "../tracePrintSFL.h"
//! Memmory data container
/*!
 * Contains a copy of the data in the shared memory. It contains the data it self and the size. This class acts like a container.
 */
class MemData {
public:

	//! Constructor
	/*!
	 * Initialises the Container
	 */
    MemData();

    //! Pointer copy constructor
    /*!
     * Creates an independant copy of the source object
     * 
     * \param data a pointer to a MemData object
     */
    MemData(MemData* data);

    //! Reference copy constructor
    /*!
     * Creates an independant copy of the source object
     * 
     * \param data a reference to a MemData object
     */
    MemData(MemData& data);

    //! Destructor
    /*!
     * Disposes cleanly of the data pointer
     */
    ~MemData();

    //! Method to access the data in the buffer
    /*!
     * \param data a pointer to where the data will be copied
     * \return the size of data
     */
    int fetchData(char * data);

    //! Method to change the data in the buffer
    /*!
     * \param data a pointer to the new data
     * \param size the size of data
     */
    void putData(char * data, int size);
    
    
    //! Method to get the data in the buffer
    /*!
     * Method get the data
     */
     char* getData();
     
     //! Method to set the data in the buffer
    /*!
     * Method set the data
     */
     void setData(char * data);
        
private:
	
	 //! Pointer to the data
    char *data;
    //! The current size of the data in the buffer
    int size;
    //! The current size of the data in the buffer
    int width;
    //! The current size of the data in the buffer
    int height;
    
};
#endif //MEMDATA_H
