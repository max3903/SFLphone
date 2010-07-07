#include "H264DecoderTest.h"

#include "video/decoder/H264GstDecoder.h"
#include "video/rtp/VideoRtpSession.h"
#include "sip/sdp/RtpMap.h"
#include "sip/sdp/Fmtp.h"

#include <ccrtp/rtp.h>

void H264DecoderTest::setUp() {
	std::cout << "Setting up..." << std::endl;
}

void H264DecoderTest::tearDown() {
	std::cout << "Tearing down..." << std::endl;
}

void H264DecoderTest::testReceive()
{
	std::cout << "Trying to decode frames ... " << std::endl;

	ost::InetHostAddress address("0.0.0.0");
	sfl::VideoRtpSession* session = new sfl::VideoRtpSession(address, (ost::tpport_t) 5000);

	// No decoder is attached to this depayloader.
	sfl::H264GstDecoder decoder;
	session->registerDecoder("H264", decoder);

	// Simulate the arrival of an SDP offer
	sfl::RtpMap rtpmap("96", "H264", 9000, "");
	sfl::Fmtp fmtp("96", "profile-level-id=42A01E; packetization-mode=0; sprop-parameter-sets=Z0IACpZTBYmI,aMljiA==");
	session->addSessionCodec(rtpmap, fmtp);

	session->start();

//	if (system("./server.sh >> /dev/null &") < 0) {
//		CPPUNIT_FAIL("Failed to start server in video RTP test.");
//	}

	sleep(10);

//	if (system("killall gst-launch-0.10") < 0) {
//		CPPUNIT_FAIL("Failed to stop server in video RTP test.");
//	}

	delete session;
}
