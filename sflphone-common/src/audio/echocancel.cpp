/*
 *  Copyright (C) 2008 2009 Savoir-Faire Linux inc.
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

#include "echocancel.h"


#define FRAME_SIZE 160
#define FILTER_LENGTH 2000

EchoCancel::EchoCancel() 
{
  _echoState = speex_echo_state_init(FRAME_SIZE, FILTER_LENGTH);
}

EchoCancel::~EchoCancel() 
{
  speex_echo_state_destroy(_echoState);
  _echoState = NULL;
}

void EchoCancel::putData(SFLDataFormat *inputData) 
{
  speex_echo_playback(_echoState, inputData);
}

void EchoCancel::process(SFLDataFormat *inputData, SFLDataFormat *outputData)
{
  speex_echo_capture(_echoState, inputData, outputData);
}

void EchoCancel::process(SFLDataFormat *micData, SFLDataFormat *spkrData, SFLDataFormat *outputData){

  speex_echo_cancellation(_echoState, micData, spkrData, outputData);

}