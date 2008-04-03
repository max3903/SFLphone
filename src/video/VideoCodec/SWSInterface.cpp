

#include "SWSInterface.h"

bool SWSInterface::Convert(AVFrame *INFrame,AVFrame *OUTFrame)
{
	int infoSize;
	
	infoSize = sws_scale(Context,INFrame->data,INFrame->linesize,in.width,
    		in.height,OUTFrame->data,OUTFrame->linesize);

    printf("SWSCALE : %i\n",infoSize);
    return true;
}


FrameProperties SWSInterface::getInputProperties(){ return in; }

void SWSInterface::setInputProperties(int setWidth,int setHeight,int setPixFormat)
{ 

 		this->in.height = setHeight;
	 this->in.width = setWidth;
	 this->in.pixFormat = setPixFormat;
}
void SWSInterface::setInputProperties(int setWidth,int setHeight)
{ 

 		this->in.height = setHeight;
	 this->in.width = setWidth;
}

FrameProperties SWSInterface::getOutputProperties(){ return out; }

void SWSInterface::setOutputProperties(int setWidth,int setHeight,int setPixFormat)
{
	 this->out.height = setHeight;
	 this->out.width = setWidth;
	 this->out.pixFormat = setPixFormat; 
}
void SWSInterface::setOutputProperties(int setWidth,int setHeight)
{
	 this->out.height = setHeight;
	 this->out.width = setWidth;
}


 
AVFrame *SWSInterface::alloc_picture420P(int width, int height) {
  AVFrame *picture;
  uint8_t *picture_buf;
  int size;
  picture = avcodec_alloc_frame();
  if (!picture) return NULL;
  size = avpicture_get_size(PIX_FMT_YUV420P, width, height);
  picture_buf = (uint8_t *) av_malloc(size);
  if (!picture_buf) {
    av_free(picture);
    return NULL;
  }
  avpicture_fill((AVPicture *)picture, picture_buf, PIX_FMT_YUV420P, width, height);
  return picture;
}
AVFrame * SWSInterface::alloc_picture420P(int width, int height,uint8_t *buffer) {
  AVFrame *picture;
  int size;
  picture = avcodec_alloc_frame();
  if (!picture) return NULL;
  size = avpicture_get_size(PIX_FMT_YUV420P, width, height);
  
  if (!buffer) {
    av_free(picture);
    return NULL;
  }
  avpicture_fill((AVPicture *)picture, buffer, PIX_FMT_YUV420P, width, height);
  return picture;
}

AVFrame * SWSInterface::alloc_pictureRGB24(int width, int height) {
  AVFrame *pFrameRGB = avcodec_alloc_frame();
  if(pFrameRGB==NULL) return NULL;
  int numBytes = avpicture_get_size(PIX_FMT_RGB24, width, height);
  uint8_t *buffer= (uint8_t *)av_malloc(numBytes);
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, width, 
height);
  return pFrameRGB;
}

AVFrame *SWSInterface::alloc_pictureRGB24(int width, int height,uint8_t *buffer) {
  AVFrame *pFrameRGB = avcodec_alloc_frame();
  if(pFrameRGB==NULL) return NULL;
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, width, height);
  if (!buffer) {
    av_free(pFrameRGB);
    return NULL;
  }
  return pFrameRGB;
}

SWSInterface::SWSInterface(int InWidth,int InHeight,int InPixFormat,int OutWidth,int OutHeight,int OutPixFormat)
 {
 
 in.height = InHeight;
 in.width = InWidth;
 in.pixFormat = InPixFormat;
 out.height = OutHeight;
 out.width = OutWidth;
 out.pixFormat = OutPixFormat;
 
BufferSize = avpicture_get_size( out.pixFormat,out.width, out.height);
 
 Context = sws_getContext( in.width,in.height,in.pixFormat,
	 							out.width,out.height,out.pixFormat,0,NULL,NULL,NULL);
	 							
 }
SWSInterface::~SWSInterface(){}
