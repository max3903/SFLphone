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
#include <cstdio>
#include <cstdlib>
#include "VideoRtp.h"
#include "VideoRtpRTX.h"


VideoRtp::VideoRtp()
{
  vRTXThread = 0;
  vRTXThreadConf = 0;
}
VideoRtp::~VideoRtp()
{
  delete vRTXThread;
  delete vRTXThreadConf;
  vRTXThread = 0;
  vRTXThreadConf = 0;
}

int VideoRtp::createNewVideoSession(SIPCall *call, bool Conf)
{
  ost::MutexLock m(vThreadMutex);

  // TODO: something should stop the thread before...
  if ( vRTXThread != 0 ) { 
    //_debug("! ARTP Failure: Thread already exists..., stopping it\n");
    delete vRTXThread; vRTXThread = 0;
    //return -1; 
  }

 /*
 // TODO:  something should stop the thread before...
  if ( vRTXThreadConf != 0 ) { 
    _debug("! ARTP Failure: ThreadConf already exists..., stopping it\n");
    delete vRTXThreadConf; vRTXThreadConf = 0;
    //return -1; 
  }
  */

  if (!Conf){
    // Start RTP Send/Receive threads, CONVERSATION
    // TODO: Lire sur symmetric RTP video session!
    //_symmetric = Manager::instance().getConfigInt(SIGNALISATION,SYMMETRIC) ? true : false; 
    //vRTXThread = new VideoRtpRTX (call, _symmetric);
    vRTXThread = new VideoRtpRTX (call, false);
    try {
      //TODO: ACTIVER CELA POUR QUE LE THREAD RUN (probleme de semaphore)
      if (vRTXThread->start() != 0) {
        //_debug("! ARTP Failure: unable to start RTX Thread\n");
        return -1;
      }
    } catch(...) {
      //_debugException("! ARTP Failure: when trying to start a thread");
      throw;
    }
    return 0;
  }
  /*
  else{
    // Start RTP Send/Receive threads, CONFERENCE
    _symmetric = Manager::instance().getConfigInt(SIGNALISATION,SYMMETRIC) ? true : false; // TODO: Ca mange quoi en hiver ca ?
    vRTXThreadConf = new VideoRtpRTX (call, _symmetric);
    try {
      if (vRTXThreadConf->start() != 0) {
        _debug("! ARTP Failure: unable to start RTX Thread\n");
        return -1;
      }
    } catch(...) {
      _debugException("! ARTP Failure: when trying to start a thread");
      throw;
    }
    return 0;
  }
  */

  
}

void VideoRtp::closeVideoSession(bool Conf)
{
  ost::MutexLock m(vThreadMutex);
  // This will make RTP threads finish.
  // _debug("Stopping AudioRTP\n");
  try {
    delete vRTXThread; vRTXThread = 0;
  } catch(...) {
    //_debugException("! ARTP Exception: when stopping audiortp\n");
    throw;
  }
}

