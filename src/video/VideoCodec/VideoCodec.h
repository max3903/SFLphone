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
/**
 *  VideoCodec Class
 * 
 * This is the mother VideoCodec class. It's a virtual abstract class for encoding and 
 * decoding video data.
 */

#ifndef VIDEOCODEC_H
#define VIDEOCODEC_H
extern "C"{
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
//#include <ffmpeg/swscale.h>
}
#include "../VideoCodecDescriptor.h"

class VideoCodec {
public:
/**
     * Default Constructor
     * 
     */
    VideoCodec();
	/**
     * Default Destructor
     * 
     */
   ~VideoCodec() ;
  
   /**
     *  Constructor we force to use
     * 
     */
    VideoCodec(char* codecName);
/**
	
     * Function to decode video information
     * @param in_buf the input buffer
     * @param out_buf the output buffer
     * 
     */

     int videoDecode(uint8_t *in_buf,uint8_t* out_buf);

/**
     * Function to encode video information
     * @param buf the buffer to encode
     * @param in_buf the input buffer
     * @param out_buf the output buffer
     * 
     */

    int videoEncode(uint8_t *in_buf, uint8_t* out_buf,int bufferSize,int width,int height);

    
    
private:
 
	
	/**
 	* Function to init the Codec with it's proper context
 	* */
    void init();
    
    AVPicture* encodeConvert(uint8_t *in_buf,int inWidth,int inHeight, int outWidth, int outHeight);
    
    /**
 	* instance of the videoDesc
 	* */
    VideoCodecDescriptor* _videoDesc;

	/**
     * Libavcodec Codec type
     */
    AVCodec* _Codec;
    
    /**
     * Libavcodec Codec type
     */
    char* _codecName;
    
    /**
     * Libavcodec Codec context
     */
    AVCodecContext* _baseCodecCtx;
    

};
#endif //VIDEOCODEC_H

