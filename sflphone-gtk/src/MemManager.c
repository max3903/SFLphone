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
	int lentgh= strlen(key);
	int cIndex= 0;
	MemKey* theKey= (MemKey*)malloc(sizeof(MemKey));
	char *tmp;
	int i= 0;
	
	for( i= 0; i < lentgh; i++  )
	{
		if( key[i] == ' ')
		{
			tmp= (char*)malloc( i-cIndex );
			memcpy( tmp, key, i-cIndex );
			theKey->key= atoi(tmp);
			free(tmp);
			cIndex= i+1;
			break;
			
		}
	}
	
	tmp= (char*)malloc( lentgh-cIndex );
	memcpy( tmp, key, lentgh-cIndex );
	theKey->size= atoi(tmp);
	free(tmp);
	
	theKey->description= NULL;
	theKey->BaseAdd= NULL;
	
	return theKey;
}

MemKey* initSpace( MemKey *key )
{
	int shmid;
	
	// \ TODO: Only need to attach ?
	if ( (shmid = shmget(key->key, key->size, IPC_CREAT | 0666)) < 0)
	{
		g_print("Error: Cannot create shared memory segment");
        return NULL;
	}
	
	key->BaseAdd= (char *)shmat(shmid, NULL, 0);
	
	if( key->BaseAdd == NULL )
	{
		g_print("Error: Cannot attach shared memory segment");
		return NULL;
	}
				
	return key;
}

int fetchData( MemKey *key, MemData *data )
{
	//\TODO: Add multiple access protection
	if( data != NULL && key != NULL )
		memcpy(data->data, key->BaseAdd, key->size);
	else
		return -1;
		
	return 0;
}

int putData( MemKey *key, MemData *data )
{
	//\TODO: Add multiple access protection
	if( data != NULL && key != NULL )
		memcpy(key->BaseAdd, data->data, key->size);
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
		if( localBuff == NULL)
			return -1;
	}
	
	if( remote != NULL )
	{
		remoteKey= createMemKeyFromChar( remote );
		if(remoteKey == NULL)
			return -1;
		
		remoteBuff= (MemData*)calloc(1, sizeof(MemData));
		if( remoteBuff == NULL)
			return -1;
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
		if( remoteBuff->data != NULL )
			free(remoteBuff->data);
		
		free(remoteBuff);
	}
	
	if( localBuff != NULL)
	{
		if( localBuff->data != NULL )
			free(localBuff->data);
		
		free(localBuff);
	}
		
	return 0;
	
}
