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
 * 
 *	//TODO -this class is an libavcodec interface. Any codec can be used at this time
 * 			but the settings at initiation are made for h263 and h264 codecs mainly  
 */

#ifndef VIDEOCODEC_H
#define VIDEOCODEC_H

#include "../VideoCodecDescriptor.h"
#include "VideoSettings.h"
#include "../V4L/VideoDeviceManager.h"
#include "SWSInterface.h"

class VideoCodec {
public:
	

	/**
     * Default Destructor
     * 
     */
    ~VideoCodec();
  
   /**
     *  Constructor we force to use
     * 
     */
    VideoCodec(char* codecName,int inWidth,int inHeight);
    VideoCodec(enum CodecID id,int inWidth,int inHeight);
/**
	
     * Function to decode video information
     * @param in_buf the input buffer
     * @param out_buf the output buffer
     * 
     */

     int videoDecode(uint8_t *in_buf, uint8_t* out_buf,int inSize);

/**
     * Function to encode video information - The user has to set the input and output buffers
     * 
     * @param in_buf 	the input buffer containing the data to encode
     * @param out_buf	The encoded data
     * @param inWidth	the in_buf  width
     * @param inWidth	the in_buf height
     * @return the size of the encoded buffer, a negative value otherwise
     */

    int videoEncode(unsigned char* in_buf, unsigned char* out_buf);
 	

    
private:
 	/**
     * Default Constructor
     * 
     */
    VideoCodec();
	/**
 	* Function to init the Codec
 	* */
    void init();
        
    /**
 	* Function to init the Codec with it's proper context
 	* */
    void initEncodeContext();

    /**
 	* Function to init the Codec with it's proper context
 	* */
    void initDecodeContext();
    
    /**
 	* Function to quit the Codec with it's proper context
 	* */
    void quitEncodeContext();

    /**
 	* Function to quit the Codec with it's proper context
 	* */
    void quitDecodeContext();

	/**
 	* Instance of the VideoCodecDescriptor class
 	* */
	VideoCodecDescriptor *_videoDesc;
	
	/**
     * Libavcodec Encoding power duo
     */
    AVCodecContext* _encodeCodecCtx;
    AVCodec* _CodecENC;
    
	/**
     * Libavcodec Decoding power duo
     */
    AVCodecContext* _decodeCodecCtx;
	AVCodec* _CodecDEC;
   
   /** 
    * width and height the codec will receive to encode
    */
    int inputWidth;
    int inputHeight;
    
      
   /** 
    * width and height the codec will receive to decode
    */
    int outWidth;
    int outHeight;
    
    /**
     * set to true if needed
     * 
     */
    bool padding;
    int paddingbottom;
    int paddingTop;
	
	
	// SWSInterface for Format and width/height conversions
    SWSInterface *decodeSWS;
    SWSInterface *encodeSWS;
    


};
#endif //VIDEOCODEC_H

