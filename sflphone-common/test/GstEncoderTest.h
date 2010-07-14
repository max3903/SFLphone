#ifndef __GST_ENCODER_TEST_H__
#define __GST_ENCODER_TEST_H__

#include "video/rtp/VideoRtpSessionSimple.h"
#include "video/source/VideoInputSourceGst.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include <assert.h>
#include <stdint.h>
#include <iostream>

class GstEncoderTest: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( GstEncoderTest );
	CPPUNIT_TEST( testSendH264 );
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

	void testSendH264();

	void testSendJpeg();

private:
	sfl::VideoInputSourceGst* source;
	sfl::VideoRtpSessionSimple* session;
};
/* Register our test module */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(GstEncoderTest, "GstEncoderTest");
CPPUNIT_TEST_SUITE_REGISTRATION( GstEncoderTest );

#endif
