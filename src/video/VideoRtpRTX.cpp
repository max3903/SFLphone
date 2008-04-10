/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Alexis S. Bourrelle <bourrelle@polymtl.ca>
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
#include "VideoRtpRTX.h"


#define PIC_WIDTH 352
#define PIC_HEIGHT 288
#define FRAME_SIZE  (PIC_WIDTH*PIC_HEIGHT*3) //frame size of the RGB picture

VideoRtpRTX::VideoRtpRTX(SIPCall *sipcall, bool sym)
{
  setCancel(cancelDeferred);
  vidCall = sipcall;
  _sym = sym;
  std::string localipConfig = vidCall->getLocalIp();
  ost::InetHostAddress local_ip(localipConfig.c_str());

  if (!_sym) {
    videoSessionReceive = new ost::RTPSession(local_ip, vidCall->getLocalVideoPort());
    videoSessionSend = new ost::RTPSession(local_ip, vidCall->getLocalVideoPort());
  }
  else
    session = new ost::SymmetricRTPSession(local_ip, vidCall->getLocalVideoPort());
  
  cmdCapture = (Capture*) VideoDeviceManager::getInstance()->getCommand(VideoDeviceManager::CAPTURE);
  cmdRes= (Resolution*) VideoDeviceManager::getInstance()->getCommand(VideoDeviceManager::RESOLUTION);
  
  this->_Active= false;
  this->_OkToKill= false;
  
}

VideoRtpRTX::~VideoRtpRTX()
{
  //semStart.wait();

  try {
    this->terminate();
  } catch(...) {
    _debugException("! ARTP: Video Thread destructor didn't terminate correctly");
    throw;
  }
  _debug("terminate videortprtx ended...\n");
  vidCall = 0;

  if(data_to_display != NULL) 
  	free(data_to_display); data_to_display = NULL;
  if(data_from_wc != NULL)
  free(data_from_wc); data_from_wc = NULL;
  if(data_from_peer != NULL)
  free(data_from_peer); data_from_peer = NULL;
  if(data_to_send!= NULL)
  free(data_to_send); data_to_send = NULL;

   if (!_sym) {
    delete videoSessionReceive; videoSessionReceive = NULL;
    delete videoSessionSend; videoSessionSend = NULL;
   }
   else
    delete session; session = NULL;

}

void VideoRtpRTX::run(){

  // Loading codecs
  loadCodec((CodecID)CODEC_ID_H263,0);
  loadCodec((CodecID)CODEC_ID_H263,1);
  
  // Initializing buffers
  initBuffers();
  
  try {
    // Initializing the RTP session
    initVideoRtpSession();

    // Starting Session
    if (!_sym) {
      videoSessionReceive->startRunning();
      videoSessionSend->startRunning();
    } else
      session->startRunning();

    timestamp = 0;
    //if (!_sym) {
      //uint32 tstampInc = videoSessionSend->getCurrentRTPClockRate()/ 24;
    //else
      uint32 tstampInc = session->getCurrentRTPClockRate()/ 10;

    _debug("- ARTP Action: Start (video)\n");
    
    this->_Active= true;
    this->_OkToKill= false;

    while (!testCancel() && this->_Active) {

      ////////////////////////////
      // Send session
      ////////////////////////////
      sendSession();
      timestamp += tstampInc;
      
      ////////////////////////////
      // Recv session
      ////////////////////////////
      receiveSession();
      
      Thread::sleep(24);
      
    }

    free(data_to_display);data_to_display= NULL;
    free(data_from_wc);data_from_wc= NULL;
    free(data_from_peer);data_from_peer= NULL;
    free(data_to_send);data_to_send= NULL;

    unloadCodec((CodecID)CODEC_ID_H263,0);
    unloadCodec((CodecID)CODEC_ID_H263,1);
    _debug("stop stream for videortp loop\n");
    
    this->_OkToKill= true;

  } catch(std::exception &e) {
    _debug("! ARTP: Stop %s\n", e.what());
    throw;
  } catch(...) {
    _debugException("* ARTP Action: Stop");
    throw;
  }

}
	
