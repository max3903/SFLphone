/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean-Francois Blanchard-Dionne <jean-francois.blanchard-dionne@polymtl.ca>
 *                                                                              
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                                
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                              
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/**
 *  VideoTrpTTX Class
 * 
 * This class is a thread of execution for sending and receiving video data.
 * It has to interact with local capture (V4l) the mixer and the RTPsessions.
 * 
 */

#ifndef VIDEORTPRTX_H
#define VIDEORTPRTX_H

#include "VideoCodec/VideoCodec.h"
#include "VideoCodecDescriptor.h"
//#include "VideoRtp.h"
#include "../sipcall.h"
#include <cc++/thread.h>
#include <ccrtp/rtp.h>
#include "V4L/VideoDeviceManager.h"
//#include "../mixer/VideoInput.h"
//#include "../mixer/VideoOutput.h"
#include "../memmanager/MemManager.h"


//#include <signal.h>
//#include <stdlib.h>

class SIPCall; //TODO: pourquoi pas de include SipCall..h????

class VideoRtpRTX : public ost::Thread, public ost::TimerPort {
public:

	/**
	 * Destructor
	 */ 	
    ~VideoRtpRTX();
	/**
	 * Default Constructor
	 */ 
    VideoRtpRTX(SIPCall *sipcall, bool sym);
	/**
	 * Main function to init RTPSession, send and receive data
	 */ 
    void run();
	/**
	 * Function to init buffer size
	 */ 
    void initBuffers();
	/**
	 * Function to create RTP Session to send Video Packets
	 */ 
    void initVideoRtpSession();
    /**
	 * Function to Start the thread
	 */ 
    void Start();
    /**
	 * Function to stop the thread
	 */ 
    void Stop();
    

private:

    ost::Mutex          threadMutex;
    SIPCall* 		vidCall;
    /** RTP Session to send */
    ost::RTPSession* 	videoSessionSend;
    /** RTP Session to receive */
    ost::RTPSession* 	videoSessionReceive;
    /** System Semaphore */
    ost::Semaphore 	semStart;

    /** SYMMETRIC RTP Session to send/receive */
    ost::SymmetricRTPSession* session;

    bool _sym;

    /** Codec for encoding */
    VideoCodec* 	encodeCodec;
    /** Codec for decoding */
    VideoCodec* 	decodeCodec;
    /** Codec Context **/
    AVCodecContext*	codecCtx;
    /** Video Device manager **/
    VideoDeviceManager* VideoDevMng;
    
    Resolution* cmdRes;
    Capture* cmdCapture;

    uint32 timestamp;

    unsigned char *data_to_send;
    unsigned char *data_from_peer;
    unsigned char *data_from_wc;
    unsigned char *data_to_display;
    unsigned char *rcvWorkingBuf;

    bool isMarked;
    int peerBufLen;
    int workingBufLen;
    
	/**
	 * Get the data from V4l, send it to the mixer, encode and send to RTP
	 * @param timestamp : puts the current time
	 */
	void sendSession();
	/**
	 * Receive RTP packet, decode it, send it to mixer
	 */
	void receiveSession();

	/**
	 * Load  a codec
	 * @param id : The ID of the codec you want to load
	 * @param type : 0 decode codec, 1 encode codec
	 */
	void loadCodec(enum CodecID id,int type);

	/**
	 * unloadCodec 
	 * @param id : The ID of the codec you want to unload
	 * @param type : 0 decode codec, 1 encode codec
	 */
	void unloadCodec(enum CodecID id,int type);

};
#endif //VIDEORTPRTX_H
