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
  //_debug("terminate audiortprtx ended...\n");
  vidCall = 0;

   //TODO: Symmetric session
    delete videoSessionReceive; videoSessionReceive = NULL;
    delete videoSessionSend; videoSessionSend = NULL;

  delete [] sendDataEncoded; sendDataEncoded = NULL;
  delete [] receiveDataDecoded; receiveDataDecoded = NULL;

  //delete time; time = NULL;

}


void VideoRtpRTX::run(){
	//test Commit
}
	
void VideoRtpRTX::initBuffers(){}
	
void VideoRtpRTX::initVideoRtpSession()
{
  
  try {
    if (vidCall == 0) { return; }

    //Getting Basic AVCodecContext settings from Video Call
    codecCtx = vidCall->getVideoCodecContext();
   
    // TODO: à vérifier si c'est les mêmes codec tout le temps... V4L pour decode?
    encodeCodec = new VideoCodec(avcodec_find_encoder(codecCtx->codec_id));
    decodeCodec = new VideoCodec(avcodec_find_decoder(codecCtx->codec_id));

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
 
  //VideoDeviceManager::TCommand test(VideoDeviceManager::CAPTURE);
  Command* cmdCapture = VideoDevMng->getCommand(VideoDeviceManager::CAPTURE); // enum....
  //unsigned char* test =  cmdCapture->GetCapture();

  //TODO: Symmetric session
      //videoSessionSend->putData(timestamp, _sendDataEncoded, compSize);


  /*
  try {
    int16* toSIP = NULL;

    timestamp += time->getSecond();
    if (_ca==0) { _debug(" !ARTP: No call associated (mic)\n"); return; } // no call, so we do nothing
    AudioLayer* audiolayer = Manager::instance().getAudioDriver();
    if (!audiolayer) { _debug(" !ARTP: No audiolayer available for mic\n"); return; }

    if (!_audiocodec) { _debug(" !ARTP: No audiocodec available for mic\n"); return; }

    // we have to get 20ms of data from the mic *20/1000 = /50
    int maxBytesToGet = _layerSampleRate * _layerFrameSize * sizeof(SFLDataFormat) / 1000;
    // available bytes inside ringbuffer
    int availBytesFromMic = audiolayer->canGetMic();
    //printf("%i \n", availBytesFromMic);

    // take the lowest
    int bytesAvail = (availBytesFromMic < maxBytesToGet) ? availBytesFromMic : maxBytesToGet;
    //printf("%i\n", bytesAvail);
    // Get bytes from micRingBuffer to data_from_mic
    int nbSample = audiolayer->getMic(_dataAudioLayer, bytesAvail) / sizeof(SFLDataFormat);
    int nb_sample_up = nbSample;
    int nbSamplesMax = _layerFrameSize * _audiocodec->getClockRate() / 1000;

    nbSample = reSampleData(_audiocodec->getClockRate(), nb_sample_up, DOWN_SAMPLING);	

    toSIP = _intBufferDown;

    if ( nbSample < nbSamplesMax - 10 ) { // if only 10 is missing, it's ok
      // fill end with 0...
      //_debug("begin: %p, nbSample: %d\n", toSIP, nbSample);
      memset(toSIP + nbSample, 0, (nbSamplesMax-nbSample)*sizeof(int16));
      nbSample = nbSamplesMax;
    }
    // debug - dump sound in a file
    //_debug("AR: Nb sample: %d int, [0]=%d [1]=%d [2]=%d\n", nbSample, toSIP[0], toSIP[1], toSIP[2]);
    // for the mono: range = 0 to RTP_FRAME2SEND * sizeof(int16)
    // codecEncode(char *dest, int16* src, size in bytes of the src)
    int compSize = _audiocodec->codecEncode(_sendDataEncoded, toSIP, nbSample*sizeof(int16));
    //printf("jusqu'ici tout vas bien\n");

    // encode divise by two
    // Send encoded audio sample over the network
    if (compSize > nbSamplesMax) { _debug("! ARTP: %d should be %d\n", compSize, nbSamplesMax);}
    if (!_sym) {
      _sessionSend->putData(timestamp, _sendDataEncoded, compSize);
    } else {
      _session->putData(timestamp, _sendDataEncoded, compSize);
    }
    toSIP = NULL;
  } catch(...) {
    _debugException("! ARTP: sending failed");
    throw;
  } */
}

		
void VideoRtpRTX::receiveSession(){}

void VideoRtpRTX::loadCodec(enum CodecID id,int type){}

void VideoRtpRTX::unloadCodec(enum CodecID id,int type){}
