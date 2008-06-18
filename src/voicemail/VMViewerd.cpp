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
#include <fstream>  // To read xml file
#include "VMViewerd.h"

using namespace std;


VoicemailFolder *vmf;
Voicemail       *vm;
VoicemailSound  *vms;
string          eltValue;


/****************
 * XML HANDLERS *
 ****************/
void startElement( void *userData , const XML_Char *name , const XML_Char **atts ) {
	if( strcmp( name , "result" ) == 0 ) {
//		cout << "<result>" << endl;
	}
	else if( strcmp( name , "directory" ) == 0 ) {
//		cout << "<directory>" << endl;
		vmf = new VoicemailFolder();
		bool value = false;		// To know if parsing name or value
		const char** attribute;
		for( attribute = atts ; *attribute ; attribute++ ) {
			if( value ) {
				// Parsing a value so its name is the previous string
				const char* attributeName = *(attribute-1);
				if( strcmp( attributeName , "name" ) == 0 ) {
					vmf->setName( *attribute );
				}
				else if( strcmp( attributeName , "count" ) == 0 ) {
					vmf->setCount( atoi( *attribute ) );
				}
			}
			// Alterning between name and value
			value = ! value;
		}
	}
	else if( strcmp( name , "voicemail" ) == 0 ) {
//		cout << "<voicemail>" << endl;
		vm = new Voicemail();
	}
	else if( strcmp( name , "deleted" ) == 0 ) {
//		cout << "<deleted>" << endl;
	}
	else if( strcmp( name , "renamed" ) == 0 ) {
//		cout << "<renamed>" << endl;
	}
	else if( strcmp( name , "sound" ) == 0 ) {
//		cout << "<sound>" << endl;
		vms = new VoicemailSound();
		bool value = false;		// To know if parsing name or value
		const char** attribute;
		for( attribute = atts ; *attribute ; attribute++ ) {
			if( value ) {
				// Parsing a value so its name is the previous string
				const char* attributeName = *(attribute-1);
				if( strcmp( attributeName , "folder" ) == 0 ) {
					vms->setFolder( *attribute );
				}
				else if( strcmp( attributeName , "file" ) == 0 ) {
					vms->setFile( *attribute );
				}
				else if( strcmp( attributeName , "format" ) == 0 ) {
					vms->setFormat( *attribute );
				}
			}
			// Alterning between name and value
			value = ! value;
		}
	}
	else if( strcmp( name , "error" ) == 0 ) {
//		cout << "<error>" << endl;
	}
	else if( strcmp( name , "error-message" ) == 0 ) {
//		cout << "<error-message>" << endl;
	}
	else {
//		cout << "<" << name << ">" << endl;
		if( strcmp( name , "name" ) == 0 ) {
//			cout << "voicemail name : " << eltValue << endl;
			vm->setName( eltValue );
			eltValue.clear();
		}
	}
}


void endElement( void *userData , const XML_Char *name ) {
	VMViewerd *vmv = (VMViewerd *)userData;
	if( strcmp( name , "result" ) == 0 ) {
//		cout << "</result>" << endl;
	}
	else if( strcmp( name , "directory" ) == 0 ) {
//		cout << "</directory>" << endl;
		vmv->addVMF( vmf );
		vmf = NULL;
	}
	else if( strcmp( name , "voicemail" ) == 0 ) {
//		cout << "</voicemail>" << endl;
		vmf->addVM( vm );
		vm = NULL;
	}
	else if( strcmp( name , "deleted" ) == 0 ) {
//		cout << "</deleted>" << endl;
	}
	else if( strcmp( name , "renamed" ) == 0 ) {
//		cout << "</renamed>" << endl;
	}
	else if( strcmp( name , "sound" ) == 0 ) {
		vms->setDatas( eltValue );
		vmv->addVMS( vms );
		vms = NULL;
	}
	else if( strcmp( name , "error" ) == 0 ) {
//		cout << "</error>" << endl;
	}
	else if( strcmp( name , "error-message" ) == 0 ) {
//		cout << "</error-message>" << endl;
	}
	else {
		/** VOICEMAIL'S INFORMATIONS */
//		cout << "endElement, value : " << eltValue << endl;
//		cout << "</" << name << ">" << endl;
		if( strcmp( name , "name" ) == 0 ) {
			vm->setName( eltValue );
		}
		if( strcmp( name , "id" ) == 0 ) {
			vm->setId( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "callerchan" ) == 0 ) {
			vm->setCallerchan( eltValue );
		} 
        if( strcmp( name , "callerid" ) == 0 ) {
			vm->setCallerid( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "category" ) == 0 ) {
			vm->setCategory( eltValue );
		}
		if( strcmp( name , "context" ) == 0 ) {
			vm->setContext( eltValue );
		}
		if( strcmp( name , "duration" ) == 0 ) {
			vm->setDuration( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "exten" ) == 0 ) {
			vm->setExten( eltValue );
		}
		if( strcmp( name , "macrocontext" ) == 0 ) {
			vm->setMacrocontext( eltValue );
		}
		if( strcmp( name , "origdate" ) == 0 ) {
			vm->setOrigdate( eltValue );
		}
		if( strcmp( name , "origmailbox" ) == 0 ) {
			vm->setOrigmailbox( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "origtime" ) == 0 ) {
			vm->setOrigtime( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "priority" ) == 0 ) {
			vm->setPriority( atoi( eltValue.c_str() ) );
		}
		eltValue.clear();
	}
}