void VideoRtpRTX::initBuffers()
{
  data_to_display = (unsigned char *)malloc(FRAME_SIZE);
  data_from_wc = (unsigned char *)malloc(FRAME_SIZE);
  data_to_send = (unsigned char *)malloc(FRAME_SIZE);
  data_from_peer = (unsigned char *)malloc(FRAME_SIZE);
  rcvWorkingBuf = (unsigned char *)malloc(FRAME_SIZE);
  
  peerBufLen=0;
  workingBufLen=0;
}
	
void VideoRtpRTX::initVideoRtpSession()
{
  try {
    if (vidCall == 0) { return; }

    VideoDevMng = VideoDeviceManager::getInstance();
    
    ost::InetHostAddress remote_ip(vidCall->getRemoteIp().c_str());
    if (!remote_ip) {
      _debug("! ARTP Video Thread Error: Target IP address [%s] is not correct!\n",vidCall->getRemoteIp().c_str());
      return;
    }

    // Initialization
    if (!_sym) {
      // Receive Session
      videoSessionReceive->setSchedulingTimeout (10000);
      videoSessionReceive->setExpireTimeout(1000000);
      if ( !videoSessionReceive->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort()) ) {
	_debug("Video RTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteVideoPort());
	return;
      }
      if ( !videoSessionReceive->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) 34)) ) {
	_debug("Video RTP Thread Error: could not set session PayloadFormat\n");
	return;
      }

      // Send Session
      videoSessionSend->setSchedulingTimeout(10000);
      videoSessionSend->setExpireTimeout(1000000);
      if (!videoSessionSend->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort())) {
	_debug("! Video ARTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteVideoPort());
	return;
      }
      if ( !videoSessionSend->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) 34)) ) {
	_debug("Video RTP Thread Error: could not set session PayloadFormat\n");
	return;
      }
    }
    else{
      // Symmetric Session
      session->setSchedulingTimeout(10000);
      session->setExpireTimeout(1000000);

      if ( !session->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort()) ) {
	_debug("Video RTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteVideoPort());
	return;
      }
      if ( !session->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) ost::sptH263) ) ) {
	_debug("Video RTP Thread Error: could not set symmetric session PayloadFormat\n");
	return;
      }
      
      session->setSessionBandwidth(768000);
    }

  } catch(...) {
    _debug("! ARTP Failure: video initialisation failed");
    throw;   
  }

}

