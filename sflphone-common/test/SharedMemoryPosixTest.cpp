/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
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
#include "SharedMemoryPosixTest.h"
#include <string.h>

void SharedMemoryPosixTest::tearDown() {
	delete shm;
}

void SharedMemoryPosixTest::setUp() {
	shm = new sfl::SharedMemoryPosix("/org.sflphone.video", false);
	testData
			= "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
}

void SharedMemoryPosixTest::testTruncate() {
	std::cout << "Testing truncate()..\n." << std::endl;

	CPPUNIT_ASSERT_NO_THROW(shm->truncate(testData.length()));

	std::cout << "Requested size: ";
	std::cout << testData.length();
	std::cout << "\nActual size : ";
	std::cout << shm->getSize();

	CPPUNIT_ASSERT_EQUAL((off_t)testData.length(), shm->getSize());
}

void SharedMemoryPosixTest::testWrite()
{
	std::cout << "\nTesting write...\n" << std::endl;

	char* region = (char*) shm->getRegion();
	CPPUNIT_ASSERT(region != NULL);

	strncpy(region, testData.c_str(), testData.length());

	CPPUNIT_ASSERT(strncmp(region, testData.c_str(), testData.length()) == 0);
}

void SharedMemoryPosixTest::testAccessModes()
{
	std::cout << "\nTesting access modes...\n" << std::endl;

	// The segment should already exists.
	sfl::SharedMemoryPosix * readOnlyShm = new sfl::SharedMemoryPosix("/org.sflphone.video", false, std::ios_base::out);
}
