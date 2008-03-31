

#include "SWSInterface.h"

uint8_t *SWSInterface::Convert(AVFrame *INFrame,AVFrame *OUTFrame)
{
	uint8_t *OUT_buf;
	OUT_buf = (uint8_t*)malloc(BufferSize);
	int infoSize;
	
	//printf("BufferSize : %i\n",BufferSize);
	
	//printf("FILL\n");
	infoSize = avpicture_fill((AVPicture *)OUTFrame,OUT_buf,out.pixFormat,out.width,
    		out.height);
   // 		printf("INFOSIZE : %i\n",infoSize);
	//printf("SCALE\n");

	infoSize = sws_scale(Context,INFrame->data,INFrame->linesize,in.width,
    		in.height,OUTFrame->data,OUTFrame->linesize);
    
  //  printf("SWSCALE : %i\n",infoSize);
    return OUT_buf;
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
	 							out.width,out.height,out.pixFormat,SWS_PRINT_INFO,NULL,NULL,NULL);
	 							
 }


SWSInterface::~SWSInterface(){}
