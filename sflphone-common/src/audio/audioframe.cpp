/*
 *  Copyright (C) 2010 Savoir-Faire Linux inc.
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

#include "audioframe.h"

AudioFrame::AudioFrame (const unsigned int size, const unsigned int nbChannel, const unsigned int smplRatei) {}

AudioFrame::AudioFrame (const unsigned int size, const unsigned int nbChannel, const unsigned int smplRate, SFLDataFormat *buffer) {}

AudioFrame::~AudioFrame() {}

boolean AudioFrame::getData(SFLDataFormat *buffer, unsigned int channel) {}

boolean AudioFrame::putData(SF:DataFormat *buffer, unsigned int channel) {}

#endif 
