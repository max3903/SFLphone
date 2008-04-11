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

sem_t tracePrintSfl::access;

tracePrintSfl::tracePrintSfl(){
	sem_init(&this->access, 0 ,1);
}

tracePrintSfl::~tracePrintSfl(){
}

void tracePrintSfl::operator()(const char* Message, int type, int level, bool ret, int num, ...){

	if(level > SFLTRACE_LEVEL && type != MT_ERROR && type != MT_FATAL)
		return;

	/*int lenMsg= strlen(Message) + 1;
	char* tmpMsg= new char[ lenMsg + 100 ];
	strcpy(tmpMsg, Message);
		
	if( num > 0 ){
	
		char* buffMsg= new char[ lenMsg + 100 ];		
			
		va_list param_pointer;
		
		// get the number of parameters
		va_start(param_pointer,num);
	
		int tmpVal= -2;
		for (int index = 0 ; index < num ; index++){
			strcpy(buffMsg,tmpMsg);
			tmpVal= va_arg( param_pointer, int ); 
			sprintf(tmpMsg, buffMsg, tmpVal );	
		}
		
		delete[] buffMsg;
	}*/
		
	sem_wait(&this->access);
	
	switch(type){
		case MT_ERROR:
			printf(SOFT_HEADER);
			printf(HEADER_ERROR);
			printf(Message);
			if(ret) printf("\n");
			break;
		case MT_FATAL:
			printf(SOFT_HEADER);
			printf(HEADER_FATAL);
			printf(Message);
			if(ret) printf("\n");			
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
	
	//delete[] tmpMsg;
	
	sem_post(&this->access);
}
