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

/*
 * HOW TO COMPILE ALONE :
 * g++ -lexpat -o vmv *.cpp
 */

#include <iostream>
#include <fstream>
#include <string>
#include <expat.h> // For XML parsing
#include "VMViewerd.h"

using namespace std;

VMViewerd       *vmv;
VoicemailFolder *vmf;
Voicemail       *vm;
string          eltValue;


/****************
 * XML HANDLERS *
 ****************/
void
startElement( void *userData , const XML_Char *name , const XML_Char **atts ) {
//	cout << "-- start Element --" << endl;
//	cout << "  name : " << name << endl;
//	cout << "  atts : " << atts << endl;
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
				if( strcmp( attributeName , "count" ) == 0) {
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
		bool value = false;		// To know if parsing name or value
		const char** attribute;
		for( attribute = atts ; *attribute ; attribute++ ) {
			if( value ) {
				// Parsing a value so its name is the previous string
				const char* attributeName = *(attribute-1);
				if( strcmp( attributeName , "folder" ) == 0 ) {
					//vmf->setName( *attribute );
					cout << " attr_folder : " << *attribute << endl;
				}
				if( strcmp( attributeName , "file" ) == 0) {
					//vmf->setCount( *attribute) );
					cout << " attr_file   : " << *attribute << endl;
				}
				if( strcmp( attributeName , "format" ) == 0) {
					//vmf->setCount( *attribute );
					cout << " attr_format : " << *attribute << endl;
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
		cout << "<" << name << ">" << endl;
		if( strcmp( name , "name" ) == 0 ) {
			cout << "voicemail name : " << eltValue << endl;
			vm->setName( eltValue );
			eltValue.clear();
		}
	}
}

void
endElement( void *userData , const XML_Char *name ) {
//	cout << "-- end Element --" << endl;
//	cout << "  name : " << name << endl;
//	cout << "endElement, value : " << eltValue << endl;
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
//		cout << "</sound>" << endl;
	}
	else if( strcmp( name , "error" ) == 0 ) {
//		cout << "</error>" << endl;
	}
	else if( strcmp( name , "error-message" ) == 0 ) {
//		cout << "</error-message>" << endl;
	}
	else {
		cout << "endElement, value : " << eltValue << endl;
		cout << "</" << name << ">" << endl;
		if( strcmp( name , "name" ) == 0 ) {
			vm->setName( eltValue );
		}
		if( strcmp( name , "id" ) == 0 ) {
			vm->setId( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "context" ) == 0 ) {
			vm->setContext( eltValue );
		}
		if( strcmp( name , "macrocontext" ) == 0 ) {
			vm->setMacrocontext( eltValue );
		}
		if( strcmp( name , "callerid" ) == 0 ) {
			vm->setCallerid( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "duration" ) == 0 ) {
			vm->setDuration( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "date" ) == 0 ) {
//			vm->setDate( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "mailboxuser" ) == 0 ) {
//			vm->setMailboxuser( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "mailboxcontext" ) == 0 ) {
//			vm->setMailboxcontext( atoi( eltValue.c_str() ) );
		}
		if( strcmp( name , "category" ) == 0 ) {
			vm->setCategory( eltValue );
		}
		eltValue.clear();
	}
}

void
character( void *userData , const char *s , int len ) {
//	cout << "-- character --" << endl;
//	cout << "  s    : " << s << endl;
//	cout << "  len  : " << len << endl;

	eltValue = string( s , len );
//	cout << "eltValue : " << eltValue << endl;
}




/********
 * MAIN *
 ********/
int
main( int argc, char **argv ) {

	/*VMViewerd *v*/vmv = new VMViewerd(6666, 735, "default", "127.0.0.1", "uml/index", 80);
	//cout << system("wget -q -O - http://default-6666:735@127.0.0.1/uml/index") << endl;

	if( argc != 2 ) {
		cout << "Usage : ./vmv <CMD>" << endl
			 << "With command like :" << endl
			 << "   list" << endl
			 << "   sound?f=<FOLDER_NAME>&v=<VOICEMAIL_NAME>" << endl
			 << "   del?f=<FOLDER_NAME>&v=<VOICEMAIL_NAME>" << endl
			 << "   rename?f=<FOLDER_NAME>&o=<VOICEMAIL_OLD_NAME>&n=<VOICEMAIL_NEW_NAME>" << endl;
		exit(1);
	}

	string req = (argc == 2 ? argv[1] : "list" );
//	cout << req << endl;
	int res = vmv->exec( req );
	
	// Parse the data
	std::fstream file;
	char * buf;
	int length;

	// Open XML file, usually /home/user/.sflphone/Account:XXXXXXXXXX-contactlist.xml
	file.open( vmv->getFileStore().c_str(), fstream::in);

	// get length of file:
	file.seekg(0, ios::end);
	length = file.tellg();
	file.seekg(0, ios::beg);

	// allocate memory:
	buf = new char[length];

	// Create parser for presence status provided in XML body
	XML_Parser parser = XML_ParserCreate(NULL);

	// Set functions that will be called when parsing doctype declarations, elements and character data
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser,	character);

	// Parse the data
//	while( ! file.eof() )
//	{
		file.read(buf, length);
		//cout << "file : " << buf << endl;
		if( XML_Parse(parser, buf, length, 0) == XML_STATUS_ERROR ) {
			cout << " ERRORR reading file..." << endl;
			cout << XML_ErrorString( XML_GetErrorCode( parser ) ) << endl;
		}
//	}
	
	file.close();
	delete[] buf;

//	XML_Parse(parser, s.c_str(), sizeof(s), 0);
	XML_ParserFree( parser );
	
	cout << endl << endl << endl;
	vmv->toString();
	
	/******************/
	/* DELETE GLOBALS */
	/******************/
	if( vmv )
		delete vmv;
	
	return 0;
}

