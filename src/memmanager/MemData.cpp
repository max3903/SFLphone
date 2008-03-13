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
}

MemData::~MemData()
{
	// TODO: Use memcpy to copy data
}

MemData::MemData(MemData* data)
{
	// TODO: Use memcpy to copy data
	strcpy(this->data,data->data);
}

MemData::MemData(MemData& data)
{
	strcpy(this->data,data.data);
}

int MemData::fetchData(char* data)
{
	strcpy(data,this->data);
	return size;
}
void MemData::putData(char * data, int size)
{
	strcpy(this->data,data);
	this->size = size;
	
}
