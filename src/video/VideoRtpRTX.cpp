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


VideoRtpRTX::VideoRtpRTX(){}
VideoRtpRTX::~VideoRtpRTX(){}


void VideoRtpRTX::run(){}
	
void VideoRtpRTX::initBuffers(){}
	
void VideoRtpRTX::initVideoRtpSession(){}
	 	
void VideoRtpRTX::sendSession(int timestamp){}

		
void VideoRtpRTX::receiveSession(){}

void VideoRtpRTX::loadCodec(enum CodecID id,int type){}

void VideoRtpRTX::unloadCodec(enum CodecID id,int type){}
