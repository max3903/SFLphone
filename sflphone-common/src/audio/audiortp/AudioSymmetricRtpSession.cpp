/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010, 2011 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
 *  Author: Laurielle Lea <laurielle.lea@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#include "AudioSymmetricRtpSession.h"

#include "sip/sdp.h"
#include "audio/audiolayer.h"
namespace sfl
{

AudioSymmetricRtpSession::AudioSymmetricRtpSession (SIPCall * sipcall) :
    AudioRtpRecordHandler (sipcall)
    , ost::SymmetricRTPSession (ost::InetHostAddress (sipcall->getLocalIp().c_str()), sipcall->getLocalAudioPort())
    , _mainloopSemaphore (0)
    , _timestamp (0)
    , _timestampIncrement (0)
    , _timestampCount (0)
    , _ca (sipcall)
    , _isStarted (false)
    , _rtpThread (new AudioRtpThread (this))
{
    assert (_ca);

    _info ("AudioSymmetricRtpSession: Setting new RTP session with destination %s:%d", _ca->getLocalIp().c_str(), _ca->getLocalAudioPort());

    _audioRtpRecord._callId = _ca->getCallId();

    setTypeOfService (tosEnhanced);
}

AudioSymmetricRtpSession::~AudioSymmetricRtpSession()
{
    _info ("AudioSymmetricRtpSession: Delete AudioSymmetricRtpSession instance");
}

void AudioSymmetricRtpSession::final()
{
    delete _rtpThread;

    delete static_cast<AudioSymmetricRtpSession *> (this);
}

void AudioSymmetricRtpSession::setSessionTimeouts (void)
{
    _debug ("AudioSymmetricRtpSession: Set session scheduling timeout (%d) and expireTimeout (%d)", sfl::schedulingTimeout, sfl::expireTimeout);

    setSchedulingTimeout (sfl::schedulingTimeout);
    setExpireTimeout (sfl::expireTimeout);
}

void AudioSymmetricRtpSession::setSessionMedia (AudioCodec *audioCodec)
{
    _debug ("AudioSymmetricRtpSession: Set session media");

    // set internal codec info for this session
    setRtpMedia (audioCodec);

    // store codec info locally
    int payloadType = getCodecPayloadType();
    int frameSize = getCodecFrameSize();
    int smplRate = getCodecSampleRate();
    bool dynamic = getHasDynamicPayload();

    // G722 requires timestamp to be incremented at 8 kHz
    if (payloadType == g722PayloadType)
        _timestampIncrement = g722RtpTimeincrement;
    else
        _timestampIncrement = frameSize;

    _debug ("AudioRptSession: Codec payload: %d", payloadType);
    _debug ("AudioSymmetricRtpSession: Codec sampling rate: %d", smplRate);
    _debug ("AudioSymmetricRtpSession: Codec frame size: %d", frameSize);
    _debug ("AudioSymmetricRtpSession: RTP timestamp increment: %d", _timestampIncrement);

    if (payloadType == g722PayloadType) {
        _debug ("AudioSymmetricRtpSession: Setting G722 payload format");
        setPayloadFormat (ost::DynamicPayloadFormat ( (ost::PayloadType) payloadType, g722RtpClockRate));
    } else {
        if (dynamic) {
            _debug ("AudioSymmetricRtpSession: Setting dynamic payload format");
            setPayloadFormat (ost::DynamicPayloadFormat ( (ost::PayloadType) payloadType, smplRate));
        } else {
            _debug ("AudioSymmetricRtpSession: Setting static payload format");
            setPayloadFormat (ost::StaticPayloadFormat ( (ost::StaticPayloadType) payloadType));
        }
    }

    _ca->setRecordingSmplRate (getCodecSampleRate());
}

void AudioSymmetricRtpSession::updateSessionMedia (AudioCodec *audioCodec)
{
    _debug ("AudioSymmetricRtpSession: Update session media");

    // Update internal codec for this session
    updateRtpMedia (audioCodec);

    int payloadType = getCodecPayloadType();
    int frameSize = getCodecFrameSize();
    int smplRate = getCodecSampleRate();
    int dynamic = getHasDynamicPayload();

    // G722 requires timetamp to be incremented at 8khz
    if (payloadType == g722PayloadType)
        _timestampIncrement = g722RtpTimeincrement;
    else
        _timestampIncrement = frameSize;

    _debug ("AudioRptSession: Codec payload: %d", payloadType);
    _debug ("AudioSymmetricRtpSession: Codec sampling rate: %d", smplRate);
    _debug ("AudioSymmetricRtpSession: Codec frame size: %d", frameSize);
    _debug ("AudioSymmetricRtpSession: RTP timestamp increment: %d", _timestampIncrement);

    if (payloadType == g722PayloadType) {
        _debug ("AudioSymmetricRtpSession: Setting G722 payload format");
        setPayloadFormat (ost::DynamicPayloadFormat ( (ost::PayloadType) payloadType, g722RtpClockRate));
    } else {
        if (dynamic) {
            _debug ("AudioSymmetricRtpSession: Setting dynamic payload format");
            setPayloadFormat (ost::DynamicPayloadFormat ( (ost::PayloadType) payloadType, smplRate));
        } else {
            _debug ("AudioSymmetricRtpSession: Setting static payload format");
            setPayloadFormat (ost::StaticPayloadFormat ( (ost::StaticPayloadType) payloadType));
        }
    }


    _ca->setRecordingSmplRate (getCodecSampleRate());

    _timestamp = getCurrentTimestamp();
}


void AudioSymmetricRtpSession::setDestinationIpAddress (void)
{
    _info ("AudioSymmetricRtpSession: Setting IP address for the RTP session");

    if (_ca == NULL) {
        _error ("AudioSymmetricRtpSession: Sipcall is gone.");
        throw AudioSymmetricRtpSessionException();
    }

    // Store remote ip in case we would need to forget current destination
    _remote_ip = ost::InetHostAddress (_ca->getLocalSDP()->getRemoteIP().c_str());
    if (!_remote_ip) {
        _warn ("AudioSymmetricRtpSession: Target IP address (%s) is not correct!",
               _ca->getLocalSDP()->getRemoteIP().data());
        return;
    }

    // Store remote port in case we would need to forget current destination
    _remote_port = (unsigned short) _ca->getLocalSDP()->getRemoteAudioPort();

    _info ("AudioSymmetricRtpSession: New remote address for session: %s:%d",
           _ca->getLocalSDP()->getRemoteIP().data(), _remote_port);

    if (!addDestination (_remote_ip, _remote_port)) {
        _warn ("AudioSymmetricRtpSession: Can't add new destination to session!");
        return;
    }
}

void AudioSymmetricRtpSession::updateDestinationIpAddress (void)
{
    _debug ("AudioSymmetricRtpSession: Update destination ip address");

    // Destination address are stored in a list in ccrtp
    // This method remove the current destination entry

    if (!forgetDestination (_remote_ip, _remote_port, _remote_port+1)) {
        _warn ("AudioSymmetricRtpSession: Could not remove previous destination: %s:%d",
        						inet_ntoa(_remote_ip.getAddress()), _remote_port);
    }

    // new destination is stored in call
    // we just need to recall this method
    setDestinationIpAddress();
}

void AudioSymmetricRtpSession::sendDtmfEvent (sfl::DtmfEvent *dtmf)
{
    _debug ("AudioSymmetricRtpSession: Send Dtmf");

    _timestamp += _timestampIncrement;
    dtmf->factor++;

    // discard equivalent size of audio
    processDataEncode();

    // change Payload type for DTMF payload
    setPayloadFormat (ost::DynamicPayloadFormat ( (ost::PayloadType) getDtmfPayloadType(), 8000));

    // Set marker in case this is a new Event
    if (dtmf->newevent)
        setMark (true);

    // putData (_timestamp, (const unsigned char*) (& (dtmf->payload)), sizeof (ost::RTPPacket::RFC2833Payload));
    sendImmediate (_timestamp, (const unsigned char *) (& (dtmf->payload)), sizeof (ost::RTPPacket::RFC2833Payload));

    // This is no more a new event
    if (dtmf->newevent) {
        dtmf->newevent = false;
        setMark (false);
    }

    // get back the payload to audio
    setPayloadFormat (ost::StaticPayloadFormat ( (ost::StaticPayloadType) getCodecPayloadType()));

    // decrease length remaining to process for this event
    dtmf->length -= _timestampIncrement;

    dtmf->payload.duration++;


    // next packet is going to be the last one
    if ( (dtmf->length - _timestampIncrement) < _timestampIncrement)
        dtmf->payload.ebit = true;

    if (dtmf->length < _timestampIncrement) {
        delete dtmf;
        getEventQueue()->pop_front();
    }
}

bool AudioSymmetricRtpSession::onRTPPacketRecv (ost::IncomingRTPPkt&)
{
    receiveSpeakerData ();

    return true;
}



void AudioSymmetricRtpSession::sendMicData()
{
    int compSize = processDataEncode();

    // If no data, return
    if (!compSize)
        return;

    // Increment timestamp for outgoing packet
    _timestamp += _timestampIncrement;

    // putData put the data on RTP queue, sendImmediate bypass this queue
    // putData (_timestamp, getMicDataEncoded(), compSize);
    // _debug ("compsize = %d", compSize);
    sendImmediate (_timestamp, getMicDataEncoded(), compSize);
}


void AudioSymmetricRtpSession::receiveSpeakerData ()
{

    const ost::AppDataUnit* adu = NULL;

    int packetTimestamp = getFirstTimestamp();

    adu = getData (packetTimestamp);

    if (!adu) {
        return;
    }

    unsigned char* spkrDataIn = NULL;
    unsigned int size = 0;

    spkrDataIn  = (unsigned char*) adu->getData(); // data in char
    size = adu->getSize(); // size in char

    // DTMF over RTP, size must be over 4 in order to process it as voice data
    if (size > 4) {
        processDataDecode (spkrDataIn, size);
    }

    delete adu;
}

int AudioSymmetricRtpSession::startRtpThread (AudioCodec* audiocodec)
{
    if (_isStarted)
        return 0;

    _debug ("AudioSymmetricRtpSession: Starting main thread");

    _isStarted = true;
    setSessionTimeouts();
    setSessionMedia (audiocodec);
    initBuffers();
    initNoiseSuppress();

    startRunning();
    _rtpThread->start();

    return 0;
}

void AudioSymmetricRtpSession::stopRtpThread ()
{
    _debug ("AudioSymmetricRtpSession: Stoping main thread");

    _rtpThread->stopRtpThread();

    disableStack();
}

AudioSymmetricRtpSession::AudioRtpThread::AudioRtpThread (AudioSymmetricRtpSession *session) : rtpSession (session), running (true)
{
    _debug ("AudioSymmetricRtpSession: Create new rtp thread");
}

AudioSymmetricRtpSession::AudioRtpThread::~AudioRtpThread()
{
    _debug ("AudioSymmetricRtpSession: Delete rtp thread");
}

void AudioSymmetricRtpSession::AudioRtpThread::run()
{
    int threadSleep = 20;

    TimerPort::setTimer (threadSleep);

    _debug ("AudioRtpThread: Entering Audio rtp thread main loop");

    while (running) {

        // Send session
        if (rtpSession->getEventQueueSize() > 0) {
            rtpSession->sendDtmfEvent (rtpSession->getEventQueue()->front());
        } else {
            rtpSession->sendMicData ();
        }

        Thread::sleep (TimerPort::getTimer());

        TimerPort::incTimer (threadSleep);
    }

    _debug ("AudioRtpThread: Leaving audio rtp thread loop");
}

}