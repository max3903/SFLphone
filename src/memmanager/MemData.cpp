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

#include "MemData.h"
MemData::MemData()
{	
	this->data= NULL;
	this->size= -1;
	this->height= -1;
	this->width= -1;
}

MemData::~MemData()
{
	if(this->data != NULL)
		delete this->data;
		
}

MemData::MemData(MemData* data)
{
		
	this->data= new unsigned char[data->size];
	
	memcpy(this->data,data->data, data->size);
	
	this->size= data->size;
}

MemData::MemData(MemData& data)
{
		
	this->data= new unsigned char[data.size];
	
	memcpy(this->data,data.data, data.size);
	
	this->size= data.size;
}

int MemData::fetchData(unsigned char* dataOuput)
{
	if( data != NULL ){
		
		memcpy(dataOuput,this->data, this->size);
		return this->size;
		
	}
	
	return -1;
}
void MemData::putData(unsigned char * dataInput, int sizeInput)
{
	if( this->data != NULL )
		delete data;
	
	this->data= (unsigned char*)malloc(sizeInput);
	
	memcpy(this->data,data, sizeInput);
	
	this->size = sizeInput;
	
}

void MemData::setData(unsigned char* data)
{
this->data = data;	
	
}

unsigned char* MemData::getData()
{
return this->data ;	
	
}
