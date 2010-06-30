#ifndef __H264_DEPAYLOADER_TEST_H__
#define __H264_DEPAYLOADER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>
#include <iostream>

class H264DecoderTest: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( H264DecoderTest );
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
};
/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(H264DecoderTest, "H264DecoderTest");
CPPUNIT_TEST_SUITE_REGISTRATION( H264DecoderTest );

#endif
