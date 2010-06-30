#ifndef _SEMAPHOREPOSIX_TEST_
#define _SEMAPHOREPOSIX_TEST_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>

#include "util/ipc/SemaphorePosix.h"

class SemaphorePosixTest: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( SemaphorePosixTest );
		CPPUNIT_TEST( testRemove );
		CPPUNIT_TEST( testPostWait );
	CPPUNIT_TEST_SUITE_END();

	public:
		/**
		 * @Override
		 */
		void setUp();
		/**
		 * @Override
		 */
		void tearDown();

		/**
		 * Tries to remove the semaphore.
		 */
		void testRemove();

		/**
		 * Tries post/wait operations.
		 */
		void testPostWait();

	private:
		sfl::SemaphorePosix* sem;
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SemaphorePosixTest, "SemaphorePosixTest");
CPPUNIT_TEST_SUITE_REGISTRATION( SemaphorePosixTest );

#endif
