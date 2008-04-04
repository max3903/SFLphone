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
 
 
#include "VideoCodec.h"

#define VIDEOCODECPTRACE 2


int VideoCodec::videoEncode(unsigned char*in_buf, unsigned char* out_buf,int inWidth,int inHeight)
{
	AVFrame *IN=NULL,*SWS=NULL,*OUT=NULL;
	int outsize;
	
	
	IN  =  encodeSWS->alloc_pictureRGB24(inWidth,inHeight,in_buf);
	SWS = encodeSWS->alloc_picture420P(_encodeCodecCtx->width,_encodeCodecCtx->height);
	

	if(IN != NULL || SWS != NULL)
	{
 	if (encodeSWS->Convert(IN,SWS) == false)
 		printf("ERROR SWS FUNCTION\n");
 	}
 	else printf("NULL\n");

	//Step 2:Encode
	//TODO GET A PROPER BUFFER SIZE
	printf("ENCODE\n");
	if ( (outsize = avcodec_encode_video(_encodeCodecCtx, out_buf, FF_MIN_BUFFER_SIZE, SWS))<= 0)
		printf("ERROR ENCODE\n");
		
	printf("PTS PTS : %d\n",_encodeCodecCtx->coded_frame->pts);
	
	if(outsize > 1400)
	printf("OUTSIZE : %d\n",outsize);
	
	
	if(outsize <= 0)
		ptracesfl("ERROR",MT_ERROR,1,true);
			
	
	av_free(SWS);
	av_free(IN);
	printf("FREE\n");

	//return outBufferSize
	return outsize;
	
	}

int VideoCodec::videoDecode(uint8_t *in_buf, uint8_t* out_buf,int size,int outWidth,int outHeight)
{
	int frame, got_picture, len;
	uint8_t *buf;
	AVFrame *SWS,*OUT;
	
	if((buf = (uint8_t*)av_malloc(size+FF_INPUT_BUFFER_PADDING_SIZE)) == NULL)
	printf("malloc error\n");
    if( memcpy(buf,in_buf,size) ==NULL)
	printf("memcpy error\n");
	if ( memset(buf + size, 0, FF_INPUT_BUFFER_PADDING_SIZE) == NULL)
	printf("memset error\n");
	
    SWS = decodeSWS->alloc_picture420P(_decodeCodecCtx->width,_decodeCodecCtx->height);
    printf("SWS width: %i, height: %i \n",_decodeCodecCtx->width,_decodeCodecCtx->height);
    printf("Video decoding 1\n");

      len = avcodec_decode_video(_decodeCodecCtx, SWS, &got_picture,buf,size );
   		printf("len: %i\n",len);
   	
   	OUT = decodeSWS->alloc_pictureRGB24(outWidth,outHeight,out_buf)	;
   	
   	decodeSWS->Convert(SWS,OUT);
   	
    av_free(SWS);
    av_free(buf);
    av_free(OUT);

	return 0;
}

void VideoCodec::init(){
	
	ptracesfl("VideoCodec initialisation",MT_INFO,VIDEOCODECPTRACE,true);
	
	//Get VideoDescriptor Instance
	_videoDesc = VideoCodecDescriptor::getInstance();
	
	//Get V4LManager instance
	_v4lManager = VideoDeviceManager::getInstance();
	
	_cmdRes = (Resolution*)_v4lManager->getCommand(VideoDeviceManager::RESOLUTION);

	//check if active Codec
	if(_codecName == NULL)
	{
	ptracesfl("CODEC ERROR",MT_FATAL,1,true);
	}
	else{
	_CodecENC = avcodec_find_encoder_by_name(_codecName);
	_CodecDEC = avcodec_find_decoder_by_name(_codecName);
	}
	
	//These are Settings adjustements
	initEncodeContext();
	initDecodeContext();
}

