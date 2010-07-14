#!/bin/sh
#
# A simple RTP receiver 
#
#  receives H264 encoded RTP video on port 5000, RTCP is received on  port 5001.
#  the receiver RTCP reports are sent to port 5006
#
#             .-------.      .----------.     .---------.   .-------.   .-----------.
#  RTP        |udpsrc |      | rtpbin   |     |h264depay|   |h264dec|   |xvimagesink|
#  port=5000  |      src->recv_rtp recv_rtp->sink     src->sink   src->sink         |
#             '-------'      |          |     '---------'   '-------'   '-----------'
#                            |          |      
#                            |          |     .-------.
#                            |          |     |udpsink|  RTCP
#                            |    send_rtcp->sink     | port=5005
#             .-------.      |          |     '-------' sync=false
#  RTCP       |udpsrc |      |          |               async=false
#  port=5001  |     src->recv_rtcp      |                       
#             '-------'      '----------'              


# the caps of the sender RTP stream. This is usually negotiated out of band with
# SDP or RTSP. normally these caps will also include SPS and PPS but we don't
# have a mechanism to get this from the sender with a -launch line.
VIDEO_CAPS=""

# the destination machine to send RTCP to. This is the address of the sender and
# is used to send back the RTCP reports of this receiver. If the data is sent
# from another machine, change this address.
DEST=127.0.0.1

gst-launch -v gstrtpbin name=rtpbin                                                \
	   udpsrc caps=application/x-rtp,media=video,clock-rate=90000,encoding-name=THEORA,sampling=YCbCr-4:2:0,width=368,height=288,configuration=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\\=\\=\",delivery-method=inline,ssrc=1834593755,payload=96,clock-base=568391105,seqnum-base=20093 port=5000 ! rtpbin.recv_rtp_sink_0              \
	         rtpbin. ! rtptheoradepay ! theoraparse ! theoradec ! autovideosink                 \
           udpsrc port=5001 ! rtpbin.recv_rtcp_sink_0                              \
         rtpbin.send_rtcp_src_0 ! udpsink port=5005 host=$DEST sync=false async=false
         
