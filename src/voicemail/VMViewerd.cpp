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
#include "VMViewerd.h"

using namespace std;

VMViewerd::~VMViewerd() {
//	cout << "~VMViewerd" << endl;
}


int VMViewerd::exec(string cmd) {
	// Sets the unix following command :
	// wget -q -O <path_to_file_to_store_received_datas> "http://<context>-<user_login>:<user_password>@<full_server_address>/<cmd>"
	string s = "wget -q -O ";
	s += getFileStore(); // save request to file
	s += " \"http://";
	s += _context; // user's asterisk context
	s += "-";
	s += getLogVMailString(); // user's login/voicemail #
	s += ":";
	s += getPwdVMailString(); // user's passcode to voicemail system
	s += "@";
	s += _srvAddr; // asterisk's ip address
	s += ":";
	s += getSrvPortString(); // asterisk's ip port
	s += "/";
	s += _srvPath; // asterisk's path to webservice
	s += "/";
	s += cmd; // command to execute (e.g. list, sound, del, etc.)
	s += "\"";
//	cout << s.c_str() << endl;
	return system( s.c_str() );
}

int VMViewerd::getLogVMail() {
	return _logVMail;
}

string VMViewerd::getLogVMailString() {
	ostringstream oss;
	oss << _logVMail;
	return oss.str();
}

string VMViewerd::getPwdVMailString() {
	ostringstream oss;
	oss << _pwdVMail;
	return oss.str();
}

string VMViewerd::getSrvPortString() {
	ostringstream oss;
	oss << _srvPort;
	return oss.str();
}

string VMViewerd::getFileStore() {
	return _file_store;
}


vector<VoicemailFolder *> VMViewerd::getLstFolders() {
	return _lst_folders;
}

VoicemailFolder * VMViewerd::getFolderAt(int i) {
	if( i < 0 || i >= getLstFolders().size() ) {
		return NULL;
	} else {
		return _lst_folders[i];
	}
}

void VMViewerd::addVMF(VoicemailFolder *vmf) {
	_lst_folders.push_back( vmf );
}

//bool VMViewerd::removeVMF(VoicemailFolder vmf) {
//	return _lst_folders.erase( vmf );
//}

void VMViewerd::toString() {
	int i,j;
	cout << "VMViewerd" << endl;
	if( getLstFolders().size() != 0 ) {
		cout << " '-VoicemailFolders" << endl;
		for( i=0 ; i<=getLstFolders().size()-1 ; i++ ) {
			//cout << "   '-" << getFolderAt(i)->getName() << endl;
			getFolderAt(i)->toString();
			for( j=0 ; j<=getFolderAt(i)->getCount()-1 ; j++ ) {
				//cout << "       '-" << getFolderAt(i)->getVMAt(j)->getName() << endl;
				getFolderAt(i)->getVMAt(j)->toString();
			}
		}
	}
}

/*
int main(int argc, char *argv[]) {
	cout << " -- VMViewerdMain --" << endl;
	return 0;
}
*/
