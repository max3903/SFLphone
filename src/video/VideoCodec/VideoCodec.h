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
 
/*!
 *  VideoCodec Class
 * 
 * This is the VideoCodec class. It's a virtual abstract class for encoding and 
 * decoding video data.
 * 
 *	//TODO 	-this class is a libavcodec interface. Any codec can be used at this time
 * 			but the settings at initiation are made for h263 and h264 codecs mainly.
 * 
 * 			-Would be interesting to set files for loading any codec and save/load those
 * 			settings. Would also be interesting when it will be available in the libavcodec library
 * 			to get set default contexts in the init parts. (as of april 2008 - libavcodec does not support it)
 * 
 * 			-The encode and decode are made to be dynamic if the local and foreign user change its
 * 			resolution and if we enter in a video-conference
 * 
 * 
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
    VideoCodec(char* codecName);
    VideoCodec(enum CodecID id);
	/**
	
     * Function to decode video information
     * @param in_buf the input buffer
     * @param out_buf the output buffer
     * @param inSize input's buffer size
     * @param width - width of the input buffer
     * @param height - height of the input buffer
     * 
     */

     int videoDecode(uint8_t *in_buf, uint8_t* out_buf,int inSize,int width,int height);

/**
     * Function to encode video information - The user has to set the input and output buffers
     * 
     * @param[in] in_buf 	the input buffer containing the data to encode
     * @param[out] out_buf	The encoded data
     * @param[in] width	the in_buf  width
     * @param[in] height	the in_buf height
     * @return the size of the encoded buffer, a negative value otherwise
     */

    int videoEncode(unsigned char* in_buf, unsigned char* out_buf,int width,int height);
 	
	/***
	 * Functions to get the encoding input or output resolution
	 */
	 pair<int,int> getEncodeIntputResolution();
	 
	 pair<int,int> getEncodeOutputResolution();
	 
	 pair<int,int> getDecodeIntputResolution();
	 
	 pair<int,int> getDecodeOutputResolution();
    
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
     * Active Resolution
     */
    Resolution* _cmdRes;
    // Video device manager instance
	VideoDeviceManager *_v4lManager; 
	
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
    * width and height the codec will receive as input
    */
    int inputWidth;
    int inputHeight;
    /** 
    * width and height the codec will send
    */
    int outputWidth;
    int outputHeight;    
    /**
     * set to true if needed
     * the padding will auto adjust while encoding.
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

