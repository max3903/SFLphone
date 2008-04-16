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

#ifndef CONFIGSET_H
#define CONFIGSET_H

#include <vector>
#include <map>

#include "../../tracePrintSFL.h"

#include "YUV420.h"
#include "YUV422.h"

#include "Control.h"

using namespace std;

#define CONFIGSET_TRACE	1

/** Class that collects and manages configuration data for a VideoDevice 
 * 
 * On construction it enumerates all the configuration data and sets default values as the first value that is available.
 * */ 
class ConfigSet {
public:

	/** Constructor
	 * @param fd the file descriptor to the VideoDevice
	 */
    ConfigSet(int fd);

	/** Destructor
	 */
    ~ConfigSet();
    
    /** Returns the current image format used by the camera
     * @return the format currently used
     */
    Format* getCurrentFormat();

	/** Sets the current format as the next supported format
	 */
    void nextFormat();

	/** Access method to the supported formats
	 * @return a vector containing all the supported formats
	 */
    vector<Format*> getFormats();
    
    /** Method to get a supported control for the VideoDevice
     * 
     * The supported control type are listed in the enum TControl in the Control class.
     * @param type The type of control (see TControl);
     * @return Returns the control, if supported by the device or return NULL if control not supported.
     */
    Control* getControl(int type);

private:

	/** Default contructor
	 * You cannot create a config set without specifing a VideoDevice
	 */
	ConfigSet();

	/** Method to enumerate the supported image format
	 * @param fd the file descriptor for the VideoDevice
	 */
    void init_enumerateImagesFormats(int fd);
    
    /** Method to enumerate the supported controls
     * @param fd the file descriptor for the VideoDevice
     */
    void init_Controls(int fd);
    
    /** Method to create a supported Image format
     * @param pFormat a char poiter to the FourCC code
     * @param Description a char pointer to the description of the image format.
     * @param theFormat the format of the image as described in the pixelformat field of the v4l2_fmtdesc structure.
     * @param fd the fileDescriptor
     * @return the supported format, if the format is not supported the return value is NULL
     */
    Format* getFormat(char * pFormat, char * Description, int fd, __u32 theFormat);
    
    /** The map of the supported controls
     */
    map<string,Control*> controlMap;

	/** The supported formats
	 */
    vector<Format*> formats;
    
    /** The current format used by the camera
     */
    vector<Format*>::iterator current;
    
};
#endif //CONFIGSET_H
