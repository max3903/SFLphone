#include "H264DepayloaderTest.h"

#include "video/decoder/H264GstDecoder.h"
#include "video/rtp/VideoRtpSession.h"
#include "sip/sdp/RtpMap.h"
#include "sip/sdp/Fmtp.h"

#include <ccrtp/rtp.h>

void H264DepayloaderTest::setUp() {
	std::cout << "Setting up..." << std::endl;

	if (system("./server.sh >> /dev/null &") < 0) {
		CPPUNIT_FAIL("Failed to start server in video RTP test.");
	}

	sleep(2);
}

void H264DepayloaderTest::tearDown() {
	std::cout << "Tearing down..." << std::endl;

	if (system("killall gst-launch-0.10") < 0) {
		CPPUNIT_FAIL("Failed to stop server in video RTP test.");
	}
}

void H264DepayloaderTest::testReceive()
{
	std::cout << "Trying to decode frames ... " << std::endl;

	ost::InetHostAddress address("127.0.0.1");

	std::cout << "Creating session ... " << std::endl;

	sfl::VideoRtpSession* session = new sfl::VideoRtpSession(address, (ost::tpport_t) 5000);

	std::cout << "Session created." << std::endl;

	// No decoder is attached to this depayloader.
	sfl::H264GstDecoder decoder;

	std::cout << "Decoder created." << std::endl;

	std::cout << "Registering Decoder ... " << std::endl;
	session->registerDecoder("H264", decoder);

	// Simulate the arrival of an SDP offer
	sfl::RtpMap rtpmap("98", "H264", 9000, "");
	sfl::Fmtp fmtp("98", "profile-level-id=42A01E; packetization-mode=0; sprop-parameter-sets=Z0IACpZTBYmI,aMljiA==");
	session->configureFromSdp(rtpmap, fmtp);

	// Start capturing
	session->listen();
	sleep(3);
	delete session;
}
