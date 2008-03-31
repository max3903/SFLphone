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
#define INBUF_SIZE 4096

int VideoCodec::videoEncode(uint8_t *in_buf, uint8_t* out_buf,int in_bufferSize,int inWidth,int inHeight)
{
	
	AVFrame *IN,*SWS;
	uint8_t *SWS_BUF;
	
	//TODO check better buffer size
	out_buf = (uint8_t*)malloc(in_bufferSize);
	
	//Step 1: change in_buf in AVFRAME pict
	if ((IN = avcodec_alloc_frame()) == NULL)
         ptracesfl("OUT OF MEMORY TRYING TO ENCODE",MT_ERROR,1,true);
	     
    //get RGB picture
    avpicture_fill((AVPicture *)IN, in_buf,PIX_FMT_RGB24, inWidth,inHeight);         

 	SWS_BUF = encodeSWS->Convert(IN,SWS);
 	av_free(IN);
    free(in_buf);
	//Step 2:Encode
	//TODO GET A PROPER BUFFER SIZE
	avcodec_encode_video(_encodeCodecCtx, out_buf, 100000, SWS);

	//Step 3:Clean
	av_free(SWS);
    free(SWS_BUF);
	free(out_buf);

	
	//return outBufferSize
	return 100000;
	
	}

	//decode one frame

