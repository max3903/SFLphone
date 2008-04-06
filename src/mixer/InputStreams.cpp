/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean Tessier <jean.tessier@polymtl.ca> 
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

#include "InputStreams.h"

InputStreams::InputStreams()
{
  audioInput = NULL;
  videoInput = NULL;
}

InputStreams::InputStreams(VideoInput* videoData, AudioInput* audioData)
{
	// We want this to be the pointer to the buffer because this object is just a container
  audioInput = audioData;
  videoInput = videoData;
}

InputStreams::~InputStreams()
{
}

VideoInput* InputStreams::fetchVideoStream() const
{
  return videoInput;
}

AudioInput* InputStreams::fetchAudioStream() const
{
  return audioInput;
}
