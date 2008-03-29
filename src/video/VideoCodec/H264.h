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
 *  H264 Class
 * 
 * This class is a VideoCodec child class. Since the VideoCodec class is virtual and abstract
 * an H264 videoCodec will assumme encoding and decoding video data.
 * This class acts like an interface with the encoding and decoding methods of the h264
 *  libavcodec files
 * 
 */

#ifndef H264_H
#define H264_H
#include "VideoCodec.h"


/**
 * @author Jean-Francois Blanchard-Dionne 
 */
class H264 : public VideoCodec {
public:
	/**
     * Default Constructor
     * 
     */

    H264();
	/**
     * Default Destructor
     * 
     */
    ~H264();
	/**
     * Function to decode video information
     * @param in_buf the input buffer
     * @param width of the video frame
     * @param height of the video frame
     * @param out_buf the output buffer
     * 
     */
    int videoDecode(uint8_t *in_buf, int width, int height, uint8_t* out_buf  );
	/**
     * Function to encode video information
     * @param width of the video frame
     * @param height of the video frame
     * @param buf the buffer to encode
     * @param size buffer size
     * 
     */
    int videoEncode(int width, int height, uint8_t* buf, unsigned int size);
};
#endif //H264_H