int VideoCodec::videoDecode(uint8_t *in_buf, uint8_t* out_buf,int size)
{
	
	int bytesRemaining = size;
	int got_picture_ptr = 0;
	int bytesDecoded=0;
	uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE], *inbuf_ptr;
	AVFrame *pict;

	memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);
	
	
	
	pict = avcodec_alloc_frame();
	if (pict == NULL)
         return -1;
                
               
   // avpicture_fill((AVPicture *)pict, out_buf+FF_INPUT_BUFFER_PADDING_SIZE,PIX_FMT_RGB24, width, height);
	
	while(bytesRemaining > 0){
		
	bytesDecoded = avcodec_decode_video(_decodeCodecCtx, pict,
            &got_picture_ptr, in_buf, bytesRemaining);

        if(bytesDecoded < 0)
            return false;

        bytesRemaining -= bytesDecoded;
		in_buf+=bytesDecoded;
		
		if(got_picture_ptr)
	{
	
	
	
	
	
	}
        
    }

    // Decode the rest of the frame
    bytesDecoded=avcodec_decode_video(_decodeCodecCtx, pict, &got_picture_ptr, in_buf, bytesRemaining);

	//
	if(got_picture_ptr)
	{
	
	
	
	
	
	}

     
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
	_Codec = _videoDesc->getDefaultCodec();
	_codecName = _Codec->name;
	}
	else
	_Codec = _videoDesc->getCodec(_codecName);
	
	//These are Settings adjustements
	initEncodeContext();
	initDecodeContext();

}
void VideoCodec::initEncodeContext(){

	
	pair<int,int> tmp; 
	//initialize basic encoding context
	
	_encodeCodecCtx = _videoDesc->getCodecContext(_Codec);
	
	//TODO change if it's not the webcam
	if (_cmdRes != NULL){
	tmp = _cmdRes->getResolution();
	tmp = getSpecialResolution(tmp.first);
	_encodeCodecCtx->width = tmp.first;
	_encodeCodecCtx->height = tmp.second;
	
	}
	else
	{
	_encodeCodecCtx->width = DEFAULT_WIDTH;
	_encodeCodecCtx->height = DEFAULT_HEIGHT;	
	}
	
	//TODO PUT IN VIDEOCODECDESCRIPTOR ----------------------
	_encodeCodecCtx->bit_rate = VIDEO_BIT_RATE;
	if (_codecName == "h264")
	_encodeCodecCtx->me_method = 8;
	else
	_encodeCodecCtx->me_method = 7;
	
	_encodeCodecCtx->time_base.den = STREAM_FRAME_RATE;
	_encodeCodecCtx->time_base.num = 1;
	_encodeCodecCtx->gop_size = GOP_SIZE;
	_encodeCodecCtx->pix_fmt = PIX_FMT_YUV420P;
	_encodeCodecCtx->max_b_frames = MAX_B_FRAMES;
	_encodeCodecCtx->mpeg_quant = 0;
	if (_codecName == "h264")
	_encodeCodecCtx->idct_algo = FF_IDCT_H264;
	else
	_encodeCodecCtx->idct_algo = FF_IDCT_AUTO;
	
	_encodeCodecCtx->mb_decision = FF_MB_DECISION_BITS;
	//TODO ADD VLC MATRIX
	_encodeCodecCtx->intra_matrix = NULL;
	_encodeCodecCtx->inter_matrix = NULL;
	_encodeCodecCtx->workaround_bugs = FF_BUG_AUTODETECT;
	
//	#define X264_PART_I4X4 0x001  /* Analyse i4x4 */
//#define X264_PART_I8X8 0x002  /* Analyse i8x8 (requires 8x8 transform) */
//#define X264_PART_P8X8 0x010  /* Analyse p16x8, p8x16 and p8x8 */
//#define X264_PART_P4X4 0x020  /* Analyse p8x4, p4x8, p4x4 */
//#define X264_PART_B8X8 0x100  /* Analyse b16x8, b8x16 and b8x8 */
//	_encodeCodecCtx->partitions = 

	if(avcodec_open (_encodeCodecCtx, _Codec) < 0)
		ptracesfl("CANNOT OPEN ENCODE CODEC",MT_FATAL,1,true);
	
//TODO END PUT IN VIDEOCODECDESCRIPTOR ----------------------
	

}
void VideoCodec::initDecodeContext()
{

	//initialize basic encoding context
	_decodeCodecCtx = avcodec_alloc_context();
	
	_decodeCodecCtx->pix_fmt = PIX_FMT_YUV420P;
	_decodeCodecCtx->max_b_frames = MAX_B_FRAMES;
	//TODO ALLOCATE MORE MEM???
	//codec_tag
	if(_Codec->capabilities&CODEC_CAP_TRUNCATED)
	  _decodeCodecCtx->flags|= CODEC_FLAG_TRUNCATED;

	//if(avcodec_open (_decodeCodecCtx, _Codec) < 0)
	//	ptracesfl("CANNOT OPEN DECODE CODEC",MT_FATAL,1,true);
	
	//intialize SWSdecodeContext
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
 else if (width <= 176){
	 returnSize.first = 176;
	 returnSize.second = 144;
 }
 else if (width <= 352){
	 returnSize.first = 352;
	 returnSize.second = 288;
 }
 else if (width <= 704){
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
/*
 * 
 * 
 * Tran Huu Tri wrote:
> Hi,
> 
> I need to write the program that capture buffer from webcam
> in RGB format and use ffmpeg codecs library to convert to
> mpeg video file with the codecs as CODEC_ID_MPEG1VIDEO or
> CODEC_ID_H263,…
> 
>  
> 
> But I don’t know how to convert RGB buffer to AVFrame
> structure.
> 

delphi code:

const
   in_fmt: PIX_FMT_YUV420P;
var
   YUVframe, RGBframe, ENCframe: PAVFrame;
   YUVbuf,
   RGBbuf: pointer;
   size: integer;
   dwWidth, dwHeight: integer;

   codec_ctx: PAVCodecContext;
   codec: PAVCodec;

...

   YUVframe := avcodec_alloc_frame ();
   size := avpicture_get_size (in_fmt, dwWidth, dwHeight);
   YUVbuf := AllocMem (size);
   avpicture_fill (PAVPicture (YUVframe), YUVbuf, in_fmt, dwWidth, 
dwHeight);

   RGBframe := avcodec_alloc_frame ();
   size := avpicture_get_size (PIX_FMT_RGBA32, dwWidth, dwHeight);
   RGBbuf := AllocMem (size);
   avpicture_fill (PAVPicture (RGBframe), RGBbuf, PIX_FMT_RGBA32, 
dwWidth, dwHeight);

   codec_ctx := avcodec_alloc_context ();

   codec_ctx.codec_type := CODEC_TYPE_VIDEO;
   codec_ctx.codec_id := CODEC_ID_XVID;

   codec_ctx.bit_rate := 100000;
   codec_ctx.time_base.num := 25;
   codec_ctx.time_base.den := 1;
   codec_ctx.pix_fmt := in_fmt;

   codec := avcodec_find_encoder (codec_ctx.codec_id);
   avcodec_open (codec_ctx, codec);

...
getting data in RGBbuf
...

    img_convert (PAVPicture (YuvFrame), in_fmt,
                 PAVPicture (RgbFrame), PIX_FMT_RGBA32,
                 dwWidth, dwHeight);
    avcodec_encode_video (codec_ctx, EncodedBuf, 100000, YuvFrame);
    
    */
    
    
    
    
    /*
     * 
     * // A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
 
#include <string>
#include <iostream>
#include <stdexcept>
 
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
 
#include <Data/VideoParser.hpp>
 
using namespace std;
using namespace caviar;
 
string VideoParser::SaveFrame(AVFrame *pFrame, int width, int height) {
    FILE *pFile;
    char szFilename[32];
    int y;
 
    // Open file
    sprintf(szFilename, "frame.ppm");
    pFile=fopen(szFilename, "wb");
    if (pFile==NULL)
        throw runtime_error("Could not open file (VideoParser::SaveFrame)");
 
    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);
 
    // Write pixel data
    for (y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
 
    // Close file
    fclose(pFile);
    return szFilename;
}
 
VideoParser::VideoParser(const string& filename) {
 
    // Register all formats and codecs
    av_register_all();
 
    // Open video file
    if (av_open_input_file(&pFormatCtx, filename.c_str(), NULL, 0, NULL)!=0)
        throw runtime_error("Couldn't open file");
 
    // Retrieve stream information
    if (av_find_stream_info(pFormatCtx)<0)
        throw runtime_error("Couldn't find stream information");
 
    // Dump information about file onto standard error
    dump_format(pFormatCtx, 0, filename.c_str(), 0);
 
    // Find the first video stream
    videoStream=-1;
    for (int i=0; i<pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) {
            videoStream=i;
            break;
        }
    if (videoStream==-1)
        throw runtime_error("Didn't find a video stream");
 
    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;
 
    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec==NULL) {
        cerr << "Unsupported codec!\n";
        throw runtime_error("Codec not found");
    }
    // Open codec
    if (avcodec_open(pCodecCtx, pCodec)<0)
        throw runtime_error("Could not open codec");
 
    // Allocate video frame
    pFrame=avcodec_alloc_frame();
 
    // Allocate an AVFrame structure
    pFrameRGB=avcodec_alloc_frame();
    if (pFrameRGB==NULL)
        throw runtime_error("Allocation problem");
 
    // Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
            pCodecCtx->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
 
    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
            pCodecCtx->width, pCodecCtx->height);
}
 
string VideoParser::getFrame() {
    int frameFinished=0;
    string frameFilename("");
 
    while (!frameFinished) {
        if (av_read_frame(pFormatCtx, &packet)>=0) {
            // Is this a packet from the video stream?
            if (packet.stream_index==videoStream) {
                // Decode video frame
                avcodec_decode_video(pCodecCtx, pFrame, &frameFinished,
                        packet.data, packet.size);
 
                // Did we get a video frame?
                if (frameFinished) {
                    // Convert the image from its native format to RGB
                    img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24,
                            (AVPicture*)pFrame, pCodecCtx->pix_fmt,
                            pCodecCtx->width, pCodecCtx->height);
 
                    // Save the frame to disk
                    frameFilename = SaveFrame(pFrameRGB, pCodecCtx->width,
                            pCodecCtx->height);
                }
            }
 
            // Free the packet that was allocated by av_read_frame
            av_free_packet(&packet);
        }
    }
    return frameFilename;
}
 

// * Free the RGB image
// * Free the YUV frame
// * Close the codec
// * Close the video file
// * 
VideoParser::~VideoParser() {
    av_free(buffer);
    av_free(pFrameRGB);
    av_free(pFrame);
    avcodec_close(pCodecCtx);
    av_close_input_file(pFormatCtx);
}
*/



	/*
	 * static int frameFinished = 0;  //this is the got_picture_pointer
argument
    int bytesDecoded=0;
    int bytesRemaining = data_size;

    while(bytesRemaining > 0)
    {
        bytesDecoded=avcodec_decode_video(pCodecCtx, pFrame,
            &frameFinished, rawData, bytesRemaining);

        if(bytesDecoded < 0)
            return false;

        bytesRemaining-=bytesDecoded;
        rawData+=bytesDecoded;

        if(frameFinished)
            return true;
    }

    // Decode the rest of the last frame
    bytesDecoded=avcodec_decode_video(pCodecCtx, pFrame, &frameFinished,

        rawData, bytesRemaining);

    return frameFinished!=0;
	 * 
	 * */
	
