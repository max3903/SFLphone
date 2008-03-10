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

#ifndef MEMMANAGER_H
#define MEMMANAGER_H

#include <vector>
#include <iterator>
#include "MemSpace.h"
#include "MemSorter.h"
#include "MemData.h"
#include "MemSpace.h"
#include "MemKey.h"

using namespace std;

//! Shared memory pool manager
/*!
 * This class manages a pool of MemSpace, it make the link between the MemKey and the MemSpace. It also creates the MemKey Only one instance of the manager can exist at one time
 */
class MemManager {
public:

	//! Destructor
	/*!
	 * Disposes cleanly of the manager
	 */
    ~MemManager();

    //! Method to get the instance of the Manager.
    /*!
     * If it does not exist, this method will create it
     * 
     * \return an instance of the manager
     */
    MemManager* getInstance();

    //! Initialise a shared memory space with the specified size
    /*!
     * This method creates a new shared memory space and a new key.
     * \param size the size of the shared memory space
     * \param description is a description of the key to initialize
     * \return a MemKey that identifies the MemSpace and is necessary for future access
     */
    const MemKey* initSpace(int size,int width,int height);

    //! Initialise a shared memory space with the specified info
    /*!
     * This method creates a new shared memory space and create a new key or links to an existing one and returns the corresponding MemKey. To link 2 spaces, the size and the key muste be the same.
     * 
     * \param key the key identifiying the shared memory space
     * \param size the size of the shared memory space
     * \param description is a description of the key to initialize
     * \return a MemKey linking to a MemSpace dans that is needed for future access
     */
    const MemKey* initSpace(key_t key, int size,int width,int height);

    //! Initialise a shared memory space with the specified info
    /*!
     * This method creates a new shared memory space and create a new key or links to an existing one and returns the corresponding MemKey. To link 2 spaces, the size and the key muste be the same.
     * 
     * \param key the key identifiying the shared memory space
     * \return a MemKey linking to a MemSpace dans that is needed for future access
     */
    const MemKey* initSpace(MemKey* key);
    
    //! Delete a shared memory space with the specified info
    /*!
     * This method creates a new shared memory space and create a new key or links to an existing one and returns the corresponding MemKey. To link 2 spaces, the size and the key muste be the same.
     * 
     * \param key the key identifiying the shared memory space
     * \return bool true if the memspace was deleted, false otherwise
     */
    bool deleteSpace(MemKey* key);
    
    //! Sets the default MemSpace to work with
    /*!
     * \param key the key identifiying the MemSpace
     * \return a bool representing the succes of the operation
     */
    bool setDefaultSpace(MemKey* key);

    //! Sets the next MemSpace in the pool as the MemSpace to work with
    /*!
     * Works in a cirular manner, at the end of the pool the first one is referenced.
     */
    void nextSpace();

    //! Sets the previous MemSpace in the pool as the MemSpace to work with
    /*!
     * Works in a cirular maner, at the begining of the pool the last one is referenced.
     */
    void previousSpace();

    //! Returns the MemData from the current MemSpace
    /*!
     * \return the data from the MemSpace
     */
    MemData* fetchData();

    //! Returns the MemData from the MemSpace with the specifed key
    /*!
     * \param key the key of the MemSpace
     * \return the data from the MemSpace
     */
    MemData* fetchData(key_t key);

    //! Returns the MemData from the MemSpace with the specifed key
    /*!
     * \param key the key of the MemSpace
     * \return the data from the MemSpace
     */
    MemData* fetchData(MemKey* key);

    //! Changes the data in the current MemSpace
    /*!
     * \param Data a pointer to the new data
     * \param size the size of Data
     * \return the success of the operation
     */
    bool putData(char * Data, int size);

    //! Changes the data in the MemSpace specified by the key
    /*!
     * \param key the key of the MemSpace
     * \param Data a pointer to the new data
     * \param size the size of Data
     * \return the success of the operation
     */
    bool putData(int key, char * Data, int size);

    //! Changes the data in the MemSpace specified by the key
    /*!
     * \param key the key of the MemSpace
     * \param Data a pointer to the new data
     * \param size the size of Data
     * \return the success of the operation
     */
    bool putData(MemKey* key, char * Data, int size);

    //! Gets a list of the available MemKeys
    /*!
     * \return a vector of available MemKey
     */
    vector<MemKey*> getAvailSpaces(); 


private:
	
	//! Generates a random key to access the MemSpace
	/*!
	 * \return a generated key
	 */
    int genKey();
    
    //! The MemSpaces
    vector<MemSpace*> spaces;
   
    //! The instance of the manager
    static MemManager* instance;
    
    // The current index of the pool of MemSpace
    vector<MemSpace*>::iterator defaultIndex;
    
protected:
	
	//! Default constructor
	/*!
	 * Part of a singleton pattern
	 */
    MemManager();
    
};
#endif //MEMMANAGER_H
