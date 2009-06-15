/*
 *  Copyright (C) 2004-2008 Savoir-Faire Linux inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author: Laurielle Lea <laurielle.lea@savoirfairelinux.com>
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
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

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <ccrtp/rtp.h>
#include <assert.h>
#include <string>
#include <cstring>
#include <math.h>
#include <dlfcn.h>
#include <iostream>
#include <sstream>

#include "../global.h"
#include "../manager.h"
#include "codecDescriptor.h"
#include "audiortp.h"
#include "audiolayer.h"
#include "ringbuffer.h"
#include "../user_cfg.h"
#include "../sipcall.h"
#include "zrtpCallback.h"

////////////////////////////////////////////////////////////////////////////////
// AudioRtp                                                          
////////////////////////////////////////////////////////////////////////////////
AudioRtp::AudioRtp() :_RTXThread(0), _symmetric(), _threadMutex()
{
}

AudioRtp::~AudioRtp (void) {
    delete _RTXThread; _RTXThread = 0;
}

void
AudioRtp::createNewSession (SIPCall *ca) {

    ost::MutexLock m(_threadMutex);

    _debug("AudioRtp::Create new rtp session\n");

    // something should stop the thread before...
    if ( _RTXThread != 0 ) { 
        _debug("**********************************************************\n");
        _debug("! ARTP Failure: Thread already exists..., stopping it\n");
        _debug("**********************************************************\n");
        delete _RTXThread; _RTXThread = 0;
    }
    
    int srtpEnable = 0;
    
    // Start RTP Send/Receive threads
    _symmetric = Manager::instance().getConfigInt(SIGNALISATION,SYMMETRIC) ? true : false;
    AccountID account_id = Manager::instance().getAccountFromCall (ca->getCallId());
    // This might be an IP-to-IP call
    if(account_id == AccountNULL) {
        srtpEnable = Manager::instance().getConfigInt(SIGNALISATION, ZRTP_IP2IP_ENABLE);
        _debug("\033[31;4m Ip-to-ip profile \033[0m\n");
        srtpEnable = 0;
    } else {
        srtpEnable = Manager::instance().getConfigInt(account_id, SRTP_ENABLE);
        _debug("\033[31;4m AccountID %s with SRTP_ENABLE %d\033[0m\n", account_id.c_str(), srtpEnable);
    }
    int keyExchange = Manager::instance().getConfigInt(account_id, SRTP_KEY_EXCHANGE);
    
    try {
        if(srtpEnable) {
            _RTXThread = new AudioRtpRTX (ca, true, true); // zrtp is only supported under symmetric mode
            _debug("Starting in zrtp mode\n");
        } else {
            _RTXThread = new AudioRtpRTX (ca, _symmetric, false);
            _debug("Starting unencrypted rtp session\n");
        }
        
    } catch(...) {
        _debugException("! ARTP Failure: when trying to create AudioRtpRTX thread");
        throw;
    }
}

int
AudioRtp::start(void)
{
    if(_RTXThread == 0) {
        _debug("! ARTP Failure: Cannot start audiortp thread since not yet created\n");
        throw AudioRtpException();
    }
    
    try {
        if (_RTXThread->start() != 0) {
            _debug("! ARTP Failure: unable to start RTX Thread\n");
            return -1;
        }
    } catch(...) {
        _debugException("! ARTP Failure: when trying to start a thread");
        throw;
    }
}

bool
AudioRtp::closeRtpSession () {

    ost::MutexLock m(_threadMutex);
    // This will make RTP threads finish.
    _debug("AudioRtp::Stopping rtp session\n");
    try {

        delete _RTXThread; _RTXThread = 0;
    } catch(...) {
        _debugException("! ARTP Exception: when stopping audiortp\n");
        throw;
    }
    AudioLayer* audiolayer = Manager::instance().getAudioDriver();
    audiolayer->stopStream();

    return true;
}


void
AudioRtp::setRecording() {

    _debug("AudioRtp::setRecording\n");
    _RTXThread->_ca->setRecording();

}

////////////////////////////////////////////////////////////////////////////////
// AudioRtpRTX Class                                                          //
////////////////////////////////////////////////////////////////////////////////
AudioRtpRTX::AudioRtpRTX (SIPCall *sipcall, bool sym, bool zrtp) : time(new ost::Time()), _ca(sipcall), _sessionSend(NULL), _sessionRecv(NULL), _session(NULL), _zsession(NULL), _start(), _sym(sym), _zrtp(zrtp), micData(NULL), micDataConverted(NULL), micDataEncoded(NULL), spkrDataDecoded(NULL), spkrDataConverted(NULL), converter(NULL), _layerSampleRate(),_codecSampleRate(), _layerFrameSize(), _audiocodec(NULL)
{

    setCancel(cancelDefault);
    // AudioRtpRTX should be close if we change sample rate
    // TODO: Change bind address according to user settings.
    // TODO: this should be the local ip not the external (router) IP

    std::string localipConfig = _ca->getLocalIp(); // _ca->getLocalIp();
    ost::InetHostAddress local_ip(localipConfig.c_str());
    if (!_sym) {
        _sessionRecv = new ost::RTPSession(local_ip, _ca->getLocalAudioPort());
        _sessionSend = new ost::RTPSession(local_ip, _ca->getLocalAudioPort());
        _session = NULL;
        _zsession = NULL;
    } else {
        _debug ("Local audio port: %i\n", _ca->getLocalAudioPort());
        if(!_zrtp) {
            _session = new ost::SymmetricRTPSession (local_ip, _ca->getLocalAudioPort());
            _zsession = NULL;
        } else {
            _zsession = new ost::SymmetricZRTPSession (local_ip, _ca->getLocalAudioPort());
            initializeZid();
            _session = NULL;
        }
        _sessionRecv = NULL;
        _sessionSend = NULL;
    }
}

void AudioRtpRTX::initializeZid(void) 
{
    std::string zidFile = std::string(HOMEDIR) + DIR_SEPARATOR_STR + "." + PROGDIR + "/" + std::string(Manager::instance().getConfigString(SIGNALISATION,ZRTP_ZIDFILE));
    
    if(_zsession->initialize(zidFile.c_str()) >= 0) {
        _debug("Register callbacks\n");
        _zsession->setEnableZrtp(true);
        _zsession->setUserCallback(new zrtpCallback(_ca));
        return;
    }   
    
    _debug("Initialization from ZID file failed. Trying to remove...\n");
    
    if(remove(zidFile.c_str())!=0) {
        _debug("Failed to remove zid file because of: %s", strerror(errno));
        throw ZrtpZidException();
    }
    
    if(_zsession->initialize(zidFile.c_str()) < 0) {
       _debug("ZRTP initialization failed\n");
       throw ZrtpZidException();
    } 
    
    //_debug("Hello hash: %s  , lenght %d\n", (_zsession->getHelloHash()).c_str(), (_zsession->getHelloHash()).lenght());
    
    return;
}

AudioRtpRTX::~AudioRtpRTX () {
    _start.wait();

    try {
        this->terminate();
    } catch(...) {
        _debugException("! ARTP: Thread destructor didn't terminate correctly");
        throw;
    }
    _ca = 0;
    if (!_sym) {
        delete _sessionRecv; _sessionRecv = NULL;
        delete _sessionSend; _sessionSend = NULL;
    } else {
        if(!_zrtp) {
            delete _session;     _session = NULL;
        } else {
            delete _zsession; _zsession = NULL;
        }
    }


    delete [] micData;  micData = NULL;
    delete [] micDataConverted;  micDataConverted = NULL;
    delete [] micDataEncoded;  micDataEncoded = NULL;

    delete [] spkrDataDecoded; spkrDataDecoded = NULL;
    delete [] spkrDataConverted; spkrDataConverted = NULL;

    delete time; time = NULL;

    delete converter; converter = NULL;

}

    void
AudioRtpRTX::initBuffers()
{
    converter = new SamplerateConverter( _layerSampleRate , _layerFrameSize );

    int nbSamplesMax = (int) (_layerSampleRate * _layerFrameSize /1000);

    micData = new SFLDataFormat[nbSamplesMax];
    micDataConverted = new SFLDataFormat[nbSamplesMax];
    micDataEncoded = new unsigned char[nbSamplesMax];

    spkrDataConverted = new SFLDataFormat[nbSamplesMax];
    spkrDataDecoded = new SFLDataFormat[nbSamplesMax];
}

float 
AudioRtpRTX::computeCodecFrameSize(int codecSamplePerFrame, int codecClockRate)
{
  return ( (float)codecSamplePerFrame * 1000.0 ) / (float)codecClockRate;
}

int
AudioRtpRTX::computeNbByteAudioLayer(float codecFrameSize)
{
    return (int)((float)_layerSampleRate * codecFrameSize * (float)sizeof(SFLDataFormat) / 1000.0);
}


int
AudioRtpRTX::processDataEncode(AudioLayer* audiolayer)
{

    // compute codec framesize in ms
    float fixed_codec_framesize = computeCodecFrameSize(_audiocodec->getFrameSize(), _audiocodec->getClockRate());

    // compute nb of byte to get coresponding to 20 ms at audio layer frame size (44.1 khz)
    int maxBytesToGet = computeNbByteAudioLayer(fixed_codec_framesize);
    
    // available bytes inside ringbuffer
    int availBytesFromMic = audiolayer->canGetMic();

    // set available byte to maxByteToGet
    int bytesAvail = (availBytesFromMic < maxBytesToGet) ? availBytesFromMic : maxBytesToGet;

    if (bytesAvail == 0)
      return 0;

    // Get bytes from micRingBuffer to data_from_mic
    int nbSample = audiolayer->getMic( micData , bytesAvail ) / sizeof(SFLDataFormat);

    // nb bytes to be sent over RTP
    int compSize = 0;

    // test if resampling is required
    if (_audiocodec->getClockRate() != _layerSampleRate) {

        int nb_sample_up = nbSample;
        // _debug("_nbSample audiolayer->getMic(): %i \n", nbSample);
    
        // Store the length of the mic buffer in samples for recording
        _nSamplesMic = nbSample;


        // int nbSamplesMax = _layerFrameSize * _audiocodec->getClockRate() / 1000; 
	 nbSample = reSampleData(micData , micDataConverted, _audiocodec->getClockRate(), nb_sample_up, DOWN_SAMPLING);

        compSize = _audiocodec->codecEncode( micDataEncoded, micDataConverted, nbSample*sizeof(int16));

    } else {
        // no resampling required

        // int nbSamplesMax = _codecFrameSize;
        compSize = _audiocodec->codecEncode( micDataEncoded, micData, nbSample*sizeof(int16));

    }

    return compSize;
}

void
AudioRtpRTX::processDataDecode(AudioLayer* audiolayer, unsigned char* spkrData, unsigned int size, int& countTime)
{

    if (_audiocodec != NULL) {

        // Return the size of data in bytes 
        int expandedSize = _audiocodec->codecDecode( spkrDataDecoded , spkrData , size );

        // buffer _receiveDataDecoded ----> short int or int16, coded on 2 bytes
        int nbSample = expandedSize / sizeof(SFLDataFormat);

        // test if resampling is required 
        if (_audiocodec->getClockRate() != _layerSampleRate) {

            // Do sample rate conversion
            int nb_sample_down = nbSample;
            nbSample = reSampleData(spkrDataDecoded , spkrDataConverted, _codecSampleRate , nb_sample_down, UP_SAMPLING);

            // Store the number of samples for recording
            _nSamplesSpkr = nbSample;

	    // put data in audio layer, size in byte
            audiolayer->putMain (spkrDataConverted, nbSample * sizeof(SFLDataFormat));

        } else {

            // Stor the number of samples for recording
            _nSamplesSpkr = nbSample;

	    // put data in audio layer, size in byte
            audiolayer->putMain (spkrDataDecoded, nbSample * sizeof(SFLDataFormat));
        }

        // Notify (with a beep) an incoming call when there is already a call 
        countTime += time->getSecond();
        if (Manager::instance().incomingCallWaiting() > 0) {
            countTime = countTime % 500; // more often...
            if (countTime == 0) {
                Manager::instance().notificationIncomingCall();
            }
        }

    } else {
        countTime += time->getSecond();
    }
}

    void
AudioRtpRTX::initAudioRtpSession (void) 
{

    try {
        if (_ca == 0) { return; }
        _audiocodec = _ca->getLocalSDP()->get_session_media ();

        if (_audiocodec == NULL) { return; }

        _codecSampleRate = _audiocodec->getClockRate();
        _codecFrameSize = _audiocodec->getFrameSize();
        

        ost::InetHostAddress remote_ip(_ca->getLocalSDP()->get_remote_ip().c_str());
        _debug("Init audio RTP session %s\n", _ca->getLocalSDP()->get_remote_ip().data());
        if (!remote_ip) {
            _debug("! ARTP Thread Error: Target IP address [%s] is not correct!\n", _ca->getLocalSDP()->get_remote_ip().data());
            return;
        }


        if (!_sym) {
            _sessionRecv->setSchedulingTimeout (10000);
            _sessionRecv->setExpireTimeout(1000000);

            _sessionSend->setSchedulingTimeout(10000);
            _sessionSend->setExpireTimeout(1000000);
        } else {
            if(!_zrtp) {
                _session->setSchedulingTimeout(10000);
                _session->setExpireTimeout(1000000);
            } else {
                _zsession->setSchedulingTimeout(10000);
                _zsession->setExpireTimeout(1000000);
            }
        }

        if (!_sym) {
            if ( !_sessionRecv->addDestination(remote_ip, (unsigned short) _ca->getLocalSDP()->get_remote_audio_port()) ) {
                _debug("AudioRTP Thread Error: could not connect to port %d\n",  _ca->getLocalSDP()->get_remote_audio_port());
                return;
            }
            if (!_sessionSend->addDestination (remote_ip, (unsigned short) _ca->getLocalSDP()->get_remote_audio_port())) {
                _debug("! ARTP Thread Error: could not connect to port %d\n", _ca->getLocalSDP()->get_remote_audio_port());
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
    
            if(!_zrtp)
            {
                if (!_session->addDestination (remote_ip, (unsigned short)_ca->getLocalSDP()->get_remote_audio_port() )) {
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
            } else {
                 if (!_zsession->addDestination (remote_ip, (unsigned short)_ca->getLocalSDP()->get_remote_audio_port() )) {
                    return;
                }

                bool payloadIsSet = false;
                if (_audiocodec) {
                    if (_audiocodec->hasDynamicPayload()) {
                        payloadIsSet = _zsession->setPayloadFormat(ost::DynamicPayloadFormat((ost::PayloadType) _audiocodec->getPayload(), _audiocodec->getClockRate()));
                    } else {
                        payloadIsSet = _zsession->setPayloadFormat(ost::StaticPayloadFormat((ost::StaticPayloadType) _audiocodec->getPayload()));
                    }
                }  
                
                _zsession->startZrtp(); 
                _ca->getLocalSDP()->set_zrtp_hash(_zsession->getHelloHash());
            }
        }


    } catch(...) {
        _debugException("! ARTP Failure: initialisation failed");
        throw;
    }    

}

    void
AudioRtpRTX::sendSessionFromMic(int timestamp)
{
    // STEP:
    //   1. get data from mic
    //   2. convert it to int16 - good sample, good rate
    //   3. encode it
    //   4. send it
    

    timestamp += time->getSecond();
    // no call, so we do nothing
    if (_ca==0) { _debug(" !ARTP: No call associated (mic)\n"); return; }

    AudioLayer* audiolayer = Manager::instance().getAudioDriver();
    if (!audiolayer) { _debug(" !ARTP: No audiolayer available for mic\n"); return; }

    if (!_audiocodec) { _debug(" !ARTP: No audiocodec available for mic\n"); return; }
  
    int compSize = processDataEncode(audiolayer);
    send(timestamp, micDataEncoded, compSize);
}

    void
AudioRtpRTX::send(uint32 timestamp, const unsigned char *micDataEncoded, size_t compSize)
{
    // putData put the data on RTP queue, sendImmediate bypass this queue
    if (!_sym) {
        // _sessionSend->putData(timestamp, micDataEncoded, compSize);
        _sessionSend->sendImmediate(timestamp, micDataEncoded, compSize);
    } else {
        // _session->putData(timestamp, micDataEncoded, compSize);
        if(!_zrtp) {    
            _session->sendImmediate(timestamp, micDataEncoded, compSize);
        } else {
            _zsession->putData(timestamp, micDataEncoded, compSize);
        }
    }
}

    void
AudioRtpRTX::receive(const ost::AppDataUnit** adu)
{
    if (!_sym) {
        *adu = _sessionRecv->getData(_sessionRecv->getFirstTimestamp());
    } else {
        if(!_zrtp) {    
            *adu = _session->getData(_session->getFirstTimestamp());
        } else {
            *adu = _zsession->getData(_zsession->getFirstTimestamp());
        }
    }
}


    void
AudioRtpRTX::receiveSessionForSpkr (int& countTime)
{

    if (_ca == 0) { return; }


    AudioLayer* audiolayer = Manager::instance().getAudioDriver();
    if (!audiolayer) { return; }

    const ost::AppDataUnit* adu = NULL;
    // Get audio data stream

    receive(&adu);
    
    if (adu == NULL) {
        _debug("No RTP audio stream\n");
        return;
    }

    //int payload = adu->getType(); // codec type
    unsigned char* spkrData  = (unsigned char*)adu->getData(); // data in char
    unsigned int size = adu->getSize(); // size in char

    processDataDecode(audiolayer, spkrData, size, countTime);

}

    int 
AudioRtpRTX::reSampleData(SFLDataFormat *input, SFLDataFormat *output,int sampleRate_codec, int nbSamples, int status)
{
    if(status==UP_SAMPLING){
        return converter->upsampleData( spkrDataDecoded , spkrDataConverted , sampleRate_codec , _layerSampleRate , nbSamples );
    }
    else if(status==DOWN_SAMPLING){
        return converter->downsampleData( micData , micDataConverted , sampleRate_codec , _layerSampleRate , nbSamples );
    }
    else
        return 0;
}

    void
AudioRtpRTX::setSASVerified(void)
{
    if(_zrtp) {
        _zsession->SASVerified();
    }   
}

    void
AudioRtpRTX::resetSASVerified(void)
{
    if(_zrtp) {
        _zsession->resetSASVerified();
    }   
}

    void
AudioRtpRTX::setConfirmGoClear(void)
{
    if(_zrtp) {
        _zsession->goClearOk();
    }   
}

    void
AudioRtpRTX::requestGoClear(void)
{
    if(_zrtp) {
        _zsession->requestGoClear();
    }   
}

    void
AudioRtpRTX::acceptEnrollment(bool accepted)
{
    if(_zrtp) {
        _zsession->acceptEnrollment(accepted);
    }   
}

    void 
AudioRtpRTX::setPBXEnrollment(bool yesNo) 
{
    if(_zrtp) {
        _zsession->setPBXEnrollment(yesNo);
    } 
}

void
AudioRtpRTX::run () {

  //mic, we receive from soundcard in stereo, and we send encoded
  //encoding before sending
  AudioLayer *audiolayer = Manager::instance().getAudioDriver();
  _layerFrameSize = audiolayer->getFrameSize(); // en ms
  _layerSampleRate = audiolayer->getSampleRate();
  
  initBuffers();
  int step; 

  int sessionWaiting;

  //try {

    // Init the session
    initAudioRtpSession();

    // step = (int) (_layerFrameSize * _codecSampleRate / 1000);
    step = _codecFrameSize;
    // start running the packet queue scheduler.
    //_debug("AudioRTP Thread started\n");
    if (!_sym) {
        _sessionRecv->startRunning();
        _sessionSend->startRunning();
    } else {
        if(!_zrtp) {
            _session->startRunning();
        } else {
            _zsession->startRunning();
        }
        //_debug("Session is now: %d active\n", _session->isActive());
    }

    int timestamp = 0; // for mic
    // step = (int) (_layerFrameSize * _codecSampleRate / 1000);
    step = _codecFrameSize;

    int countTime = 0; // for receive
    
    int threadSleep = 0;
    if (_codecSampleRate != 0)
        threadSleep = (_codecFrameSize * 1000) / _codecSampleRate;
    else
      threadSleep = _layerFrameSize;

    TimerPort::setTimer(threadSleep);

    audiolayer->startStream();
    _start.post();
    _debug("- ARTP Action: Start call %s\n",_ca->getCallId().c_str());
    while (!testCancel()) {

     
      // printf("AudioRtpRTX::run() _session->getFirstTimestamp() %i \n",_session->getFirstTimestamp());
    
      // printf("AudioRtpRTX::run() _session->isWaiting() %i \n",_session->isWaiting());
      /////////////////////
      ////////////////////////////
      // Send session
      ////////////////////////////
    
      if(!_zrtp) {
         sessionWaiting = _session->isWaiting();
      } else {
         sessionWaiting = _zsession->isWaiting();
      }

      sendSessionFromMic(timestamp); 
      timestamp += step;
      
      ////////////////////////////
      // Recv session
      ////////////////////////////
      receiveSessionForSpkr(countTime);
      
      // Let's wait for the next transmit cycle

      
      if(sessionWaiting == 1){
        // Record mic and speaker during conversation
        _ca->recAudio.recData(spkrDataConverted,micData,_nSamplesSpkr,_nSamplesMic);
      }
      else {
        // Record mic only while leaving a message
        _ca->recAudio.recData(micData,_nSamplesMic);
      }

      Thread::sleep(TimerPort::getTimer());
      // TimerPort::incTimer(20); // 'frameSize' ms
      TimerPort::incTimer(threadSleep);
      
    }
    
    audiolayer->stopStream();
    _debug("- ARTP Action: Stop call %s\n",_ca->getCallId().c_str());
  //} catch(std::exception &e) {
    //_start.post();
    //_debug("! ARTP: Stop %s\n", e.what());
    //throw;
  //} catch(...) {
    //_start.post();
    //_debugException("* ARTP Action: Stop");
    //throw;
  //}
    
}


// EOF
