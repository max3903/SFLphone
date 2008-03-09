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
 *  VideoRTP Class
 * 
 * This class is responsible of creating the thread for getting and setting
 * video data in a conversation and add an other thread in case Local SFLPhone
 * is a Conference server
 */

#ifndef VIDEORTP_H
#define VIDEORTP_H

#include "VideoRtpRTX.h"
#include <cc++/thread.h>
#include <samplerate.h>
class VideoRtpRTX;
/**
 * @author Jean-Francois Blanchard-Dionne 
 */
class VideoRtp {
public:
	/**
	 * Destructor
	 */ 	
    ~VideoRtp();
	/**
	 * Default Constructor
	 */ 	
    VideoRtp();

	/**
	 * Function to create a new Vrtxthread
	 * @param conf is 0 to create a a conference video session thread
	 * note : must have an initial normal thread going on
	 * @return 0 if success , -1 if failure
	 */ 
    int createNewVideoSession(bool conf);
	/**
	 * Function to close a Vrtxthread
	 * @param conf is 0 to create a a conference video session thread
	 * note : must have an initial normal thread going on
	 *@return 0 if success , -1 if failure
	 */ 
    int closeVideoSession(bool conf);

private:
    VideoRtpRTX* vRTXThread;
    VideoRtpRTX* vRTXThreadConf;
    ost::Mutex vThreadMutex;
    
};

#endif //VIDEORTP_H
