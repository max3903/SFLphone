#include "GstDecoderTest.h"

#include "video/codec/GstCodecH264.h"
#include "video/codec/GstCodecJpeg.h"
#include "sip/sdp/RtpMap.h"
#include "sip/sdp/Fmtp.h"

#include <ccrtp/rtp.h>

void GstDecoderTest::setUp() {
	std::cout << "Setting up..." << std::endl;

	ost::InetHostAddress address("0.0.0.0");
	session = new sfl::VideoRtpSession(address, (ost::tpport_t) 5000);
}

void GstDecoderTest::tearDown() {
	std::cout << "Tearing down..." << std::endl;

	sleep(10);

	if (system("killall gst-launch-0.10") < 0) {
		CPPUNIT_FAIL("Failed to stop server in video RTP test.");
	}

	delete session;
}

void GstDecoderTest::testReceiveJpeg()
{
	std::cout << "Trying to decode JPEG frames ... " << std::endl;

	sfl::GstCodecJpeg* codec = new sfl::GstCodecJpeg();
	session->registerCodec(codec);

	std::cout << "Codec registered." << std::endl;

	// Simulate the arrival of an SDP offer
	sfl::RtpMap rtpmap("96", "JPEG", 90000, "");
	sfl::Fmtp fmtp("96", "");
	session->addSessionCodec(rtpmap, fmtp);

	std::cout << "Session codec added." << std::endl;

	session->start();

	if (system("./server-jpeg.sh >> /dev/null &") < 0) {
		CPPUNIT_FAIL("Failed to start server in video RTP test.");
	}
}

void GstDecoderTest::testReceiveH264()
{
	std::cout << "Trying to decode H264 frames ... " << std::endl;

	sfl::GstCodecH264* codec = new sfl::GstCodecH264();
	session->registerCodec(codec);

	std::cout << "Codec registered." << std::endl;

	// Simulate the arrival of an SDP offer
	sfl::RtpMap rtpmap("96", "H264", 90000, "");
	sfl::Fmtp fmtp("96", "profile-level-id=42A01E; packetization-mode=0; sprop-parameter-sets=Z0IACpZTBYmI,aMljiA==");
	session->addSessionCodec(rtpmap, fmtp);

	std::cout << "Session codec added." << std::endl;

	session->start();

	if (system("./server.sh >> /dev/null &") < 0) {
		CPPUNIT_FAIL("Failed to start server in video RTP test.");
	}
}
