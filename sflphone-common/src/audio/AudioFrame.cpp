/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
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

#include <string.h> // for memset/memcpy

#include "AudioFrame.h"

AudioFrame::AudioFrame (const unsigned int size, const unsigned int nbChannel, const unsigned int smplRate) :
						_size(size),
						_nbChannel(nbChannel),
						_smplRate(smplRate),
						_internalBuffer(NULL),
						_bufferSize(0)
{
	if(_size == 0 || _nbChannel == 0) {
		_warn("AudioFrame: Cannot create audio frame of size 0");
		_size=160;         // Default is 160 samples, 20 ms at 8000 Hz
		_nbChannel = 1;
	}

	// compute length of internal buffer in samples
	_bufferSize = _size*_nbChannel;

	_internalBuffer = new SFLDataFormat[_bufferSize];

	// size in byte
	memset(_internalBuffer, 0, _bufferSize*sizeof(SFLDataFormat));
}

AudioFrame::AudioFrame (const unsigned int size, const unsigned int nbChannel, const unsigned int smplRate, SFLDataFormat *buffer)
{
	if(_size == 0 || _nbChannel == 0) {
		_warn("AudioFrame: Cannot create audio frame of size 0");
		_size=160;
		_nbChannel = 1;
	}

	// compute length of internal buffer in sample
	_bufferSize = _size*_nbChannel;

	_internalBuffer = new SFLDataFormat[_bufferSize];

	// size in byte
	memcpy(_internalBuffer, buffer, bufferSize*sizeof(SFLDataFormat));
}

AudioFrame::~AudioFrame()
{
	if(_internalBuffer)
		delete [] _internalBuffer;

	_internalBuffer = NULL;
}

boolean AudioFrame::getData(SFLDataFormat *buffer, const unsigned int channel)
{
	if(channel <= 0)
		channel = 1;

	if(channel > _nbChannel)
		channel = _nbChannel;

	memcpy(buffer, _internalBuffer[channel], _size*sizeof(SFLDataFormat));
}

boolean AudioFrame::putData(SFLDataFormat *buffer, const unsigned int channel)
{
	if(channel <= 0)
		channel = 1;

	if(channel > _nbChannel)
		channel = _nbChannel;

	memcpy(_internalBuffer[channel-1], buffer, _size*sizeof(SFLDataFormat));
}
