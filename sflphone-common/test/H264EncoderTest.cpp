#include "H264EncoderTest.h"

#include "sip/sdp/RtpMap.h"
#include "sip/sdp/Fmtp.h"

#include "video/source/VideoFrame.h"
#include "video/source/VideoFormat.h"
#include "video/codec/GstCodecH264.h"
#include "video/rtp/VideoRtpSessionSimple.h"
#include "video/source/VideoInputSourceGst.h"


#include "util/Dimension.h"

#include <ccrtp/rtp.h>

void H264EncoderTest::setUp() {
	std::cout << "Setting up..." << std::endl;

	if (system("./client-h264.sh >> /dev/null &") < 0) {
		CPPUNIT_FAIL("Failed to start client in video RTP test.");
	}
}

void H264EncoderTest::tearDown() {
	std::cout << "Tearing down..." << std::endl;

	if (system("killall gst-launch-0.10") < 0) {
		CPPUNIT_FAIL("Failed to stop client in video RTP test.");
	}
}

void H264EncoderTest::testSend()
{
	std::cout << "Testing encoding and network transmission ... " << std::endl;

	// Create a video source from which we will encode
	sfl::VideoInputSourceGst source;
	source.setDevice("/dev/video0", sfl::Dimension(320, 240), sfl::FrameRate(30,1));

	// Create a video session
	ost::InetHostAddress address("127.0.0.1");
	sfl::VideoRtpSession* session = new sfl::VideoRtpSession(address, (ost::tpport_t) 5055);

	// Set the video format for this RTP session
	sfl::VideoFormat format = source.getOutputFormat();
	session->setVideoInputFormat(format);

	// Add a destination for the packets
	session->addDestination(address, (ost::tpport_t) 5000);

	// Register supported codecs for this session.
	sfl::GstCodecH264* codec = new sfl::GstCodecH264();
	codec->activate();
	codec->setVideoInputFormat(format);
	session->addSessionCodec(96, codec);

	// Receive data
	session->start();

	// Observer type for frames events
	class WebCamObserver : public sfl::VideoFrameObserver {
	public:
		WebCamObserver(sfl::VideoRtpSession* session) : session(session) {}
		void onNewFrame (const sfl::VideoFrame* frame) {
			session->sendPayloaded(frame);
		}
		sfl::VideoRtpSession* session;
	};

	source.addVideoFrameObserver(new WebCamObserver(session));

	// Let the frames flow into the encoder
	source.open();

	sleep(30);

	codec->deactivate();
	delete codec;
	delete session;
}