void VideoRtpRTX::sendSession()
{

  int sizeV4L= 0;
  int encodedSize=0;

  if (vidCall==0) { 
    _debug(" !ARTP: No call associated (video)\n");
    return; 
  }
 
  try{
  _debug("Entering Send Session\n");
  
  printf("test0\n");
  // Getting Image from web cam
  data_from_wc = cmdCapture->GetCapture(sizeV4L);
  printf("test1\n");
  if( sizeV4L <= 0 )
  	return;
  printf("test2\n");
  // Getting webcam resolution information
  pair<int,int> Res = cmdRes->getResolution();
  
  printf("test3\n");
  // Putting captured data into mixer
  this->vidCall->getRemote_Video_Input()->putData( data_from_wc, sizeV4L, 0, Res.first, Res.second );
  
  printf("test4\n");
  int videoSize= -1;
  int width= 0, height= 0;
  
  printf("test5\n");
  // Getting Mixer video output
  unsigned char* dataToSend= this->vidCall->getRemote_Video_Output()->fetchData(videoSize, width, height);
  
  printf("test6\n");
  // Encode it
  if( videoSize > 0 ){
  	
  	printf("test6.1\n");
  	encodedSize = encodeCodec->videoEncode(dataToSend,(unsigned char*)data_to_send,width,height);
  	
  	printf("test6.2\n");
  	pair<int,int> ResEnc = encodeCodec->getOutputResolution();
  	
  	_debug("Widht: %d, Height: %d\n",ResEnc.first,ResEnc.second);

    unsigned char *packet;
    packet = new unsigned char[4+encodedSize];
    memcpy(packet+4,data_to_send,encodedSize);
    // TODO: Construire entierement le header du packet
    packet[0]=0;
    packet[1]=setHeaderPictureFormat(ResEnc);
    packet[2]=0;
    packet[3]=0;
      
    session->setMark(true);

  // Send it
    if (!_sym)
      videoSessionSend->sendImmediate(timestamp, packet, encodedSize+4);
    else
       session->sendImmediate(timestamp, packet, encodedSize+4);
       
    delete packet;
  }
     
  } catch(...) {
    _debugException("! ARTP: video sending failed");
    throw;
  }
}
void VideoRtpRTX::receiveSession()
{
  int PictureFormat=0;
  unsigned char TestFormat;
	
  if (vidCall==0) { 
    _debug(" !ARTP: No call associated (video)\n");
    return; 
  }

  try {
    const ost::AppDataUnit* adu = NULL;

    // Lit les donnes recues
    if (!_sym)
      adu = videoSessionReceive->getData(videoSessionReceive->getFirstTimestamp());
    else{
      adu = session->getData(session->getFirstTimestamp());
    }
    
    if (adu==NULL)
       return;
    
    isMarked = adu->isMarked();
    
    // On jumelle les donnes partielles recues
    rcvWorkingBuf  = (unsigned char*)adu->getData();
    workingBufLen = adu->getSize();
    memcpy(data_from_peer+peerBufLen,rcvWorkingBuf,workingBufLen);
    peerBufLen+=workingBufLen;
    
    
    // Analyse packet and retreive the picture format
    TestFormat = rcvWorkingBuf[1] and 128;
    if (TestFormat==128)
      PictureFormat += 128;
    TestFormat = rcvWorkingBuf[1] and 64;
    if (TestFormat==64)
      PictureFormat += 64;
    TestFormat = rcvWorkingBuf[1] and 32;
    if (TestFormat==32)
      PictureFormat += 32;
    
    pair<int,int> Res = getPictureFormatFromHeader(PictureFormat);
    
    // Decode it
    if (isMarked) {
    	
      int decodedSize= decodeCodec->videoDecode(data_from_peer,data_to_display,peerBufLen,Res.first,Res.second);
      
      if( decodedSize >= 0 ){
        this->vidCall->getLocal_Video_Input()->putData( data_to_display, decodedSize, 0, 320, 240  );
      }
      peerBufLen=0;
    }

    delete adu; adu = NULL;
    
  } catch(...) {
    _debugException("! ARTP: receiving failed");
    throw;
  }
}

void VideoRtpRTX::loadCodec(enum CodecID id,int type)
{
  if (type==0)  //decode
    decodeCodec = new VideoCodec("h263");
  else  //encode
    encodeCodec = new VideoCodec("h263");
}

void VideoRtpRTX::unloadCodec(enum CodecID id,int type)
{
  if (type==0)  //decode
  {
    delete decodeCodec;
    decodeCodec = NULL;
  }
  else  //encode
  {
    delete encodeCodec;
    encodeCodec = NULL;
  }
}

pair<int,int> VideoRtpRTX::getPictureFormatFromHeader(int SRC){

  pair<int,int> Return;

  if (SRC==160){ Return.first=1408; Return.second=1152; return Return;}
  if (SRC==128){ Return.first=704; Return.second=576; return Return;}
  if (SRC==96){ Return.first=352; Return.second=288; return Return;}
  if (SRC==64){ Return.first=176; Return.second=144; return Return;}
}

int VideoRtpRTX::setHeaderPictureFormat(pair<int,int> Res){
	if (Res.first==176 && Res.second==144)
	  return 64; // Bit7=0, Bit6=1, Bit5=0
	if (Res.first==352 && Res.second==288)
	  return 96; // Bit7=0, Bit6=1, Bit5=1
	if (Res.first==704 && Res.second==576)
	  return 128; // Bit7=1, Bit6=0, Bit5=0
	if (Res.first==1408 && Res.second==1152)
	  return 160; // Bit7=1, Bit6=0, Bit5=1
}

void VideoRtpRTX::stop(){
	
	if( !this->_Active )
		return;
	
	this->_Active= false;
	
	while(!this->_OkToKill);
		
}


