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
    
  
  this->memManager= MemManager::getInstance();
  
  vector<MemKey*> tmp= this->memManager->getAvailSpaces();
  
  vector<MemKey*>::iterator debut= tmp.begin();
  vector<MemKey*>::iterator fin= tmp.end();
  string searchstring= "remote";
  this->key= NULL;
  
  while( debut != fin ){
  	if( (*debut)->getDescription() == searchstring)
  		this->key= (*debut);
  	debut++;
  }
  
  if( key == NULL )
  	exit();

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
  //_debug("terminate videortprtx ended...\n");
  vidCall = 0;

  //free(data_to_display); data_to_display = NULL;
  free(data_from_wc); data_from_wc = NULL;
  //free(data_from_peer); data_from_peer = NULL;
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
  
 
  initBuffers();
  
  try {
    // Init the session
    initVideoRtpSession();

    if (!_sym) {
      videoSessionReceive->startRunning();
      videoSessionSend->startRunning();
    } else
      session->startRunning();

    timestamp = 0;
    uint32 tstampInc = session->getCurrentRTPClockRate()/ 24; //TODO: sym ?
    
    _debug("Initial time: %d\n",timestamp);
    _debug("VIDEO:  Current timestamp icrementation: %d\n", tstampInc);
    _debug("- ARTP Action: Start (video)\n");
    //semStart.post();

    while (!testCancel()) {

      ////////////////////////////
      // Send session
      ////////////////////////////
      sendSession();
      timestamp += tstampInc;
      
      ////////////////////////////
      // Recv session
      ////////////////////////////
     // receiveSession();
    }

    free(data_to_display);
    free(data_from_wc);
    free(data_from_peer);
    free(data_to_send);

    unloadCodec((CodecID)CODEC_ID_H263,0);
    unloadCodec((CodecID)CODEC_ID_H263,1);
    _debug("stop stream for videortp loop\n");

  } catch(std::exception &e) {
    //semStart.post();
    _debug("! ARTP: Stop %s\n", e.what());
    throw;
  } catch(...) {
    //semStart.post();
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

    // TODO: Enlever le Hardcode...
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
      session->setSchedulingTimeout(10000);  // TODO: a verifier
      session->setExpireTimeout(1000000);

      if ( !session->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort()) ) {
	_debug("Video RTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteVideoPort());
	return;
      }
      if ( !session->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) ost::sptH263) ) ) {
	_debug("Video RTP Thread Error: could not set symmetric session PayloadFormat\n");
	return;
      }
      
      //session->setTimeclock();
      session->setSessionBandwidth(768000);
      //session->setMaxSendSegmentSize(1340);
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
  // Get Data from V4l, send it to the mixer input
  Capture* cmdCapture = (Capture*) VideoDevMng->getCommand(VideoDeviceManager::CAPTURE);
  data_from_wc = cmdCapture->GetCapture(sizeV4L);
  
  //if (data_from_wc==NULL)
   //_debug("NULLLLL!!!!");
  //Resolution* cmdRes= (Resolution*)VideoDevMng->getCommand(VideoDeviceManager::RESOLUTION);
  //pair<int,int> Res = cmdRes->getResolution();

  // Depose les data de V4L dans le Input buffer du mixer correspondant
  //vidCall->getRemoteIntputStreams()->fetchVideoStream()->putData((char*)charFromV4L,sizeV4L,timestamp);

  // Prend les donnes de la sortie du mixer correspondant
  //vidCall->getRemoteVideoOutputStream()->fetchData((char*)sendDataEncoded);

  // Encode it
  encodedSize = encodeCodec->videoEncode((unsigned char*)data_from_wc,(unsigned char*)data_to_send,320,240);

   // _debug("Le timeStamp est: %d \n", timestamp);
   //_debug("Le size encode est: %d \n", encodedSize);
  
  //free(data_from_wc);

    unsigned char *packet;
    packet = new unsigned char[4+encodedSize];
    memcpy(packet+4,data_to_send,encodedSize);
    for(int i=0;i<4;i++)
      packet[i]=0;
    
    session->setMark(true);


  // Send it
    //if (!_sym)
      //videoSessionSend->putData(timestamp, data_from_wc, sizeV4L);
    //else
       //session->sendImmediate(rcvTimestamps, data_from_peer, TMPLONG);
       session->sendImmediate(timestamp, packet, encodedSize+4);
    //while(session->isSending());
    
    //delete packet;
     
     

  } catch(...) {
    _debugException("! ARTP: video sending failed");
    throw;
  }
}


void VideoRtpRTX::receiveSession()
{
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
        while(adu == NULL && !testCancel())
          adu = session->getData(session->getFirstTimestamp());
    }
    
    isMarked = adu->isMarked();
    
    rcvWorkingBuf  = (unsigned char*)adu->getData(); // data in char
    workingBufLen = adu->getSize();
    _debug("Le size du paquet: %d\n",workingBufLen);
    memcpy(data_from_peer+peerBufLen,rcvWorkingBuf,workingBufLen);
    peerBufLen+=workingBufLen;

    // Decode it
    if (isMarked) {
      if (decodeCodec->videoDecode(data_from_peer,data_to_display,peerBufLen) >= 0)
        this->memManager->putData(this->key, data_to_display, FRAME_SIZE, 320, 240);
      peerBufLen=0;
    }
    
    // Envoyer dans le input du mixer local!
    //vidCall->getLocalIntputStreams()->fetchVideoStream()->putData(data,size,timestamp);
    
    // Prend les donnes de la sortie du mixer correspondant TODO: A MODIFIER NON FONCTIONNEL!!!!!!!!!
    //vidCall->getLocalVideoOutputStream()->fetchData((char*)sendDataEncoded);

    delete adu; adu = NULL;
    
    /*
    if (rcvWorkingBuf[0] < 128){
      //_debug("--MODE A--\n");
    }
    else{
      if (rcvWorkingBuf[0] <= 192){
      	//_debug("--MODE B--\n");
      }
      else{
      	//_debug("--MODE C--\n");
      }
    }
    */
    
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


