

#include "SWSInterface.h"

uint8_t *SWSInterface::Convert(AVFrame *INFrame,AVFrame *OUTFrame)
{
	uint8_t *OUT_buf;
	
	OUT_buf = (uint8_t*)malloc(BufferSize);
	
	avpicture_fill((AVPicture *)OUTFrame,OUT_buf,out.pixFormat,out.width,
    		out.height);
	
	sws_scale(Context,INFrame->data,INFrame->linesize,in.width,
    		in.height,OUTFrame->data,OUTFrame->linesize);
    
    return OUT_buf;
}


FrameProperties SWSInterface::getIn(){ return in; }

void SWSInterface::setIn(int setWidth,int setHeight,int setPixFormat)
{ 

 		this->in.height = setHeight;
	 this->in.width = setWidth;
	 this->in.pixFormat = setPixFormat;
}

FrameProperties SWSInterface::getOut(){ return out; }

void SWSInterface::setOut(int setWidth,int setHeight,int setPixFormat)
{
	 this->out.height = setHeight;
	 this->out.width = setWidth;
	 this->out.pixFormat = setPixFormat; 
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



