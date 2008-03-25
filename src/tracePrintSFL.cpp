/*
 *  Copyright (C) 2004-2007 Savoir-Faire Linux inc.
 *  Author: Jean Tessier <jean.tessier@polymtl.ca>
 *                                                                              
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
 
#include "tracePrintSFL.h"

void tracePrintSfl::operator()(const char* Message, int type, int level, bool ret){
	
	if(level > SFLTRACE_LEVEL && type != MT_ERROR && type != MT_FATAL)
		return;
	
	switch(type){
		case MT_ERROR:
			perror(SOFT_HEADER);
			perror(HEADER_ERROR);
			perror(Message);
			if(ret) perror("\n");
			break;
		case MT_FATAL:
			perror(SOFT_HEADER);
			perror(HEADER_FATAL);
			perror(Message);
			if(ret) perror("\n");			
			exit(-1);			
			break;
		case MT_INFO:
			printf(SOFT_HEADER);
			printf(HEADER_INFO);
			for(int i= 0; i < level; i++)
				printf("\t");
			printf(Message);
			if(ret) printf("\n");
			break;			
		case MT_WARNING:
			printf(SOFT_HEADER);
			printf(HEADER_WARNING);
			for(int i= 0; i < level; i++)
				printf("\t");
			printf(Message);
			if(ret) printf("\n");
			break;
		case MT_NONE:
			printf(Message);
			if(ret) printf("\n");
			break;				
		default:
			perror("printTrace Error: bad message type");
			break;
	}
	
}
