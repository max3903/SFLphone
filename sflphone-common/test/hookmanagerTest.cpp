/*
 *  Copyright (C) 2009 Savoir-Faire Linux inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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

#include <stdio.h>
#include <sstream>
#include <dlfcn.h>

#include "hookmanagerTest.h"

using std::cout;
using std::endl;


void HookManagerTest::setUp(){
    // Instanciate the hook manager singleton
    urlhook = new UrlHook ();
}

void HookManagerTest::testAddAction (){

    int status;

    status = urlhook->addAction ("www.google.ca", "gnome-www-browser");
    CPPUNIT_ASSERT (status == 0); 
}

void HookManagerTest::tearDown(){
    // Delete the hook manager object
    delete urlhook; urlhook=0;
}