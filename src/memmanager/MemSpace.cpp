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

#include "MemSpace.h"
#include "MemKey.h"
#include "MemData.h"

MemSpace::MemSpace()
{
}

MemSpace::MemSpace(MemKey* key)
{
}

MemKey* MemSpace::getKey()
{
}

bool MemSpace::putData(void * Data, int size)
{
	return false;
}

MemData* MemSpace::fetchData()
{
}

MemSpace::MemSpace(MemSpace* space)
{
}

MemSpace::MemSpace(MemSpace& space)
{
}

MemSpace::~MemSpace()
{
}

void MemSpace::readSpace(MemData* data)
{
}

void MemSpace::writeSpace(char * data, int size)
{
}
