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

#ifndef IMAGESIZE_H
#define IMAGESIZE_H

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev.h>
#include <vector>
#include<algorithm>

#include "Format.h"

#define IMAGESIZE_TRACE 	4

using namespace std;

/** Class that encapsulate the image size management
 */ 
class ImageSize {
public:

	/** Constructor
	 * 
	 * Initializes the object by finding the frame rates associated to the resolution
	 * @param w the width
	 * @param h the height
	 * @param fd the file descriptor to the device
	 * @param imgFormat the Format of the image
	 */
	ImageSize(int w, int h, int fd, int imgFormat);

	/** Destructor
	 */
    ~ImageSize();

	/**	Access method for the width
	 * @return the width of the image
	 */
    int getWidth();

	/**	Access method for the height
	 * @return the height of the image
	 */
    int getHeight();

	/**	Access method for the FPS vector
	 * @return the FPS vector for the image
	 */
    vector<int> getFps();
    
    /** Access method for the current fps rate
     * @return A char pointer to the current fps rate
     */
    const char* getFpsString();
    
    /** Access method for the current fps rate
     * @return The current fps rate
     */ 
    int getCurrentFps();
    
    /** Method to set the current fps rate
     * @param fps the desired fps rate
     * @return The success of the operation
     */ 
    bool setCurrent(int fps);
    
    /** Validation method for the fps rate
     * @param fps rate to validate
     * @return if the fps rate is supported
     */ 
    bool isFPSSupported(int fps);    

private:

	/** Default constructor
	 * 
	 * You cannot create an image size without the proper information
	 */
    ImageSize();
    
    /** Method to enumerate the possible frame rates of the camera
     * @param fd The file descriptor to the device
     * @param imgFormat The image format
     */
    void init_enumerateFPS(int fd, int imgFormat);
    
    /** The width of the image
     */
    int width;
    
    /** The height of the image
     */
    int height;
    
    /** the supported fps
     */
    vector<int> fps;
    
    /** The current fps
     */
    vector<int>::iterator currentFps;
    
};
#endif //IMAGESIZE_H
