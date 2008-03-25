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


int VideoCodec::videoEncode(uint8_t *in_buf, uint8_t* out_buf,int bufferSize,int width,int height)
{
	AVCodecContext* _CodecCtx;
	const AVPicture *pict;
	_CodecCtx = _baseCodecCtx;

	//Step 1: change in_buf in AVFRAME pict
	pict = encodeConvert(in_buf,width,height, width, height);

	//Step 2:Open the codec
   avcodec_open(_CodecCtx,_Codec);

	//Step 3:Encode
	//avcodec_encode_video(_CodecCtx, out_buf, bufferSize, pict);

	//Step 4:Close
	avcodec_close(_CodecCtx);

	return 0;
	}
	
int VideoCodec::videoDecode(uint8_t *in_buf, uint8_t* out_buf  )
{
	AVCodecContext* _CodecCtx;
	return 0;
}

void VideoCodec::init(){
	
	//Getting Basic AVCodecContext settings from Codec Descriptor
	_videoDesc = VideoCodecDescriptor::getInstance();
	
	//check if active Codec
	_Codec = _videoDesc->getActiveCodec(_codecName);
	
	if(_Codec == NULL)
		ptracesfl("Not An active Codec",MT_FATAL,1,true);

	// Get normal Codec Context
	_baseCodecCtx = _videoDesc->getCodecContext(_Codec);

}


VideoCodec::~VideoCodec() {
	delete _videoDesc;
	_videoDesc = NULL;

}
VideoCodec::VideoCodec(char* codec){
	
	
	this->_codecName = codec;
	this->_baseCodecCtx = NULL;
	
	init();

}

VideoCodec::VideoCodec(){
//method to get codec

}


AVPicture* VideoCodec::encodeConvert(uint8_t *in_buf,int inWidth,int inHeight, int outWidth, int outHeight)
{
	static struct SwsContext *img_convert_ctx;
	AVPicture *pic;
	
	
	
	return pic;
	
	
}

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