void character( void *userData , const char *s , int len ) {
	eltValue = string( s , len );
}


void VMViewerd::parse() {
	// Parses the data
	std::fstream file;
	char * buf;
	int length;

	// Opens XML file stored in _fileStore instance var, usually /tmp/sflphone_vm
	file.open( getFileStore().c_str(), fstream::in );

	// Gets length of file:
	file.seekg(0, ios::end);
	length = file.tellg();
	file.seekg(0, ios::beg);

	// Allocates memory:
	buf = new char[length];

	// Creates parser for presence status provided in XML body
	XML_Parser parser = XML_ParserCreate(NULL);

	// Sets functions that will be called when parsing elements and character data
	XML_SetElementHandler( parser, startElement , endElement );
	XML_SetCharacterDataHandler( parser, character );
	XML_SetUserData( parser , this );

	// Parses the data
	file.read(buf, length);
	//cout << "file : " << buf << endl;
	if( XML_Parse(parser, buf, length, 0) == XML_STATUS_ERROR ) {
		cout << " ERRORR reading file..." << endl;
		cout << XML_ErrorString( XML_GetErrorCode( parser ) ) << endl;
	}
	
	file.close();
	delete[] buf;

	XML_ParserFree( parser );
}


/**********************************
 * VMVIEWERD METHODS' DECLARATION *
 **********************************/

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
	s += getLogVMail(); // user's login/voicemail #
	s += ":";
	s += getPwdVMail(); // user's passcode to voicemail system
	s += "@";
	s += _srvAddr; // asterisk's ip address
	s += ":";
	s += getSrvPort(); // asterisk's ip port
	s += "/";
	s += _srvPath; // asterisk's path to webservice
	s += "/";
	s += cmd; // command to execute (e.g. list, sound, del, etc.)
	s += "\"";
	cout << "exec : " << s.c_str() << endl;
	return system( s.c_str() );
}

string VMViewerd::getLogVMail() {
	return _logVMail;
}

string VMViewerd::getPwdVMail() {
	return _pwdVMail;
}

string VMViewerd::getSrvPort() {
	return _srvPort;
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

VoicemailFolder * VMViewerd::getFolderByName(string name) {
	for( int i = 0 ; i < getLstFolders().size() ; i++ ) {
		if( getFolderAt(i)->getName().compare( name ) == 0 ) {
			return getFolderAt(i);
		}
	}
	return NULL;
}

void VMViewerd::addVMF(VoicemailFolder *vmf) {
	_lst_folders.push_back( vmf );
}

//bool VMViewerd::removeVMF(VoicemailFolder vmf) {
//	return _lst_folders.erase( vmf );
//}

vector<VoicemailSound *> VMViewerd::getLstSounds() {
	return _lst_sounds;
}

VoicemailSound * VMViewerd::getSoundAt(int i) {
	if( i < 0 || i >= getLstSounds().size() ) {
		return NULL;
	} else {
		return _lst_sounds[i];
	}
}

void VMViewerd::addVMS(VoicemailSound * vms) {
	_lst_sounds.push_back( vms );
}


vector< string >
VMViewerd::toArrayString() {
	int i,j;
	vector<string> vec;
	if( getLstFolders().size() != 0 ) {
		for( i=0 ; i<=getLstFolders().size()-1 ; i++ ) {
			vec.push_back( getFolderAt(i)->toString() );
		}
	}
	return vec;
}


void VMViewerd::toString() {
	int i,j;
	cout << "VMVIEWERD" << endl;
	if( getLstFolders().size() != 0 ) {
		cout << " '-VOICEMAIL FOLDERS" << endl;
		for( i=0 ; i<=getLstFolders().size()-1 ; i++ ) {
			//cout << " | '-" << getFolderAt(i)->getName() << endl;
			getFolderAt(i)->toString();
			for( j=0 ; j<=getFolderAt(i)->getCount()-1 ; j++ ) {
				//cout << " |     '-" << getFolderAt(i)->getVMAt(j)->getName() << endl;
				getFolderAt(i)->getVMAt(j)->toShortString();
			}
		}
	}
	if( getLstSounds().size() != 0 ) {
		cout << " '-VOICEMAIL SOUNDS" << endl;
		for( i=0 ; i<=getLstSounds().size()-1 ; i++ ) {
			//cout << "   '-" << getFolderAt(i)->getName() << endl;
			getSoundAt(i)->toString();
		}
	}
}

