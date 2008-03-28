/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean-Francois Blanchard-Dionne <jean-francois.blanchard-dionne@polymtl.ca>
 *  Author: Jean Tessier <jean.tessier@polymtl.ca>                                                                            
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

#ifndef MEMSPACE_H
#define MEMSPACE_H


#include "MemData.h"

class MemKey;
//! Shared memory space frontend
/*
 *  This class hides the fucntionnalities of the shared memory management.
 */
class MemSpace {
public:

	//! Constructor
	/*!
	 * Create a new shared memory space or links to an existing one
	 * 
	 * \param key the key identifiying the shared memory space
	 */
    MemSpace(MemKey* key);

    //! Pointer copy constructor
    /*!
     * This constructor \b does \b not create a new MemSpace, it copie the base address dans the key.
     * 
     * \param space a pointer to a reference MemSpace
     */ 
    MemSpace(MemSpace* space);

    //! Reference copy constructor
    /*!
     * This constructor \b does \b not create a new MemSpace, it copie the base address dans the key.
     * 
     * \param space a reference to a reference MemSpace
     */ 
    MemSpace(MemSpace& space);

    //! Destructor
    /*!
     * Unlinks the shared memory space from this process.
     */
    ~MemSpace();

    //! Returns the key associated with this MemSpace
    /*!
     * \return a MemKey containing the information for this MemSpace
     */
    MemKey* getMemKey();

    //! Changes the data in the MemSpace
    /*!
     * \param Data a pointer to the new data
     * \param size the size of the new data
     * \return the success of the operation
     */
    bool putData(unsigned char* Data, int size, int width, int height);

    //! Gets the data in the Shared Memory
    /*!
     * \return the data contained int the shared memory in the form of a MemData
     */
    MemData* fetchData();
    
     //! Returns the baseAddress associated with this MemSpace
    /*!
     * \return a MemKey containing the information for this MemSpace
     */
    unsigned char* getBaseAddress();
   //! Sets the char * baseAddress associated with this MemSpace
    /*!
     * \param Address containing the information to set the baseAddress
     */
    void setBaseAddress(unsigned char *Address);
    
    void setKey( MemKey* key );
    

private:
	

    // Default constructor
    /*!
     * The default constructor is declared private to prevent the declaration of a MemSpace without a MemKey
     */
    MemSpace();
    
    //! The base address of the shared memory space
    unsigned char * baseAddress;
    
    //! The Memkey Associated with this MemSpace
    MemKey* theKey;
    
};
#endif //MEMSPACE_H
