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
 
#include "VideoCodecDescriptor.h"

#include <string>
#include <stdio.h>

using namespace std;

VideoCodecDescriptor* VideoCodecDescriptor::instance= 0;

VideoCodecDescriptor* VideoCodecDescriptor::getInstance()
{
	//if no instance made create one,
	//ref. singleton pattern
	if (instance == 0)
	instance = new VideoCodecDescriptor;

	return instance;
}

 	VideoCodecDescriptor::~VideoCodecDescriptor()
 	{
 	vCodecOrder.clear();
 	vCodecMap.clear();

 	}
	
    VideoCodecDescriptor::VideoCodecDescriptor()
    {
    	av_register_all();
    	avcodec_init();
    	init();
    }

    void VideoCodecDescriptor::init()
    {
    VCMIterator mapIter;
	    //Create map list
	    if (initCodecMap() == false)
	    {
	    	ptracesfl("videoCodecInit error",MT_FATAL,2,true);
	    }
	    if (initBitRates() == false)
	    {
	    	ptracesfl("BitRates error",MT_FATAL,2,true);
	    }
	    if (initContext() == false)
	    {
	    ptracesfl("CodecContext error",MT_FATAL,2,true);
	    }
    }
    
     bool VideoCodecDescriptor::initCodecMap()
    {
    	
    	//TODO IMPLEMENT FULL CODEC LIST OF IMPLEMENTATION
		
		char *codec;
		AVCodec* tmp;
//(avcodec_find_decoder_by_name("h264") != NULL) &&
    		if( ((tmp = avcodec_find_decoder_by_name("h264")) != NULL) )
    			{
    			//map Codectmp
    			ptracesfl(tmp->name,MT_INFO,2,false);
    			ptracesfl(" Found",MT_NONE,2,true);
    			vCodecMap[tmp] = avcodec_alloc_context();	
    			}
    			else return false;
    
    		if((avcodec_find_decoder_by_name("h263") != NULL) &&
    		 ((tmp = avcodec_find_encoder_by_name("h263")) != NULL))
    			{
    			//map Codec
    			ptracesfl(tmp->name,MT_INFO,2,false);
    			ptracesfl(" Found",MT_NONE,2,true);
    			vCodecMap[tmp] = avcodec_alloc_context();	
    			}
    			else return false;

    return true;
    }
    
    
    bool VideoCodecDescriptor::setDefaultOrder(){
    
    VCMIterator mapIter;
    VCOIterator iter;
    //Set the default order of the codec list
    //means setting the exact same codecs as codecMap
    vCodecOrder.clear();
  
    for (mapIter = vCodecMap.begin();mapIter != vCodecMap.end();mapIter++)
    	vCodecOrder.push_back((*mapIter).first);
    
    	
    return true;
    }
    
   
    bool VideoCodecDescriptor::isActive(enum CodecID id){
   
    VCOIterator iter;
    
    for (iter = vCodecOrder.begin();iter != vCodecOrder.end();iter++)
    	if ((*iter)->id == id)
    	return true;
    
    return false;
    }

    
    char* VideoCodecDescriptor::serialize()
    {
    //return
    //TODO COULD BE USEFUL 
    return "doh!";
    }
    
	AVCodecContext* VideoCodecDescriptor::getCodecContext(AVCodec* Codec)
	{
		VCMIterator tmp;
		tmp = vCodecMap.find(Codec);
		return (*tmp).second;
	}
	
    VideoCodecOrder VideoCodecDescriptor::getActiveCodecs() { return vCodecOrder; }
	
    void VideoCodecDescriptor::setActiveCodecs(VideoCodecOrder vCodecOrder)
    {
    	this->vCodecOrder = vCodecOrder;
    }
	
    void VideoCodecDescriptor::setCodecMap(VideoCodecMap codec){this->vCodecMap = codec;}

	AVCodec* VideoCodecDescriptor::getCodec(const char* CodecName)
	{
	VCOIterator iter;
    	
    	for ( iter = this->vCodecOrder.begin(); iter != this->vCodecOrder.end();iter++)
    		if ((*iter)->name = CodecName)
    			return (*iter);
    			
    			
    return NULL;
	
	}

