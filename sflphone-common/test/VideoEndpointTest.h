#ifndef _VIDEOCAPTURE_TEST_
#define _VIDEOCAPTURE_TEST_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>

#include <video/VideoEndpoint.h>
#include <video/source/VideoInputSourceGst.h>

class VideoEndpointTest: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( VideoEndpointTest );
		CPPUNIT_TEST( shmWriteTest );
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
		 * Start the video stream, and let the object write into the shared memory.
		 */
		void shmWriteTest();

	private:
		sfl::VideoEndpoint* videoEndpoint;
		sfl::VideoInputSourceGst* videoSource;
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(VideoEndpointTest, "VideoEndpointTest");
CPPUNIT_TEST_SUITE_REGISTRATION( VideoEndpointTest );

#endif
