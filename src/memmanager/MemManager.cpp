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

#include "MemManager.h"

MemManager* MemManager::instance= 0;

MemManager* MemManager::getInstance()
{
	return this;
}

MemManager::MemManager()
{
}

MemManager::~MemManager()
{
}

const MemKey* MemManager::initSpace(int key, int size)
{
	return 0;
}

const MemKey* MemManager::initSpace(MemKey* key, int size)
{
	return 0;
}

bool MemManager::setDefaultSpace(MemKey* key)
{
	return false;
}

void MemManager::nextSpace()
{
}

void MemManager::previousSpace()
{
}

MemData* MemManager::fetchData( )
{
}

MemData* MemManager::fetchData(int key)
{
}

MemData* MemManager::fetchData(MemKey* key)
{
}

bool MemManager::putData(void * Data, int size)
{
	return false;
}

bool MemManager::putData(int key, void * Data, int size)
{
	return false;
}

bool MemManager::putData(MemKey* key, void * Data, int size)
{
	return false;
}

vector<MemKey*> MemManager::getAvailSpaces() const 
{
	vector<MemKey*> tmp;
	return tmp;
}

const MemKey* MemManager::initSpace(int size)
{
	return 0;
}

int MemManager::genKey()
{
}