void VideoCodec::initEncodeContext(){
		
	pair<int,int> tmp; 
	pair<int,int> Encodetmp; 
	//initialize basic encoding context
	_encodeCodecCtx = avcodec_alloc_context();

	//TODO change if it's not the webcam 
	tmp = _cmdRes->getResolution();
	Encodetmp = getSpecialResolution(tmp.first);
	_encodeCodecCtx->width = Encodetmp.first;
	_encodeCodecCtx->height = Encodetmp.second;
	_encodeCodecCtx->rtp_payload_size = 1400;
 	_encodeCodecCtx->time_base= (AVRational){1,25};
	_encodeCodecCtx->gop_size = GOP_SIZE;
	_encodeCodecCtx->pix_fmt = PIX_FMT_YUV420P;
	_encodeCodecCtx->max_b_frames = MAX_B_FRAMES;

		if(avcodec_open(_encodeCodecCtx, _CodecENC) < 0)
		ptracesfl("CANNOT OPEN ENCODE CODEC",MT_FATAL,1,true);
	
	printf("init ENCODE width: %i\nheight: %i\nwidth: %i\nheight: %i\n",tmp.first,tmp.second,Encodetmp.first,Encodetmp.second);

	encodeSWS = new SWSInterface(tmp.first,tmp.second,PIX_FMT_RGB24,Encodetmp.first,Encodetmp.second,PIX_FMT_YUV420P);
}
void VideoCodec::initDecodeContext()
{

	pair<int,int> codetmp; 
	pair<int,int> tmp = _cmdRes->getResolution();

	//initialize basic encoding context
	_decodeCodecCtx = avcodec_alloc_context();


//TODO change if it's not the webcam - NOW ENCODING IN H263 FORMAT

	tmp = _cmdRes->getResolution();
	codetmp = getSpecialResolution(tmp.first);
	_decodeCodecCtx->width = codetmp.first;
	_decodeCodecCtx->height = codetmp.second;

	printf("init DECODE width: %i\nheight: %i\nwidth: %i\nheight: %i\n",tmp.first,tmp.second,codetmp.first,codetmp.second);
	if(avcodec_open (_decodeCodecCtx, _CodecDEC) < 0)
		ptracesfl("CANNOT OPEN DECODE CODEC",MT_FATAL,1,true);

	//intialize SWSdecodeContext

	decodeSWS = new SWSInterface(codetmp.first,codetmp.second,PIX_FMT_YUV420P ,
	tmp.first,tmp.second,PIX_FMT_RGB24);
}

void VideoCodec::quitDecodeContext()
{
	avcodec_close(_decodeCodecCtx);
	av_free(_decodeCodecCtx);
	delete decodeSWS;
}

void VideoCodec::quitEncodeContext()
{
	//initialize basic encoding context
	avcodec_close(_encodeCodecCtx);
	av_free(_encodeCodecCtx);
	delete encodeSWS;
}

VideoCodec::VideoCodec(char* codec){
	
	this->_codecName = codec;
	this->_encodeCodecCtx = NULL;
	this->_decodeCodecCtx = NULL;
	init();

}

 pair<int,int> VideoCodec::getSpecialResolution(int width)
 {
 // Text from libavcodec
 //128x96, 176x144, 352x288, 704x576, and 1408x1152
 pair<int,int> returnSize;
 
 
 if (width <= 128){
	 returnSize.first = 128;
	 returnSize.second = 96;
 }
 else if (width >= 129 && width < 352){
	 returnSize.first = 176;
	 returnSize.second = 144;
 }
 else if (width >= 352 && width < 704){
	 returnSize.first = 352;
	 returnSize.second = 288;
 }
 else if (width >= 704 && width < 1408){
	 returnSize.first = 704;
	 returnSize.second = 576;
 }
 else
 {
	 returnSize.first = 1408;
	 returnSize.second = 1152;
 }
 
 return returnSize;
 }
VideoCodec::VideoCodec(){
	
	this->_codecName = NULL;
	this->_encodeCodecCtx = NULL;
	this->_decodeCodecCtx = NULL;
	init();

}

 VideoCodec::~VideoCodec()
 {}
 
 
    
    
    
    