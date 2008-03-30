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
    //videoSessionReceive = new ost::RTPSession(local_ip, vidCall->getLocalVideoPort());
    //videoSessionSend = new ost::RTPSession(local_ip, vidCall->getLocalVideoPort());
    session = new ost::SymmetricRTPSession(local_ip, vidCall->getLocalVideoPort());
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
    //delete videoSessionReceive; videoSessionReceive = NULL;
    //delete videoSessionSend; videoSessionSend = NULL;
    delete session; session = NULL;

  delete [] sendDataEncoded; sendDataEncoded = NULL;
  delete [] receiveDataDecoded; receiveDataDecoded = NULL;

}


void VideoRtpRTX::run(){

  _debug("IL ROULE!!!!!!!!!!!!!");
  
  //Getting Basic AVCodecContext settings from Video Call
  codecCtx = vidCall->getVideoCodecContext();

  if (codecCtx==NULL)
    _debug("codecCtx EST NULLLL!!!!");

  // Loading codecs  TODO: ATTENTE DE JF!
  //loadCodec(codecCtx->codec_id,0);
  //loadCodec(codecCtx->codec_id,1);
  
  initBuffers();
  int step;
 
  try {
    // Init the session
    initVideoRtpSession();
    //step = (int) ( codecCtx->frame_size * codecCtx->sample_rate / 1000 ); // TODO: à vérifier!!!!
  
    // start running the packet queue scheduler.
    //videoSessionReceive->startRunning();
    //videoSessionSend->startRunning();

    session->startRunning();

    cout << "The RTP queue Send/Receive is ";
                if( session->isActive() ==true)
                        cout << "active." << endl;
                else
                        cout << "not active." << endl;


    //int timestamp = 0; // pour V4L
    //TimerPort::setTimer(codecCtx->frame_size); // TODO: à vérifier si nescessaire

    semStart.post();
    // _debug("- ARTP Action: Start\n");


    // a enlever
    const ost::AppDataUnit* adu = NULL;
    int payload;
    char* data;
    int size;
    int timestamp=0; //TODO: a lire sur le paquet ! important...

    while (true) { // TODO: a changer jimagine...

      _debug("RUN RTPX!!!!!!!!");

      ////////////////////////////
      // Send session
      ////////////////////////////
      sendSession(timestamp);
      //timestamp += step;
  
      //videoSessionSend->putData(timestamp, "TEST", 4);

      ////////////////////////////
      // Recv session
      ////////////////////////////
      //receiveSession();



    // Lit les donnes recues
    adu = session->getData(session->getFirstTimestamp());

    if (adu == NULL) {
      _debug("No RTP video stream\n");
    }
    else  {
      payload = adu->getType(); // codec type
      data  = (char*)adu->getData(); // data in char
      size = (int)adu->getSize(); // size in char
      //timestamp=0; //TODO: a lire sur le paquet ! important...

       cout << "VIDEO PAYLOAD is " << payload << " Data is " << data << " SIZE IS " << size << endl;
     }

      // Let's wait for the next transmit cycle
      Thread::sleep(50);
     // TimerPort::incTimer(codecCtx->frame_size); // 'frameSize' ms // todo: vraiment pas sur
    }
    //unloadCodec();
    //_debug("stop stream for audiortp loop\n");
  } catch(std::exception &e) {
    semStart.post();
    _debug("! ARTP: Stop %s\n", e.what());
    throw;
  } catch(...) {
    semStart.post();
    _debugException("* ARTP Action: Stop");
    throw;
  }

}
	
