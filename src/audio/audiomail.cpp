/*
 *  Copyright (C) 2005-2007 Savoir-Faire Linux inc.
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *
 *  Inspired by tonegenerator of 
 *   Laurielle Lea <laurielle.lea@savoirfairelinux.com> (2004)
 *  Inspired by ringbuffer of Audacity Project
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
#include "audiomail.h"
#include "codecDescriptor.h"
#include <fstream>
#include <string.h>
#include <math.h>
#include <samplerate.h>


AudioMail::AudioMail()
 : AudioLoop()
{
  // could vary later ...
  _start = false;
}

AudioMail::~AudioMail()
{
}

// load file in mono format
bool
AudioMail::loadMail( const std::string& filename , AudioCodec* codec , unsigned int sampleRate=8000 ) 
{
	_codec = codec;

	// if the filename was already loaded, with the same samplerate 
	// we do nothing
	if( _filename == filename && _sampleRate == sampleRate ) {
		return true;
	} else {
		// reset to 0
		delete [] _buffer; _buffer = 0;
		_size = 0;
		_pos = 0;
	}

	// no filename to load
	if( filename.empty() ) {
		_debug("Unable to open audio file: filename is empty\n");
		return false;
	}

	std::fstream file;
	file.open( filename.c_str() , std::fstream::in | std::fstream::binary );
	if( ! file.is_open() ) {
		// unable to load the file
		_debug("Unable to open audio file %s\n", filename.c_str());
		return false;
	}

	// get length of file:
	file.seekg( 0 , std::ios::end );
	int length = file.tellg();
	std::cout << "--** length : " << length << std::endl;
	file.seekg( 0 , std::ios::beg );

	// allocate memory:
	char fileBuffer[ length ];

	// read data as a block:
	file.read( fileBuffer , length );
	file.close();
	
	int16 monoBuffer[ (length / 33) * 320 ];
	unsigned int expandedsize = 0;
	for( int i = 0 ; i < (length/33) ; i++ ) {
		char  buffer33[34];
		int16 buffer320[321];

		memset( buffer33, 0, 33 );
		if( (i*33) +33 > length ) {
//			std::cout << " i > length :: " << (i*33+33) << std::endl;
			int rest = i*33 + 33 - length;
			memcpy( buffer33, fileBuffer + i*33-1, rest );
			memcpy( buffer33, 0, i*33+1 + rest);
		} else {
//			std::cout << " memcpy( .. , ..+ " << (i*33) << " -> " << (i*33)+33 << std::endl;
			memcpy( buffer33, fileBuffer + i*33, 33 );
		}
		expandedsize += _codec->codecDecode( buffer320 , (unsigned char *)buffer33, length );
//		std::cout << "expandedsize : " << expandedsize << std::endl;
		memcpy( monoBuffer + (i*320), buffer320, 320 ); // mettre iteration si ordering a l`envers
	}
	std::cout << "expanded size : " << expandedsize << std::endl;
/*	if( expandedsize != length/33*320 ) {
		_debug("Audio file error on loading audio file (%d/%d)!", expandedsize , length/33*320 );
		return false;
	}*/
	unsigned int nbSampling = expandedsize/sizeof(int16);





	// we need to change the sample rating here:
	// case 1: we don't have to resample : only do splitting and convert
/*	_size   = nbSampling;
	std::cout << "size : " << _size << std::endl;
	std::cout << "size*sizeof(SFLDataFormat) : " << _size*sizeof(SFLDataFormat) << std::endl;
	_buffer = new SFLDataFormat[_size];
#ifdef DATAFORMAT_IS_FLOAT
	// src to dest
	src_short_to_float_array(monoBuffer, _buffer, nbSampling);
#else
	// dest to src
	memcpy(_buffer, monoBuffer, _size*sizeof(SFLDataFormat));
#endif*/



	double factord = (double)sampleRate / 8000;
	float* floatBufferIn = new float[nbSampling];
	int    sizeOut  = (int)(ceil(factord*nbSampling));
	src_short_to_float_array(monoBuffer, floatBufferIn, nbSampling);
	SFLDataFormat* bufferTmp = new SFLDataFormat[sizeOut];

	SRC_DATA src_data;
	src_data.data_in = floatBufferIn;
	src_data.input_frames = nbSampling;
	src_data.output_frames = sizeOut;
	src_data.src_ratio = factord;

	float* floatBufferOut = new float[sizeOut];
	src_data.data_out = floatBufferOut;

	src_simple (&src_data, SRC_SINC_BEST_QUALITY, 1);
	src_float_to_short_array(floatBufferOut, bufferTmp, src_data.output_frames_gen);

	delete [] floatBufferOut;
	delete [] floatBufferIn;
	nbSampling = src_data.output_frames_gen; 

	// if we are in mono, we send the bufferTmp location and don't delete it
	// else we split the audio in 2 and put it into buffer
	_size = nbSampling;
	_buffer = bufferTmp;  // just send the buffer pointer;
	bufferTmp = 0;


	return true;
}

