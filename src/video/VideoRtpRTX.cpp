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
  //TODO: Ajouter le symmetric call!!
  //if (!_sym) {
    videoSessionReceive = new ost::RTPSession(local_ip, vidCall->getLocalVideoPort());
    videoSessionSend = new ost::RTPSession(local_ip, vidCall->getLocalVideoPort());
    //_session = NULL; //TODO: Symetric!
  //} else {
    //_session = new ost::SymmetricRTPSession (local_ip, _ca->getLocalAudioPort());
    //_sessionRecv = NULL;
    //_sessionSend = NULL;
  //}
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
  //if (!_sym) {
    delete videoSessionReceive; videoSessionReceive = NULL;
    delete videoSessionSend; videoSessionSend = NULL;
  //} else {
    //delete _session;     _session = NULL;
  //}

  //delete [] _intBufferDown; _intBufferDown = NULL;
  //delete [] _floatBufferUp; _floatBufferUp = NULL;
  //delete [] _floatBufferDown; _floatBufferDown = NULL;
  //delete [] _dataAudioLayer; _dataAudioLayer = NULL;

  delete [] sendDataEncoded; sendDataEncoded = NULL;
  delete [] receiveDataDecoded; receiveDataDecoded = NULL;

  //delete time; time = NULL;

  // libsamplerate-related
  //_src_state_mic  = src_delete(_src_state_mic);
  //_src_state_spkr = src_delete(_src_state_spkr);
}


void VideoRtpRTX::run(){
	//test Commit
}
	
void VideoRtpRTX::initBuffers(){}
	
void VideoRtpRTX::initVideoRtpSession()
{
  try {
    if (vidCall == 0) { return; }
    _audiocodec = Manager::instance().getCodecDescriptorMap().getCodec( vidCall->getVideoCodec() );
    _codecSampleRate = _audiocodec->getClockRate();






  /* 
  try {
    if (_ca == 0) { return; }
    _audiocodec = Manager::instance().getCodecDescriptorMap().getCodec( _ca->getAudioCodec() );
    _codecSampleRate = _audiocodec->getClockRate();	

    _debug("Init audio RTP session\n");
    ost::InetHostAddress remote_ip(_ca->getRemoteIp().c_str());
    if (!remote_ip) {
      _debug("! ARTP Thread Error: Target IP address [%s] is not correct!\n", _ca->getRemoteIp().data());
      return;
    }

    // Initialization
    if (!_sym) {
      _sessionRecv->setSchedulingTimeout (10000);
      _sessionRecv->setExpireTimeout(1000000);

      _sessionSend->setSchedulingTimeout(10000);
      _sessionSend->setExpireTimeout(1000000);
    } else {
      _session->setSchedulingTimeout(10000);
      _session->setExpireTimeout(1000000);
    }

    if (!_sym) {
      if ( !_sessionRecv->addDestination(remote_ip, (unsigned short) _ca->getRemoteAudioPort()) ) {
	_debug("AudioRTP Thread Error: could not connect to port %d\n",  _ca->getRemoteAudioPort());
	return;
      }
      if (!_sessionSend->addDestination (remote_ip, (unsigned short) _ca->getRemoteAudioPort())) {
	_debug("! ARTP Thread Error: could not connect to port %d\n",  _ca->getRemoteAudioPort());
	return;
      }

      bool payloadIsSet = false;
      if (_audiocodec) {
	if (_audiocodec->hasDynamicPayload()) {
	  payloadIsSet = _sessionRecv->setPayloadFormat(ost::DynamicPayloadFormat((ost::PayloadType) _audiocodec->getPayload(), _audiocodec->getClockRate()));
	} else {
	  payloadIsSet= _sessionRecv->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) _audiocodec->getPayload()));
	  payloadIsSet = _sessionSend->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) _audiocodec->getPayload()));
	}
      }
      _sessionSend->setMark(true);
    } else {

      //_debug("AudioRTP Thread: Added session destination %s\n", remote_ip.getHostname() );

      if (!_session->addDestination (remote_ip, (unsigned short) _ca->getRemoteAudioPort())) {
	return;
      }

      bool payloadIsSet = false;
      if (_audiocodec) {
	if (_audiocodec->hasDynamicPayload()) {
	  payloadIsSet = _session->setPayloadFormat(ost::DynamicPayloadFormat((ost::PayloadType) _audiocodec->getPayload(), _audiocodec->getClockRate()));
	} else {
	  payloadIsSet = _session->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) _audiocodec->getPayload()));
	}
      }
    }
  } catch(...) {
    _debugException("! ARTP Failure: initialisation failed");
    throw;
  }
 */
}
	 	
void VideoRtpRTX::sendSession(int timestamp){}

		
void VideoRtpRTX::receiveSession(){}

void VideoRtpRTX::loadCodec(enum CodecID id,int type){}

void VideoRtpRTX::unloadCodec(enum CodecID id,int type){}
