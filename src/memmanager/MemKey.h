/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean-Francois Blanchard-Dionne <jean-francois.blanchard-dionne@polymtl.ca>
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

#ifndef MEMKEY_H
#define MEMKEY_H

#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string>

//! Represent a unique key associated to a shared memory space
/*!
 * This class is a container for the information relative to a shared memory space
 */
class MemKey {
public:

	//! Constructor
	/*!
	 * Initiate the object
	 * 
	 * \param size the maximum size of the shared memory space
	 * \param description a description of the shared memory space
	 */
    MemKey(int size);
	//! Constructor
	/*!
	 * Initiate the object
	 * 
	 * \param size the maximum size of the shared memory space
	 * \param description a description of the shared memory space
	 * \param key is to set the key
	 */
    MemKey(int size,key_t key);
    //! Pointer copy constructor
    /*!
     * Creates an independant copy of the source object
     * 
     * \param key a pointer to a MemKey object
     */
    MemKey(MemKey* key);
   
    //! Reference copy constructor
    /*!
     * Creates an independant copy of the source object
     * 
     * \param key a reference to a MemKey object
     */
    MemKey(MemKey& key);
    
    //! Constructor from serialized data
    /*!
     * \param serializedData a char pointer to the representation of a MemKey
     */
    MemKey(char* serializedData);

    //! Destructor
    /*!
     * Disposes cleanly of object
     */
    ~MemKey();

    //! Access method to the key of the shared memory space
    /*!
     * \return the key to the memory space
     */
    key_t getKey();
    
    //! Access method to set the key of the shared memory space
    /*!
     */
    void setKey(key_t key);
    
    
 

    //! Access method to the description of the shared memory space
    /*!
     * \return a constant char pointer to the description
     */
    const char * getDescription() const;

    //! Access method to the index of the memory space in the MemManager
    /*!
     * \return the index in the MemManager
     */
    int getIndex();

    //! Modification method to the index of the memory space in the MemManager
    /*!
     * \param index the index in the MemManager
     */
    void setIndex(int index);

    //! Access method to the maixmum size of shared memory space
    /*!
     * \return the maximum size of the shared memory space
     */
    int getSize();
    
    //! Modification method to the size of the memory space in the MemManager
    /*!
     * \param index the index in the MemManager
     */
    void setSize(int size);

    //! Method to serialize the MemKeyObject
    /*!
     * \return a char pointer reprensenting the state of the key
     */
    char * serialize();

private:
	
	//! Default constructor
	/*!
	 * The default constructor is declared private to prevent the decleration of an MemKey without the proper information.
	 */
    MemKey();
    //! Random key Generator
	/*!
	 * Will create a key if needed
	 */
    int genKey();
    
    //! The key to access the shared memory space
    key_t key;
    
    
    //! The description of the shared memory space
    char * description;
    
    //! The index of the MemSpace in the MemManager
    int index;
    
    //! The maximum size of the shared memory space
    int size;
    
    
};
#endif //MEMKEY_H
