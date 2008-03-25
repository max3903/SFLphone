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



VideoRtpRTX::VideoRtpRTX(SIPCall *sipcall, bool sym)
{
  setCancel(cancelDeferred);
  //time = new ost::Time();
  vidCall = sipcall;
  //_sym = sym;
  std::string localipConfig = vidCall->getLocalIp();
  ost::InetHostAddress local_ip(localipConfig.c_str());

  //TODO: Ajouter la symmetric symmetric!!
    videoSessionReceive = new ost::RTPSession(local_ip, vidCall->getLocalVideoPort());
    videoSessionSend = new ost::RTPSession(local_ip, vidCall->getLocalVideoPort());
}

VideoRtpRTX::~VideoRtpRTX()
{
  semStart.wait();

  try {
    this->terminate();
  } catch(...) {
    //_debugException("! ARTP: Thread destructor didn't terminate correctly");
    throw;
  }
  //_debug("terminate videortprtx ended...\n");
  vidCall = 0;

   //TODO: Symmetric session
    delete videoSessionReceive; videoSessionReceive = NULL;
    delete videoSessionSend; videoSessionSend = NULL;

  delete [] sendDataEncoded; sendDataEncoded = NULL;
  delete [] receiveDataDecoded; receiveDataDecoded = NULL;

}


void VideoRtpRTX::run(){
  	
  //Getting Basic AVCodecContext settings from Video Call
  codecCtx = vidCall->getVideoCodecContext();

  // Loading codecs
  loadCodec(codecCtx->codec_id,0);
  loadCodec(codecCtx->codec_id,1);
  
  initBuffers();
  int step;

  try {
    // Init the session
    initVideoRtpSession();
    step = (int) ( codecCtx->frame_size * codecCtx->sample_rate / 1000 ); // TODO: à vérifier!!!!
    
    // start running the packet queue scheduler.
    videoSessionReceive->startRunning();
    videoSessionSend->startRunning();

    int timestamp = 0; // pour V4L
    TimerPort::setTimer(codecCtx->frame_size); // TODO: à vérifier si nescessaire

    //_start.post();
    // TODO: MIXER START
    // _debug("- ARTP Action: Start\n");
    while (true) { // a changer jimagine...

      ////////////////////////////
      // Send session
      ////////////////////////////
      sendSession(timestamp);
      timestamp += step;

      ////////////////////////////
      // Recv session
      ////////////////////////////
      receiveSession();

      // Let's wait for the next transmit cycle
      Thread::sleep(TimerPort::getTimer());
      TimerPort::incTimer(codecCtx->frame_size); // 'frameSize' ms
    }
    //unloadCodec();
    //_debug("stop stream for audiortp loop\n");
    // TODO: MIXER STOP
  } catch(std::exception &e) {
    //_start.post();
    _debug("! ARTP: Stop %s\n", e.what());
    throw;
  } catch(...) {
    //_start.post();
    _debugException("* ARTP Action: Stop");
    throw;
  }
}
	
void VideoRtpRTX::initBuffers()
{
  // Input & output for the mixers TODO: Il faut les associer au mixer!!!
    localVideoInput = new VideoInput();
    remoteVideoInput = new VideoInput();
    localVideoOutput = new VideoOutput();
    remoteVideoOutput = new VideoOutput();

  // TODO: à faire
  // sendDataDecoded = new ...
  // sendDataEncoded = new ...
}
	
void VideoRtpRTX::initVideoRtpSession()
{
  
  try {
    if (vidCall == 0) { return; }

    // TODO: J'imagine qu'il ne faudrait pas le hardcoder, checker CodecContext...
    codecClockRate = 90000;

    // TODO: A verifier!
    VideoDevMng = VideoDevMng->getInstance();

    ost::InetHostAddress remote_ip(vidCall->getRemoteIp().c_str());
    if (!remote_ip) {
      //_debug("! ARTP Thread Error: Target IP address [%s] is not correct!\n", _ca->getRemoteIp().data());
      return;
    }

    // Initialization
     //TODO: Symmetric session
      videoSessionReceive->setSchedulingTimeout (10000);
      videoSessionReceive->setExpireTimeout(1000000);

      videoSessionSend->setSchedulingTimeout(10000);
      videoSessionSend->setExpireTimeout(1000000);


      //TODO: Symmetric session
      if ( !videoSessionReceive->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort()) ) {
	//_debug("Video RTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteAudioPort());
	return;
      }
      if (!videoSessionSend->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort())) {
	//_debug("! Video ARTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteAudioPort());
	return;
      }

      bool payloadIsSet = false;
      if (encodeCodec!=NULL && decodeCodec!=NULL) {
	  payloadIsSet= videoSessionReceive->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) codecCtx->rtp_payload_size));
	  payloadIsSet = videoSessionSend->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) codecCtx->rtp_payload_size));
      }
  } catch(...) {
    //_debugException("! ARTP Failure: initialisation failed");
    throw;   
  }
}
	 	
void VideoRtpRTX::sendSession(int timestamp)
{
  // no call, so we do nothing
  if (vidCall==0) { 
    _debug(" !ARTP: No call associated (video)\n");
    return; 
  }
 
  try{
  
  // Get Data from V4l, send it to the mixer input
  Command* cmdCapture = VideoDevMng->getCommand(VideoDeviceManager::CAPTURE);
  char* charFromV4L; // =  cmdCapture->GetCapture(); A FAIRE!!!
  int sizeV4L; // A prendre de quelque part!
  // Send it to the mixer
  localVideoInput->putData(charFromV4L,sizeV4L,timestamp);

  // Encode it TODO: Verifier largeur, longeur
  encodeCodec->videoEncode(codecCtx->width,codecCtx->height,(uint8_t*)charFromV4L,sizeV4L);

  //TODO: sendDataEncoded = ???????

  // Send it
  //TODO: ajouter Symmetric session
      videoSessionSend->putData(timestamp, sendDataEncoded, sizeV4L);

  } catch(...) {
    _debugException("! ARTP: sending failed");
    throw;
  }
}

		
void VideoRtpRTX::receiveSession()
{
  
  // no call, so we do nothing
  if (vidCall==0) { 
    _debug(" !ARTP: No call associated (video)\n");
    return; 
  }
  
  try {
    const ost::AppDataUnit* adu = NULL;

    //TODO: ajouter Symmetric session
      adu = videoSessionReceive->getData(videoSessionReceive->getFirstTimestamp());

    if (adu == NULL) {
      //_debug("No RTP video stream\n");
      return;
    }

    int payload = adu->getType(); // codec type
    char* data  = (char*)adu->getData(); // data in char
    unsigned int size = adu->getSize(); // size in char
    int timestamp=0; //TODO: a lire sur le paquet ! important...

    // DECODE
    // A mes yeux il manque les size des buffers sur cet appel.
    //decodeCodec->videoDecode(data,codecCtx->width,codecCtx->height,...);  //TODO: a faire!!

    // Envoyer dans le input du mixer!
    remoteVideoInput->putData(data,size,timestamp);
   
    delete adu; adu = NULL;
  } catch(...) {
    _debugException("! ARTP: receiving failed");
    throw;
  }


}

void VideoRtpRTX::loadCodec(enum CodecID id,int type)
{
  if (type==0)  //decode
    decodeCodec = new VideoCodec(avcodec_find_decoder(id));
  else  //encode
    encodeCodec = new VideoCodec(avcodec_find_encoder(id));
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
