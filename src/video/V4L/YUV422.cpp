/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc. 
 *  Author: Jean Tessier <jean.tessier@polymtl.ca>
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

#include "YUV422.h"

YUV422::YUV422(){}

YUV422::YUV422( int fd ){
	this->enumerateFrameSizes(fd);
	this->YUV2RGB_init();
}

YUV422::~YUV422(){}

int YUV422::getType(){
	return V4L2_PIX_FMT_YUYV;
}

void YUV422::init(){ }

void YUV422::convert(unsigned char* input, unsigned char* ouput, int w, int h){
 	
 	unsigned char *u,*u1,*v,*v1;
	int Y=0,U=0,V=0,i,j;
	
	int width= w;
	int height= h;

	u=input+1; v=input+3;
	for(i=0;i<width*height;i++) {
		Y=(*input)-16;
		U=(*u)-128;
		V=(*v)-128;
		write_rgb(&ouput,Y,U,V);
		input+=2;
		if((i&1)==1) { u+=4; v+=4; }
	}
}

char* YUV422::getStringType(){
	return "YUV422";
}
