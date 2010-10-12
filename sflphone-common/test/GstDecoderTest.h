#ifndef __GST_DECODER_TEST_H__
#define __GST_DECODER_TEST_H__

#include "video/rtp/VideoRtpSessionSimple.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>
#include <iostream>

class GstDecoderTest: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( GstDecoderTest );
	CPPUNIT_TEST( testReceiveJpeg );
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

	void testReceiveH264();

	void testReceiveJpeg();

private:
	sfl::VideoRtpSession* session;
};
/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(GstDecoderTest, "GstDecoderTest");
CPPUNIT_TEST_SUITE_REGISTRATION( GstDecoderTest );

#endif
