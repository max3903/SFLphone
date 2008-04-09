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
 * 
 * This is an interface for the libswscale library
 * It's main function is to convert from a given 
 * pix format and resolution to another pix format and resolution
 * 
 * This is useful before encoding to change from
 * a RGB format to a YUV (codec ready) format or the opposite 
 * 
 * You only have to init an instance and call the convert function
 * moreover the set sizes will reset the context to dynamically change
 * the Interface context
 * 
 * 
 */

#ifndef SWSINTERFACE_H
#define SWSINTERFACE_H


extern "C"{
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#include <ffmpeg/swscale.h>
}
#include <stdlib.h>
#include <stdio.h>

 struct FrameProperties
    {
        int width;
        int height;
        int pixFormat;

    };
    
class SWSInterface {
public:
	/**
     * Default Destructor
     * 
     */
   ~SWSInterface();
  
   /**
     *  Constructor we force to use
     * @param[in] InWidth the input width
     * @param[in] InHeight the input height
     * @param[In] InPixFormat - see avcodec<s documentation for the typedefs 
     * 									representing each of the format
     * 
     */
    
    SWSInterface(int InWidth,int InHeight,int InPixFormat,int OutWidth,int OutHeight,int OutPixFormat);
/**


    /**
  *  Function to change RGB to YUV420
  * */
    bool Convert(AVFrame *In,AVFrame *Out);

    FrameProperties getInputProperties();

    void setInputProperties(int setWidth,int setHeight,int setPixFormat);
    void setInputProperties(int setWidth,int setHeight);

    FrameProperties getOutputProperties();

    void setOutputProperties(int setWidth,int setHeight,int setPixFormat);
    void setOutputProperties(int setWidth,int setHeight);
	AVFrame *alloc_pictureRGB24(int width, int height);
	AVFrame *alloc_picture420P(int width, int height);
	AVFrame *alloc_pictureRGB24(int width, int height,uint8_t *buffer);
	AVFrame *alloc_picture420P(int width, int height,uint8_t *buffer);
	static FrameProperties getSpecialResolution(int width);

private:

	SWSInterface();
	
	struct SwsContext *Context;
    /**
     * Libavcodec YUV buffer Size
     */
    int BufferSize;
    FrameProperties out;
    FrameProperties in;
     
};
#endif //SWSINTERFACE_H
