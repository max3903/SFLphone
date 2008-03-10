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
	return 0;
}

MemKey* initSpace( MemKey *key )
{
	int shmid;
	
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
	memcpy(data->data, key->BaseAdd, key->size);
	return 0;
}

int putData( MemKey *key, MemData *data )
{
	memcpy(key->BaseAdd, data->data, key->size);
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
	
	free(localKey);
	free(remoteKey);
	free(remoteBuff);
	free(localBuff);
	
	return 0;
	
}
