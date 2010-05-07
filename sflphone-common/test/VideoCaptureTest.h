#ifndef _VIDEOCAPTURE_TEST_
#define _VIDEOCAPTURE_TEST_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>

namespace sfl {
	class VideoInputSourceGst;
}
class VideoCaptureTest: public CppUnit::TestFixture 
{
	CPPUNIT_TEST_SUITE( VideoCaptureTest );
		CPPUNIT_TEST( testOpenClose );
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
		 * Open a device and close it.
		 */
		void testOpenClose();
		
	private:
		sfl::VideoInputSourceGst* videoInput;	 
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(VideoCaptureTest, "VideoCaptureTest");
CPPUNIT_TEST_SUITE_REGISTRATION( VideoCaptureTest );

#endif
