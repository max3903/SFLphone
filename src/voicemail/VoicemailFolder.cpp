/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Florian DESPORTES <florian.desportes@savoirfairelinux.com>
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

#include <iostream>
#include <sstream>
#include <string>
#include "VoicemailFolder.h"

using namespace std;

VoicemailFolder::VoicemailFolder() {
//	vector<Voicemail> _lst_vm;
}

VoicemailFolder::~VoicemailFolder() {
//	cout << "~VoicemailFolder" << endl;
}

void VoicemailFolder::setName(string name) {
	_name = name;
}

string VoicemailFolder::getName() {
	return _name;
}

void VoicemailFolder::setCount(int count) {
	_count = count;
}

int VoicemailFolder::getCount() {
	return _count;
}

string VoicemailFolder::getCountString() {
	ostringstream oss;
	oss << getCount();
	return oss.str();
}
vector<Voicemail *> VoicemailFolder::getLstVM() {
	return _lst_vm;
}

void VoicemailFolder::addVM(Voicemail *vm) {
	_lst_vm.push_back( vm );
//	_count++;
}

bool VoicemailFolder::removeVM(Voicemail *vm) {
//	_count--;
	return true;
}

Voicemail * VoicemailFolder::getVMAt(int i) {
	if( i<0 || i>=getCount() ) {
		return NULL;
	} else {
		return _lst_vm[i];
	}
}

Voicemail * VoicemailFolder::getVMByName(string name) {
	for( int i = 0 ; i < getCount() ; i++ ) {
		if( getVMAt(i)->getName().compare( name ) == 0 ) {
			return getVMAt(i);
		}
	}
	return NULL;
}

string VoicemailFolder::toString() {
//	string res("   '-");
	string res("");
	res.append( getName() );
	res.append(" (");
	res.append( getCountString() );
	res.append(")");
	cout << res << endl;
	return res;
}

