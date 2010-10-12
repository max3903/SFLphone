#ifndef __H264_DEPAYLOADER_TEST_H__
#define __H264_DEPAYLOADER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>
#include <iostream>

class H264EncoderTest: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( H264EncoderTest );
	CPPUNIT_TEST( testSend );
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
	 * Try to receive frames from a Gstreamer source (script).
	 */
	void testSend();
};
/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(H264EncoderTest, "H264EncoderTest");
CPPUNIT_TEST_SUITE_REGISTRATION( H264EncoderTest );

#endif
