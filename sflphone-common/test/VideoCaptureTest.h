#ifndef _VIDEOCAPTURE_TEST_
#define _VIDEOCAPTURE_TEST_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>

#include "video/source/VideoInputSource.h"
#include "video/source/VideoInputSourceGst.h"

class VideoCaptureTest: public CppUnit::TestFixture 
{
	CPPUNIT_TEST_SUITE( VideoCaptureTest );
		CPPUNIT_TEST( testEnumerateDevices );
		CPPUNIT_TEST( testOpenClose );
		CPPUNIT_TEST( testFrameObserver );
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
		
		/**
		 * Enumerates visible devices to GST.
		 */
		void testEnumerateDevices();

		/**
		 * Make sure that the observers are called and receiving frames.
		 */
		void testFrameObserver();

	private:
		sfl::VideoInputSourceGst* videoInput;	 

		class VideoFrameObserverTest : public sfl::VideoFrameObserver {
		public:
			int i;
			VideoFrameObserverTest() : i(0){}
			void onNewFrame(const sfl::VideoFrame * frame) { std::cout << "OBSERVER BEING CALLED\n"; i++; }
		};
};

/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(VideoCaptureTest, "VideoCaptureTest");
CPPUNIT_TEST_SUITE_REGISTRATION( VideoCaptureTest );

#endif
