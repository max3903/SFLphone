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
	
	// Check if everything  is set properly
	if(in_buf == NULL) 	{ptracesfl("CAN'T Encode - Input Buffer problem\n",MT_ERROR,1,true);return -1;}
	if(out_buf == NULL) {ptracesfl("CAN'T Encode - output Buffer problem\n",MT_ERROR,1,true);return -1;}
	if( inWidth <= 0 ||inHeight <= 0) 
	{ptracesfl("CAN'T Encode - Incorrect size\n",MT_ERROR,1,true);return -1;}

	// Create AVframe structures for encoding changes
	IN  =  encodeSWS->alloc_pictureRGB24(inWidth,inHeight,in_buf);
	SWS = encodeSWS->alloc_picture420P(_encodeCodecCtx->width,_encodeCodecCtx->height);
	
	if(IN != NULL || SWS != NULL)
	{
		if (encodeSWS->Convert(IN,SWS) == false)
 		{ptracesfl("Conversion error\n",MT_ERROR,1,true);return -1;}
 	}
 	else {ptracesfl("Conversion error - NULL Frames\n",MT_ERROR,1,true);return -1;}


	//Step 2:Encode
	//TODO GET A PROPER BUFFER SIZE
	if ( (outsize = avcodec_encode_video(_encodeCodecCtx, out_buf, FF_MIN_BUFFER_SIZE, SWS))<= 0)
		{ptracesfl("Encoding error\n",MT_ERROR,1,true);return -1;}
		
	// free codecs
	av_free(SWS);
	av_free(IN);


	//return outBufferSize
	return outsize;
	}

