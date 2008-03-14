/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
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

#ifndef MEMMANAGER_H_
#define MEMMANAGER_H_

#include <stdlib.h>

/**
 * A structure that eases the transfert of data trought the shared memory space
 */
typedef struct {
        char* data;		/** A pointer to the data it self*/
        int size;		/** The size of data (not necessarily the size of the shared memory space)*/
        int width;		/** The width of the image contained in the shared memory space*/
        int height;		/** The width of the image contained in the shared memory space*/
}MemData;

/**
 * A structure that contains all the releavent information about a shared memory space
 */
typedef struct
{
		int key;			/** The unique key associated with the shared memory space*/
		char* description;	/** A description of the shared memory space*/
		int size;			/** The maximumsize of the memory space*/
		char* BaseAdd;		/** The base address of the shared memory space*/
}MemKey;


//! Key to access the shared Memory space that refers to the remote data
MemKey* remoteKey;

//! Key to access the shared Memory space that refers to the local data
MemKey* localKey;

//! Remote data buffer
MemData* remoteBuff;

//! Local data buffer
MemData* localBuff;

/**
 * Creates a MemKey Structure from a serialized state see MemManager
 * @param key a char pointer to the serialized data
 * @return a MemKey structure
 */
MemKey* createMemKeyFromChar( char* key );

/**
 * Initializes a shared MemSpace from a MemKey strucure
 * @param a pointer to a MemKey structure
 * @return an updated MemKey
 */
MemKey* initSpace( MemKey *key );

/**
 * Function to get the data contained in the shared memory space
 * @param key the memory key associated with the shared memory space
 * @param data a pointer to a MemData structure that will contain the data contained in the shared memory space
 * @return Returns -1 in case of an error
 */ 
int fetchData( MemKey *key, MemData *data );

/**
 * Function to put data contained in the MemData strcuture in the shared memory space
 * @param key the memory key associated with the shared memory space
 * @param data a pointer to a MemData structure that contains the data to put the shared memory space
 * @return Returns -1 in case of an error
 */ 
int putData( MemKey *key, MemData *data );

/**
 * Intitializes the shred memory spaces
 * @param local A pointer to the local key in a caracter format
 * @param remote A pointer to the remote key in a caracter format
 * @ Returns the success of the operation
 */
int InitMemSpaces( char* local, char* remote );

/**
 * Destroys the memspaces
 */
int DestroyMemSpaces();

#endif /*MEMMANAGER_H_*/
