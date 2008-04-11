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

#include "MemManager.h"

MemKey* createMemKeyFromChar( char* key )
{
	MemKey* theKey= (MemKey*)malloc(sizeof(MemKey));

	theKey->id= -1;
	theKey->key= -1;
	theKey->size= -1;

	sscanf(key,"%d %d %d", &theKey->id ,&theKey->key, &theKey->size );
	
	theKey->description= NULL;
	theKey->BaseAdd= NULL;
	
	return theKey;

}

MemKey* initSpace( MemKey *key )
{
	//int shmid;
	
	key->BaseAdd= NULL;
	key->BaseAdd= (char *)shmat(key->id, NULL, 0);
		
	if ( key->BaseAdd == NULL )
	{
		g_print("Error: Cannot create shared memory segment");
        return NULL;
	}
						
	return key;
}

int fetchData( MemKey *key, MemData *data )
{
	//\TODO: Add multiple access protection
			
	if( key == NULL )
		return -1;
	
	if( data == NULL )
		return -1;
		
	if( key->BaseAdd == NULL )
		return -1;
	
	/*if(data->data !=  NULL)
		free(data->data);*/
	
	// Get Image payload size
	memcpy(&data->size, key->BaseAdd, sizeof(int));
	
	if( data->size == 0 )
		return -1;
	
	// Get image width and height
	memcpy(&data->width, key->BaseAdd + sizeof(int), sizeof(int));
	memcpy(&data->height, key->BaseAdd + (sizeof(int) * 2), sizeof(int));
		
	// Get Image payload
	//data->data= (unsigned char*)calloc(1,data->size);
	memcpy(data->data, key->BaseAdd + (sizeof(int) * 3), data->size);
	
	// Reset shared memory buffer	
	//memset(key->BaseAdd,0, data->size + (sizeof(int) * 4) );
	//memset(key->BaseAdd,0, sizeof(int) );
		
 	return 0;
	
}

int putData( MemKey *key, MemData *data )
{
	//\TODO: Add multiple access protection
	if( data != NULL && key != NULL )
		memcpy(key->BaseAdd, &data->data, key->size);
	else
		return -1;
		
 	return 0;
}


int InitMemSpaces( char* local, char* remote )
{
	if( local != NULL)
	{
		localKey= createMemKeyFromChar( local );
		if(localKey == NULL)
			return -1;
		
		localBuff= (MemData*)calloc(1, sizeof(MemData));
		//localBuff->data= 0;
		if( localBuff == NULL)
			return -1;

		initSpace(localKey);
		
	}
	
	if( remote != NULL )
	{
		remoteKey= createMemKeyFromChar( remote );
		if(remoteKey == NULL)
			return -1;
		
		remoteBuff= (MemData*)calloc(1, sizeof(MemData));
		if( remoteBuff == NULL)
			return -1;
			
		initSpace(remoteKey);
	}
	
	return 0;
	
}

int DestroyMemSpaces()
{
	
	if( localKey != NULL )
	{
		if( localKey->BaseAdd != NULL && shmdt(localKey->BaseAdd) == -1)
		{
	    	perror("Error: Failed to detach shared memory segment");
	    	return -1;
		}
		
		if( localKey->description != NULL)
			free( localKey->description );
			
		free(localKey);
	}
 	
	if( remoteKey != NULL )
	{
		if( remoteKey->BaseAdd != NULL && shmdt(remoteKey->BaseAdd) == -1)
		{
	    	perror("Error: Failed to detach shared memory segment");
	    	return -1;
		}
		
		if( remoteKey->description != NULL)
			free( remoteKey->description );
			
		free(remoteKey);	
	}

	if( remoteBuff != NULL)
	{		
		free(remoteBuff);
	}
 	
	if( localBuff != NULL)
	{		
		free(localBuff);
	}
		
 	return 0;
	
}