int VideoCodec::videoDecode(uint8_t *in_buf, uint8_t* out_buf,int inSize)
{
	int frame, got_picture, len;
	uint8_t *buf;
	AVFrame *SWS,*OUT;
	
	// Check if everything  is set properly
	if(in_buf == NULL) 	{ptracesfl("CAN'T Encode - Input Buffer problem\n",MT_ERROR,1,true);return -1;}
	if(out_buf == NULL) {ptracesfl("CAN'T Encode - output Buffer problem\n",MT_ERROR,1,true);return -1;}
	
	//set the libavcodec special memory space
	if((buf = (uint8_t*)av_malloc(inSize+FF_INPUT_BUFFER_PADDING_SIZE)) == NULL)
	{ptracesfl("Malloc Error\n",MT_ERROR,1,true);return -1;};
    if( memcpy(buf,in_buf,inSize) ==NULL)
	{ptracesfl("MemCpy Error\n",MT_ERROR,1,true);return -1;};
	if ( memset(buf + inSize, 0, FF_INPUT_BUFFER_PADDING_SIZE) == NULL)
	{ptracesfl("MemSet Error\n",MT_ERROR,1,true);return -1;};
	
	// init output picture decoded
    SWS = decodeSWS->alloc_picture420P(_decodeCodecCtx->width,_decodeCodecCtx->height);
	if(SWS == NULL)
	{ptracesfl("CAN'T Decode OUtput picture allocation problem\n",MT_ERROR,1,true);return -1;}
	
	if ( avcodec_decode_video(_decodeCodecCtx, SWS, &got_picture,buf,inSize ) < 0)
		{ptracesfl("CAN'T Decode - couldn't decode\n",MT_ERROR,1,true);return -1;}

	// Maybe you didn't get a full picture!
	if (got_picture == 0)
	{ptracesfl("Could not get full frame\n",MT_INFO,4,true);return -1;}
	
	
   	OUT = decodeSWS->alloc_pictureRGB24(DEFAULT_WIDTH,DEFAULT_HEIGHT,out_buf)	;
   
   if (decodeSWS->Convert(SWS,OUT) == false)
 		{ptracesfl("Conversion error\n",MT_ERROR,1,true);return -1;}
   
    av_free(SWS);
    av_free(buf);
    av_free(OUT);
	
	return avpicture_get_size(PIX_FMT_RGB24, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

void VideoCodec::init(){
	
	ptracesfl("VideoCodec initialisation",MT_INFO,5,true);
	
	//Get VideoDescriptor Instance
	_videoDesc = VideoCodecDescriptor::getInstance();
	
	//Get V4LManager instance
	_v4lManager = VideoDeviceManager::getInstance();
	_cmdRes = (Resolution*)_v4lManager->getCommand(VideoDeviceManager::RESOLUTION);


	
	//These are Settings adjustements
	initEncodeContext();
	initDecodeContext();
}

void VideoCodec::initEncodeContext(){
		
	pair<int,int> tmp; 
	pair<int,int> Encodetmp; 
	//initialize basic encoding context
	_encodeCodecCtx = avcodec_alloc_context();
	
	//WIDTH AND HEIGHT INIT CHANGE TO MIXER SIZES ///////////////////
	tmp = _cmdRes->getResolution();

	if(_CodecENC->id == CODEC_ID_H263)
	{
	Encodetmp = getSpecialResolution(tmp.first);
	_encodeCodecCtx->width = Encodetmp.first;
	_encodeCodecCtx->height = Encodetmp.second;
	}
	else
	{
	Encodetmp = tmp;
	_encodeCodecCtx->width = Encodetmp.first;
	_encodeCodecCtx->height = Encodetmp.second;
	}
	/////////////////////////////////////////////
	
	
	/////////////VIDEO SETTINGS.H settings///////////
	_encodeCodecCtx->rtp_payload_size = RTP_PAYLOAD;
 	_encodeCodecCtx->time_base= (AVRational){1,STREAM_FRAME_RATE};
	_encodeCodecCtx->gop_size = GOP_SIZE;
	_encodeCodecCtx->pix_fmt = PIX_FMT_YUV420P;
	_encodeCodecCtx->max_b_frames = MAX_B_FRAMES;
	_encodeCodecCtx->mpeg_quant = 0;
	//todo change to users choice
	_encodeCodecCtx->bit_rate = _videoDesc->getEncodingBitRate();
	//_encodeCodecCtx->flags 
	
	
	/// other
	if(_CodecENC->id == CODEC_ID_H264)
	{
	_encodeCodecCtx->me_method = 8;
	_encodeCodecCtx->idct_algo = FF_IDCT_H264;
	_encodeCodecCtx->ildct_cmp = FF_CMP_DCT264;
	}
	else
	{
	_encodeCodecCtx->me_method = 6;
	_encodeCodecCtx->idct_algo = FF_IDCT_AUTO;
	}

	//intra or inter matrix

	/////

	// CHECK if we can pair both
		if(avcodec_open(_encodeCodecCtx, _CodecENC) < 0)
		ptracesfl("CANNOT OPEN ENCODE CODEC",MT_FATAL,1,true);
	
	// in -> from the mixer -> out special size if h263, same size otherwise
	encodeSWS = new SWSInterface(tmp.first,tmp.second,PIX_FMT_RGB24,Encodetmp.first,Encodetmp.second,PIX_FMT_YUV420P);
}
void VideoCodec::initDecodeContext()
{

	pair<int,int> codetmp; 
	pair<int,int> tmp = _cmdRes->getResolution();

	//initialize basic decoding context
	_decodeCodecCtx = avcodec_alloc_context();
	
	//WIDTH AND HEIGHT INIT CHANGE TO MIXER SIZES ///////////////////
	tmp = _cmdRes->getResolution();
	if(_CodecDEC->id == CODEC_ID_H263)
	{
	codetmp = getSpecialResolution(tmp.first);
	_decodeCodecCtx->width = codetmp.first;
	_decodeCodecCtx->height = codetmp.second;
	}
	else
	{
	codetmp = tmp;
	_decodeCodecCtx->width = codetmp.first;
	_decodeCodecCtx->height = codetmp.second;
	}
	/////////////////////////////////////////////

	if(avcodec_open (_decodeCodecCtx, _CodecDEC) < 0)
		ptracesfl("CANNOT OPEN DECODE CODEC",MT_FATAL,1,true);

	//intialize SWSdecodeContext
	decodeSWS = new SWSInterface(codetmp.first,codetmp.second,PIX_FMT_YUV420P,
	DEFAULT_WIDTH,DEFAULT_HEIGHT,PIX_FMT_RGB24);
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
	

		//check if active Codec
	if(codec == NULL)
	ptracesfl("CODEC ERROR",MT_FATAL,1,true);
	
	if ((_CodecENC = avcodec_find_encoder_by_name(codec)) == NULL)
		ptracesfl("CODEC ERROR",MT_FATAL,1,true);
		
	if ((_CodecDEC = avcodec_find_decoder_by_name(codec)) == NULL)
		ptracesfl("CODEC ERROR",MT_FATAL,1,true);

	init();
}

VideoCodec::VideoCodec(enum CodecID id){
	

		//check if active Codec
	if(id == 0)
	ptracesfl("CODEC ERROR",MT_FATAL,1,true);
	
	if ((_CodecENC = avcodec_find_encoder(id)) == NULL)
		ptracesfl("CODEC ERROR",MT_FATAL,1,true);
		
	if ((_CodecDEC = avcodec_find_decoder(id)) == NULL)
		ptracesfl("CODEC ERROR",MT_FATAL,1,true);

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
 
 
 
VideoCodec::VideoCodec(){}

 VideoCodec::~VideoCodec(){
 
quitEncodeContext();
quitDecodeContext();

 }
 