#ifndef __H264_DEPAYLOADER_TEST_H__
#define __H264_DEPAYLOADER_TEST_H__

#include "video/depayloader/VideoDepayloader.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>
#include <iostream>

class H264DepayloaderTest: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( H264DepayloaderTest );
	CPPUNIT_TEST( testReceive );
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
	 * Try to receive frames from a gstreamer source (script).
	 */
	void testReceive();

private:
	class TestObserver : public sfl::VideoFrameDepayloadedObserver {
	public:
		TestObserver() : i(0) {}

		void onNewDepayloadedFrame(sfl::Buffer<uint8_t>& buffer) {
			std::cout << "Buffer depayloaded. Size ";
			std::cout << buffer.getSize() << std::endl;
			i++;
		}

		unsigned i;
	};
};
/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(H264DepayloaderTest, "H264DepayloaderTest");
CPPUNIT_TEST_SUITE_REGISTRATION( H264DepayloaderTest );

#endif
