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
#include "SemaphorePosixTest.h"

void SemaphorePosixTest::tearDown() {
	delete sem;
}

void SemaphorePosixTest::setUp() {
	sem = new sfl::SemaphorePosix("/org.sflphone.sem1", false, 0666, 1);
}

void SemaphorePosixTest::testRemove() {
	std::cerr << "Testing remove()\n";
	CPPUNIT_ASSERT_NO_THROW(sem->remove());
}

void SemaphorePosixTest::testPostWait() {
	std::cerr << "Testing post/wait\n";

	sem = new sfl::SemaphorePosix("/org.sflphone.sem1", false, 0666, 1);
	sfl::SemaphorePosix* sem2 = new sfl::SemaphorePosix("/org.sflphone.sem1", false, 0666, 1);

	CPPUNIT_ASSERT_NO_THROW(sem->wait());
		std::cout << "Should be false\n";
		std::cout << sem2->tryWait() << std::endl;
		CPPUNIT_ASSERT_EQUAL(false, sem2->tryWait());
		std::cerr << "Entering critical section\n";
	CPPUNIT_ASSERT_NO_THROW(sem->post());
}
