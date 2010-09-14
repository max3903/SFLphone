#include "H264DecoderTest.h"

#include "video/rtp/VideoRtpSessionSimple.h"
#include "video/codec/GstCodecH264.h"
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

	// Configure the session to have the H264 codec available.
	std::cout << "Creating new codec ... " << std::endl;

	sfl::GstCodecH264* codec = new sfl::GstCodecH264();
	session->registerCodec(codec);

	std::cout << "Codec registered." << std::endl;

	// Simulate the arrival of an SDP offer
	sfl::RtpMap rtpmap("96", "H264", 9000, "");
	sfl::Fmtp fmtp("96", "profile-level-id=42A01E; packetization-mode=0; sprop-parameter-sets=Z0IACpZTBYmI,aMljiA==");
	session->addSessionCodec(rtpmap, fmtp);

	std::cout << "Session codec added." << std::endl;

	session->start();

	if (system("./server.sh >> /dev/null &") < 0) {
		CPPUNIT_FAIL("Failed to start server in video RTP test.");
	}

	sleep(10);

	if (system("killall gst-launch-0.10") < 0) {
		CPPUNIT_FAIL("Failed to stop server in video RTP test.");
	}

	delete session;
}
