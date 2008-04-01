/*
 *  Copyright (C) 2004-2006 Savoir-Faire Linux inc.
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author : Laurielle Lea <laurielle.lea@savoirfairelinux.com>
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
#include "call.h"

Call::Call(const CallID& id, Call::CallType type) : _id(id), _type(type), 
						    _localIPAddress(""),
						    _remoteIPAddress("")  
{
  _connectionState = Call::Disconnected;
  _callState = Call::Inactive;
  //_audioCodec = 0;
  _localAudioPort = 0;
  _localVideoPort = 0;
  _localExternalAudioPort = 0;
  _localExternalVideoPort = 0;
  _remoteAudioPort = 0;
  _remoteVideoPort = 0;

  // Buffer initialization
  this->_localInputStreams= new InputStreams( new VideoInput, new AudioInput );
  this->_remoteStandardInputStreams= new InputStreams( new VideoInput, new AudioInput );
  this->_remoteExtraInputStreams= NULL;
  
  this->_remote_Audio_Output= new AudioOutput;
  this->_remote_Video_Output= new VideoOutput;
  
  //Mixer initialization
  vector<InputStreams*> tmpInputs;
  tmpInputs.push_back(_localInputStreams);
  this->_localMixer = new Mixer(Mixer::NOSYNCH_AV_STRAIGHTTHROUGH,tmpInputs,(OutputStream*)(new LocalAudioOuput),(OutputStream*)(new LocalVideoOuput));
  //this->_localMixer->start();
  tmpInputs.clear();

  tmpInputs.push_back(_remoteStandardInputStreams);  
  this->_remoteMixer = new Mixer(Mixer::NOSYNCH_AV_STRAIGHTTHROUGH,tmpInputs,(OutputStream*)this->_remote_Audio_Output,(OutputStream*)this->_remote_Video_Output);
  //this->_remoteMixer->start();
  
}


Call::~Call()
{
 
	/*this->_localMixer->terminateThreads();
	this->_remoteMixer->terminateThreads();
	
	delete this->_localMixer;
	delete this->_remoteMixer;
 
 	delete this->_localInputStreams;
	delete this->_remoteStandardInputStreams;
	delete this->_remoteExtraInputStreams;
  
	delete this->_remote_Audio_Output;
	delete this->_remote_Video_Output;*/

}

void 
Call::setConnectionState(ConnectionState state) 
{
  ost::MutexLock m(_callMutex);
  _connectionState = state;
}

Call::ConnectionState
Call::getConnectionState() 
{
  ost::MutexLock m(_callMutex);
  return _connectionState;
}


void 
Call::setState(CallState state) 
{
  ost::MutexLock m(_callMutex);
  _callState = state;
}

Call::CallState
Call::getState() 
{
  ost::MutexLock m(_callMutex);
  return _callState;
}

CodecDescriptor& 
Call::getCodecMap()
{
  return _codecMap;
}

const std::string& 
Call::getLocalIp()
{
  ost::MutexLock m(_callMutex);  
  return _localIPAddress;
}

unsigned int 
Call::getLocalAudioPort()
{
  ost::MutexLock m(_callMutex);  
  return _localAudioPort;
}

unsigned int 
Call::getLocalVideoPort()
{
  ost::MutexLock m(_callMutex);  
  return _localVideoPort;
}

unsigned int 
Call::getRemoteAudioPort()
{
  ost::MutexLock m(_callMutex);  
  return _remoteAudioPort;
}

unsigned int 
Call::getRemoteVideoPort()
{
  ost::MutexLock m(_callMutex);  
  return _remoteVideoPort;
}

const std::string& 
Call::getRemoteIp()
{
  ost::MutexLock m(_callMutex);  
  return _remoteIPAddress;
}

AudioCodecType 
Call::getAudioCodec()
{
  ost::MutexLock m(_callMutex);  
  return _audioCodec;  
}

AVCodecContext*
Call::getVideoCodecContext()
{
  ost::MutexLock m(_callMutex);  
  return _videoCodecContext;  
}

void 
Call::setAudioStart(bool start)
{
  ost::MutexLock m(_callMutex);  
  _audioStarted = start;  
}

void 
Call::setVideoStart(bool start)
{
  ost::MutexLock m(_callMutex);  
  _videoStarted = start;  
}

bool 
Call::isAudioStarted()
{
  ost::MutexLock m(_callMutex);  
  return _audioStarted;
}

bool 
Call::isVideoStarted()
{
  ost::MutexLock m(_callMutex);  
  return _videoStarted;
}

VideoInput* Call::getLocal_Video_Input(){
	return this->_localInputStreams->fetchVideoStream();
}

AudioInput* Call::getLocal_Audio_Input(){
	return this->_localInputStreams->fetchAudioStream();
}

VideoInput* Call::getRemote_Video_Input(){
	return this->_remoteStandardInputStreams->fetchVideoStream();
}

AudioInput* Call::getRemote_Audio_Input(){
	return this->_remoteStandardInputStreams->fetchAudioStream();
}
			
VideoOutput* Call::getRemote_Video_Output(){
	return this->_remote_Video_Output;
}

AudioOutput* Call::getRemote_Audio_Output(){
	return this->_remote_Audio_Output;
}

void Call::setConfMode( VideoInput* extraVideo, AudioInput* extraAudio  ){
	//\ TODO: To implement
}
