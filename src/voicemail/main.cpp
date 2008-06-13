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
#include <string>
#include "VMViewerd.h"

using namespace std;


int
main( int argc, char **argv ) {

	VMViewerd *vmv = new VMViewerd(6666, 735, "default", "127.0.0.1", "uml/index", 80);
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
	
	if( strcmp( argv[1] , "list" ) != 0 ) {
		vmv->exec( "list" );
		vmv->parse();
	}
	string req = (argc == 2 ? argv[1] : "list" );
	int res = vmv->exec( req );
	
	vmv->parse();
	cout << endl << endl;
	vmv->toString();
	
	return 0;
}

