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

#ifndef YUV422_H
#define YUV422_H

#include "Format.h"

/** Class that acts like a format converter. In this case YUV420 to RBG
 * */
class YUV422 : public Format {
public:

	/** Constructor
	 * @param fd The file descriptor to the VideoDevice
	 */
    YUV422( int fd);

	/** Destructor
	 */
    ~YUV422();

	/** Access method to the type of the Format
	 * 
	 * @return An int represention the V4L Format Code
	 */
    virtual int getType();

	/** Access method to the type of the Format
	 * 
	 * @return A char pointer to the format's Name
	 */
	virtual char* getStringType();
	 
	/* Method to initilize the format
	 */
    virtual void init();

	/** Method to convert the raw data to RGB format 
	 * 
	 * @param input An input buffer (raw data)
	 * @param ouput An output buffer (rbg data)
	 * @param w The width of the image
	 * @param h The height of the image
	 */
    virtual void convert(unsigned char* input, unsigned char* ouput, int w, int h);
   
protected:

	/* Default Constructor
	 * You cannot create a format converter without the proper information
	 */
	YUV422();
    
};
#endif //YUV422_H
