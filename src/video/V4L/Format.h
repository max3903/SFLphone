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

#ifndef FORMAT_H
#define FORMAT_H

#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

#include "ImageSize.h"
	
#include "../../tracePrintSFL.h"

#define FORMAT_TRACE	3

using namespace std;

#define CLIP 320
#define RED_NULL 128
#define BLUE_NULL 128
#define LUN_MUL 256
#define RED_MUL 512
#define BLUE_MUL 512


#define GREEN1_MUL  (-RED_MUL/2)
#define GREEN2_MUL  (-BLUE_MUL/6)
#define RED_ADD     (-RED_NULL  * RED_MUL)
#define BLUE_ADD    (-BLUE_NULL * BLUE_MUL)
#define GREEN1_ADD  (-RED_ADD/2)
#define GREEN2_ADD  (-BLUE_ADD/6)

/* lookup tables */
static unsigned int  ng_yuv_gray[256];
static unsigned int  ng_yuv_red[256];
static unsigned int  ng_yuv_blue[256];
static unsigned int  ng_yuv_g1[256];
static unsigned int  ng_yuv_g2[256];
static unsigned int  ng_clip[256 + 2 * CLIP];

#define GRAY(val)               ng_yuv_gray[val]
#define RED(gray,red)           ng_clip[ CLIP + gray + ng_yuv_red[red] ]
#define GREEN(gray,red,blue)    ng_clip[ CLIP + gray + ng_yuv_g1[red] + \
	ng_yuv_g2[blue] ]
#define BLUE(gray,blue)         ng_clip[ CLIP + gray + ng_yuv_blue[blue] ]

#define clip(x) ( (x)<0 ? 0 : ( (x)>255 ? 255 : (x) ) )
            
class ImageSize;

/** Abstract class that represent a raw video format.
 * 
 * It contains all utilities fonctions needed to convert the image to RBG
 */
class Format {
public:

	/** Default Constructor
	 */
    Format();

	/** Destructor
	 */
    ~Format();

	/* Method to initilize the format
	 * 
	 * Must be implemented by child class.
	 */
    virtual void init() =0;

	/** Access method to the type of the Format
	 * 
	 * Must be implemented by child class.
	 * @return An int represention the V4L Format Code
	 */
    virtual int getType() =0;
    
    /** Access method to the type of the Format
	 * 
	 * Must be implemented by child class.
	 * @return A char pointer to the format's Name
	 */
    virtual char* getStringType()=0;

	/** Method to convert the raw data to RGB format 
	 * 
	 * Must be implemented by child class.
	 * @param input An input buffer (raw data)
	 * @param ouput An output buffer (rbg data)
	 * @param w The width of the image
	 * @param h The height of the image
	 */
    virtual void convert(unsigned char* input, unsigned char* ouput, int w, int h) =0;

	/** Validation Method for the fps rate
	 * 
	 * Search the supported fps rate for the current image size to see if the specified format is present.
	 * @param w The width of the image
	 * @param h	The height of the image
	 * @param fps the Frame Rate per second
	 * @return If the fps rate is supported by the Format.
	 */
    bool isFPSSupported( int w, int h, int fps);

	/** Validation Method for the resolution
	 * 
	 * Search the supported image sizes to see if the specified format is present.
	 * @param w The width of the image
	 * @param h	The height of the image
	 * @return If the resolution is supported by the Format.
	 */
    bool isRessolutionSupported(int w, int h);
    
    /** Access method to get the current image size
     * @return A pointer to the current image size 
     */
    ImageSize* getCurrentImageSize();
    
    /** Method to change the current image size
     * 
     * It searches the supported image size and sets the current image size to the specified value if present.
     * @param valueX The x resolution
     * @param valueY The y resolution
     * @return The success of the operation
     */
    bool setCurrentImageSize(  __u16 valueX, __u16 valueY );
    
    /** Access method to the supported Images sizes
     * @return a vector of the supported Image Size
     */
    const char* getAllSizes();
    
    /** Enum containing the various format supported
     */
    enum TFormat{ F_YUV420= 0x01,	/**YUV 420 format*/
             	  F_YUV422= 0x02	/** YUV 422 format*/
           		 };

protected:
	
	/** Method to enumerate the supported image size for the format
	 * @param fd The file descriptor to the device
	 */
	 void enumerateFrameSizes(int fd);

	/** Utility Method to write an RBG pixel
	 */
    void write_rgb(unsigned char **out, int Y, int U, int V);
    
    /** Initialisation method for the conversion
     */
    void YUV2RGB_init(void);
    
    /** Vector containing the sizes of the images
     */
    vector<ImageSize*> sizes;
    
    /** An iterator to the current size
     */
    vector<ImageSize*>::iterator currentSize;
    
};
#endif //FORMAT_H