void VideoRtpRTX::initBuffers()
{
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
    VideoDevMng = VideoDeviceManager::getInstance();

    ost::InetHostAddress remote_ip(vidCall->getRemoteIp().c_str());
    if (!remote_ip) {
      //_debug("! ARTP Thread Error: Target IP address [%s] is not correct!\n", _ca->getRemoteIp().data());
      return;
    }

    // Initialization
     //TODO: Symmetric session
      //videoSessionReceive->setSchedulingTimeout (10000);
      //videoSessionReceive->setExpireTimeout(1000000);

      //videoSessionSend->setSchedulingTimeout(10000);
      //videoSessionSend->setExpireTimeout(1000000);

      session->setSchedulingTimeout(10000);
      session->setExpireTimeout(1000000);

      //TODO: Symmetric session
      /*
      if ( !videoSessionReceive->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort()) ) {
	//_debug("Video RTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteAudioPort());
	return;
      }
      if (!videoSessionSend->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort())) {
	//_debug("! Video ARTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteAudioPort());
	return;
      }*/

      if ( !session->addDestination(remote_ip, (unsigned short) vidCall->getRemoteVideoPort()) ) {
	_debug("Video RTP Thread Error: could not connect to port %d\n",  vidCall->getRemoteVideoPort());
	return;
      }


      bool payloadIsSet = false;
      if (encodeCodec!=NULL && decodeCodec!=NULL) {
          // TODO: a changer!!!
	  //payloadIsSet = videoSessionReceive->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) codecCtx->rtp_payload_size));
	  //payloadIsSet = videoSessionSend->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) codecCtx->rtp_payload_size));
          payloadIsSet = session->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) 34)); //TODO: p-e 33 !?!
      }


  } catch(...) {
    //_debugException("! ARTP Failure: initialisation failed");
    throw;   
  }

}
	 	
void VideoRtpRTX::sendSession(int timestamp)
{
   _debug("SEND SESSION !!!!");

  // no call, so we do nothing
  if (vidCall==0) { 
    _debug(" !ARTP: No call associated (video)\n");
    return; 
  }
 
  try{
  // Get Data from V4l, send it to the mixer input
  Capture* cmdCapture = (Capture*) VideoDevMng->getCommand(VideoDeviceManager::CAPTURE);
  Resolution* cmdRes = (Resolution*) VideoDevMng->getCommand(VideoDeviceManager::RESOLUTION);
  int sizeV4L= 0;
  unsigned char* charFromV4L = cmdCapture->GetCapture(sizeV4L);

  // Depose les data de V4L dans le Input buffer du mixer correspondant
  //vidCall->getRemoteIntputStreams()->fetchVideoStream()->putData((char*)charFromV4L,sizeV4L,timestamp);

  // Prend les donnes de la sortie du mixer correspondant
  //vidCall->getRemoteVideoOutputStream()->fetchData((char*)sendDataEncoded);

  // Encode it TODO: Verifier largeur, longeur AVEC JF !!!
  //encodeCodec->videoEncode(codecCtx->width,codecCtx->height,(uint8_t*)charFromV4L,sizeV4L);
  //TODO: sendDataEncoded = ???? TODO: attente VideoCodec ......

  // Send it
  session->putData(timestamp, charFromV4L, sizeV4L);

  //free(charFromV4L); // TODO: A verifier !!!!

  } catch(...) {
    _debugException("! ARTP: sending failed");
    throw;
  }
}

		
void VideoRtpRTX::receiveSession()
{
  _debug("Receive SESSION !!!!");

  /*
  // no call, so we do nothing
  if (vidCall==0) { 
    _debug(" !ARTP: No call associated (video)\n");
    return; 
  }
  
  try {
    const ost::AppDataUnit* adu = NULL;

    // Lit les donnes recues
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
    // decodeCodec->videoDecode(data,codecCtx->width,codecCtx->height,...);  //TODO: a faire!!

    // Envoyer dans le input du mixer local! // TODO: a verifier
    vidCall->getLocalIntputStreams()->fetchVideoStream()->putData(data,size,timestamp);
    
    // Prend les donnes de la sortie du mixer correspondant TODO: A MODIFIER NON FONCTIONNEL!!!!!!!!!
    vidCall->getLocalVideoOutputStream()->fetchData((char*)sendDataEncoded);

    delete adu; adu = NULL;
  } catch(...) {
    _debugException("! ARTP: receiving failed");
    throw;
  }
*/
}

void VideoRtpRTX::loadCodec(enum CodecID id,int type)
{
//  if (type==0)  //decode
//    decodeCodec = new VideoCodec(avcodec_find_decoder(id));
//  else  //encode
//    encodeCodec = new VideoCodec(avcodec_find_encoder(id));
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
