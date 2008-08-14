/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
 *  Author: Florian Desportes <florian.desportes@savoirfairelinux.com>
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

VoicemailFolder::VoicemailFolder() {
	_count = 0;
}

VoicemailFolder::~VoicemailFolder() {
}


std::string VoicemailFolder::getCountString() {
	std::ostringstream oss;
	oss << getCount();
	return oss.str();
}

std::vector<Voicemail *> VoicemailFolder::getLstVM() {
	return _lst_vm;
}

void VoicemailFolder::addVM(Voicemail *vm) {
	_lst_vm.push_back(vm);
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

Voicemail * VoicemailFolder::getVMByName(const std::string& name) {
	for( int i = 0 ; i < getCount() ; i++ ) {
		if( getVMAt(i)->getName().compare(name) == 0 ) {
			return getVMAt(i);
		}
	}
	return NULL;
}

std::string VoicemailFolder::toString() {
	std::string res("<b>");
	res.append(getName());
	res.append("</b>|");
	res.append(getName());
//	res.append(" (");
//	res.append( getCountString() );
//	res.append(")");
//	cout << res << endl;
	return res;
}

