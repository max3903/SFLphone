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
#include "VideoCodec.h"


int VideoCodec::videoEncode(int width, int height, uint8_t* buf, unsigned int size){


return 0;
	}
	
int VideoCodec::videoDecode(uint8_t *in_buf, int width, int height, uint8_t* out_buf  ){

return 0;
}

void VideoCodec::init(){

//Get codec to encode decode

//Getting Basic AVCodecContext settings from Codec Descriptor
videoDesc->getInstance();
codecCtx = videoDesc->getCodecContext(Codec);

//get webcam information to encode: buffer adress, size etc...

//get 

}