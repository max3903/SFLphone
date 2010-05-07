#ifndef _VIDEOCAPTURE_TEST_
#define _VIDEOCAPTURE_TEST_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>

#include "util/SharedMemoryPosix.h"

class SharedMemoryPosixTest: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( SharedMemoryPosixTest );
		CPPUNIT_TEST( testTruncate );
		CPPUNIT_TEST( testWrite );
		CPPUNIT_TEST( testAccessModes );
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
		 * Test the truncation feature, where a given size is alloted to the shm.
		 */
		void testTruncate();

		/**
		 * Tries writing to a shared memory region.
		 */
		void testWrite();

		/**
		 * Tries various combinations of access modes.
		 */
		void testAccessModes();

	private:
		sfl::SharedMemoryPosix* shm;
		std::string testData;
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SharedMemoryPosixTest, "SharedMemoryPosixTest");
CPPUNIT_TEST_SUITE_REGISTRATION( SharedMemoryPosixTest );

#endif
