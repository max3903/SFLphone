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
 *  VideoCodecDescriptor Class
 * 
 * This class  acts like a container. It generates at startup a map of all
 * codecs, active or not, supported by the application and sets a vector for the order
 * of negotiation of the video codecs
 * Most functions in this class is for managing and accessing these two containers
 * This class is mainly to interact with the GUI about video Codecs matter.
 */


#ifndef VIDEOCODECDESCRIPTOR_H
#define VIDEOCODECDESCRIPTOR_H

//TODO GET SET DEFAULT SETTINGS FOR EACH CODEC


#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "../tracePrintSFL.h"
extern "C"{
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
}

/* A codec is identified by it's AVCodec, the codec utilisation by the AVCodecContext */
typedef std::map<AVCodec*, AVCodecContext*> VideoCodecMap;
/* VideoCodecOrder iterator typedef*/
typedef VideoCodecMap::iterator VCMIterator;
/* The vector to reflect the order the user wants to use his VideoCodecs 
 * The codecs in this list are the ACTIVE CODECS */
typedef std::vector<AVCodec*> VideoCodecOrder;
/* VideoCodecOrder iterator typedef*/
typedef VideoCodecOrder::iterator VCOIterator;

typedef std::vector<std::string> StringVector;
/* VideoCodecOrder iterator typedef*/
typedef StringVector::iterator StringVectorIterator;


class VideoCodecDescriptor {
public:


//! Method to get the instance of the VideoCodecDescriptor.
    /*!
     * If it does not exist, this method will create it
     * 
     * \return an instance of the manager
     */
    VideoCodecDescriptor* getInstance();

	/**
	 * Destructor 
     */
    ~VideoCodecDescriptor();
	
    /**
     * Set the default codecs order: All map codecs are transfered to vCodecOrder
     */   
    bool setDefaultOrder();
  
    /**
     * Check in the map codec if the specified codec is supported 
     * @param id : libavcodec unique codecID
     * @return true if the codec specified is supported
     * 	     false otherwise
     */
    bool isActive(enum CodecID id);
    

	/**
     * Function to send the vector containing the active Codecs.
     * 
     */
    VideoCodecOrder getActiveCodecs();
    
	/**
     * Function to set the map
     * @param activeC to set the Codec Map with another map
     * (not really suppose to happen)
     */
    void setActiveCodecs(VideoCodecOrder vCodecOrder);
	/**
     * Function to set the map with all the codecs used by sflphone
     * @param codecMap to set the Codec Map
     * (not really suppose to happen)
     */
    void setCodecMap(VideoCodecMap codec);
	/**
     * Function to get the map containing all the videoCodecsUsed
     * @return codecMap to set the Codec Map
     */
   	VideoCodecMap getCodecMap();
   	
   	/**
     * Function to get the context of a Codec
     * @return codecMap to set the Codec Map
     */
   	AVCodecContext* getCodecContext(AVCodec* Codec);
   	
	/**
     * Function to get all the codec info
     * @return char*, with all the info in a structured way
     */
	char * serialize();
	
	 /********************************************
     * Functions for MEMMANAGER
     *********************************************
     */
	
	 /**
     * Function to send the vector containing the active Codecs IN A STRING VECTOR
     * @return a string vector with all the codec Names in it
     */
    StringVector getStringActiveCodecs();
    
    /**
     * Function to save the the vector containing the active Codecs
     * @param activeCodecs a string list of vectors
     */
    bool saveActiveCodecs(StringVector sActiveCodecs);
    
    /**
     * Function to get the map
     * @return StringVector a string vector copntaining all the vector
     */
   	StringVector getStringCodecMap();
   	
   	/**
     * Function to save the a map containing the active Codecs
     * @param sCodecMap a string list of vectors
     */
    bool saveCodecMap(StringVector sCodecMap);
   	
    

private:	

	/** The instance of the VideoCodecDescriptor */
    static VideoCodecDescriptor* instance;
	/**
     * Function called by constructor, will create lists and register active codecs
     */   
    void init();
    /**
     *  Create Map of Codecs
     */
     bool initCodecMap();
     
     
     /**
     *  return codec Name
     */
     bool checkSupported();
     
    
	/**
     * Vector of all the Active codecs
     */
    VideoCodecOrder vCodecOrder;
    /**
     * Map of all codecs, active and inactive
     */
    VideoCodecMap vCodecMap;

protected:
/**
	 * Default Constructor
     */
    VideoCodecDescriptor();
    
    

};
#endif //VIDEOCODECDESCRIPTOR_H