AVCodec* VideoCodecDescriptor::getDefaultCodec()
	{
	VCOIterator iter;
    	
    	if(vCodecOrder.empty())
    		return NULL;
    	else
    	return *(this->vCodecOrder.begin());

	}
    /********************************************
     * Functions for MEMMANAGER
     ********************************************
    */
     
     
    StringVector VideoCodecDescriptor::getStringActiveCodecs()
    {
    	StringVector tmp;
    	VCOIterator iter;
    	
    	for ( iter = this->vCodecOrder.begin(); iter != this->vCodecOrder.end();iter++)
    		tmp.push_back((string)(*iter)->name);

  		return tmp;
    }
    
    bool VideoCodecDescriptor::saveActiveCodecs(StringVector sActiveCodecs)
    {
    	
    	StringVectorIterator iter;
    	AVCodec *tmp;
    	bool saveOk =true;
    	vCodecOrder.clear();
    	
    	for ( iter = sActiveCodecs.begin(); iter != sActiveCodecs.end();iter++)
    		{
    			tmp = avcodec_find_decoder_by_name((*iter).c_str());
    			if(tmp != NULL)
    				vCodecOrder.push_back(tmp);
    			else
    			{
    			ptracesfl("Codec Not Found",MT_ERROR,2,true);
    			saveOk =false;
    			}
    		}
    		return saveOk;
    }
    
    StringVector VideoCodecDescriptor::getStringCodecMap()
    {
    StringVector tmp;
    VCMIterator iter;
    
    for ( iter = this->vCodecMap.begin(); iter != this->vCodecMap.end();iter++)
    		tmp.push_back((string)(*iter).first->name);
  		return tmp;
    }
    
    bool VideoCodecDescriptor::saveCodecMap(StringVector sCodecMap)
    {
    	StringVectorIterator iter;
    	AVCodec *tmp;
    	bool saveOk =true;
    	
    	vCodecMap.clear();
    
    for ( iter = sCodecMap.begin(); iter != sCodecMap.end();iter++)
    	{
	    	tmp = avcodec_find_decoder_by_name((*iter).c_str());
	    	
	    	if(tmp != NULL)
	    			vCodecMap[tmp] = avcodec_alloc_context();
	    		else
	    		{
	    			ptracesfl("Codec Not Found",MT_ERROR,2,true);
	    			saveOk =false;	
	    		}
    	}
  
    }
    
    bool VideoCodecDescriptor::initBitRates()
    {
    	
    	//TODO IMPLEMENT FULL BITRATES CONFIGURATION
    BitRateFormats.clear();
    
    BitRateFormats.push_back(32000);
    BitRateFormats.push_back(64000);
    BitRateFormats.push_back(128000);
    BitRateFormats.push_back(256000);
    BitRateFormats.push_back(384000);
    BitRateFormats.push_back(512000);
    BitRateFormats.push_back(768000);
    BitRateFormats.push_back(1024000);
    
    return true;
    
    }
    
    string VideoCodecDescriptor::getBitRates()
    {
    
    std::stringstream out;
    IntIterator iter;
    
    for(iter = BitRateFormats.begin(); iter != BitRateFormats.end() ; iter++)
    	out << (*iter)/1000 << ";";

	return out.str();

    }
    
    string VideoCodecDescriptor::getCurrentBitRate()
    {
    std::stringstream out;
    out << CurrentBitRate/1000;
    return out.str();
    }
    
    bool VideoCodecDescriptor::setCurrentBitRate(string bitRate)
    {
    	CurrentBitRate = atoi(bitRate.c_str())*1000;
    	ptracesfl("Current Bit Rate changed to ",MT_INFO,false);
    	ptracesfl(bitRate.c_str(),MT_NONE,true);	
    return true;
    }

    
    bool VideoCodecDescriptor::setDefaultBitRate()
    {
    	CurrentBitRate = DEFAULTBITRATE;
    	return true;
    }
    
    bool VideoCodecDescriptor::initContext()
    {
    VCMIterator mapIter;
  
    for (mapIter = vCodecMap.begin();mapIter != vCodecMap.end();mapIter++)
    	{


	(*mapIter).second->bit_rate = VIDEO_BIT_RATE;
	if ((*mapIter).first->name == "h264")
	(*mapIter).second->me_method = 8;
	else
	(*mapIter).second->me_method = 7;
	
	(*mapIter).second->time_base.den = STREAM_FRAME_RATE;
	(*mapIter).second->time_base.num = 1;
	(*mapIter).second->gop_size = GOP_SIZE;
	(*mapIter).second->pix_fmt = PIX_FMT_YUV420P;
	(*mapIter).second->max_b_frames = MAX_B_FRAMES;
	(*mapIter).second->mpeg_quant = 0;
	if ((*mapIter).first->name == "h264")
	(*mapIter).second->idct_algo = FF_IDCT_H264;
	else
	(*mapIter).second->idct_algo = FF_IDCT_AUTO;
	
	(*mapIter).second->mb_decision = FF_MB_DECISION_BITS;
	//TODO ADD VLC MATRIX
	(*mapIter).second->intra_matrix = NULL;
	(*mapIter).second->inter_matrix = NULL;
	(*mapIter).second->workaround_bugs = FF_BUG_AUTODETECT;
	
//	#define X264_PART_I4X4 0x001  /* Analyse i4x4 */
//#define X264_PART_I8X8 0x002  /* Analyse i8x8 (requires 8x8 transform) */
//#define X264_PART_P8X8 0x010  /* Analyse p16x8, p8x16 and p8x8 */
//#define X264_PART_P4X4 0x020  /* Analyse p8x4, p4x8, p4x    4 */
//#define X264_PART_B8X8 0x100  /* Analyse b16x8, b8x16 and b8x8 */
//	_encodeCodecCtx->partitions = 

    	}

return true;
    }

