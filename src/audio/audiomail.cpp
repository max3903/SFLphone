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


AudioMail::AudioMail(const std::string& folder, const std::string& name)
	: AudioLoop(),
	_folderName(folder),
	_name(name)
{
  // could vary later ...
  _start = false;
}

AudioMail::~AudioMail()
{
}

// load file in mono format
bool
AudioMail::loadMail(const std::string& filename, const std::string& extension, AudioCodec* codec, unsigned int sampleRate=8000) {
	_codec = codec;
	
	unsigned short int UNCOMPRESSED_SAMPLE;
	unsigned short int COMPRESSED_SAMPLE;
	if( extension == "gsm" ) { // GSM
		UNCOMPRESSED_SAMPLE = 33;
		COMPRESSED_SAMPLE   = 160;
	} else if( extension == "WAV" ) { // wav-gsm
		UNCOMPRESSED_SAMPLE = 65;
		COMPRESSED_SAMPLE   = 160;
	} else if( extension == "ul") {
		UNCOMPRESSED_SAMPLE = 1;
		COMPRESSED_SAMPLE   = 2;
	}
	
	/** file already launched */
	if( _buffer != NULL ) {
		reset();
		return true;
	}

	// no filename to load
	if( filename.empty() ) {
		_debug("Unable to open audio mail : filename is empty\n");
		return false;
	}

	int length = filename.size();
	/*unsigned char fileBuffer[length];
	for( int i = 0 ; i < filename.size() ; i++ ) {
		fileBuffer[i] =  filename[i];
	}*/

	unsigned int expandedsize = 0;
	int16 monoBuffer[(length / UNCOMPRESSED_SAMPLE) * COMPRESSED_SAMPLE];
	std::cout << "monoBuffer before : " << ((length / UNCOMPRESSED_SAMPLE) * COMPRESSED_SAMPLE) << std::endl;
	std::cout << "   '- length              : " << length << std::endl;
	std::cout << "   '- UNCOMPRESSED_SAMPLE : " << UNCOMPRESSED_SAMPLE << std::endl;
	std::cout << "   '- COMPRESSED_SAMPLE   : " << COMPRESSED_SAMPLE << std::endl;
	if( extension == "gsm" ) { // GSM
		expandedsize = decode_gsm(filename, monoBuffer);
	} else if( extension == "WAV" ) {
		expandedsize = decode_wav(filename, monoBuffer);
	} else {
		expandedsize = decode_ul(filename, monoBuffer);
	}

	std::cout << "expanded size : " << expandedsize << std::endl;
/*	if( expandedsize != length/UNCOMPRESSED_SAMPLE*COMPRESSED_SAMPLE ) {
		_debug("Audio file error on loading audio file (%d/%d)!", expandedsize , length/33*320 );
		return false;
	}*/
	unsigned int nbSampling = expandedsize/sizeof(int16);

//	std::fstream file;
//	file.open("/tmp/toto.raw", std::fstream::out | std::fstream::binary );
	//for(int i = 0 ; i < expandedsize/2 ; i++ ) {
//		file << monoBuffer;//[i];
	//}
//	file.close();
	

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

#ifdef DATAFORMAT_IS_FLOAT
    // case number 1: the output is float32 : convert directly in _bufferTmp
    src_data.data_out = bufferTmp;
    src_simple(&src_data, SRC_SINC_BEST_QUALITY, 1);
#else
	// case number 2: the output is int16 : convert and change to int16
	float* floatBufferOut = new float[sizeOut];
	src_data.data_out = floatBufferOut;

	src_simple(&src_data, SRC_SINC_BEST_QUALITY, 1);
	src_float_to_short_array(floatBufferOut, bufferTmp, src_data.output_frames_gen);

	delete [] floatBufferOut;
#endif
	delete [] floatBufferIn;

	// if we are in mono, we send the bufferTmp location and don't delete it
	// else we split the audio in 2 and put it into buffer
	_size = src_data.output_frames_gen;
	_buffer = bufferTmp;  // just send the buffer pointer;
	bufferTmp = 0;

/*	file.open("/tmp/titi.raw", std::fstream::out | std::fstream::binary );
	for(int i = 0 ; i < _size ; i++ ) {
		file << _buffer[i];
	}
	file.close();*/

	return true;
}


unsigned int AudioMail::decode_gsm(const std::string& filename, int16 * monoBuffer) {
	
	int length = filename.size();
	unsigned char fileBuffer[length];
	for( int i = 0 ; i < filename.size() ; i++ ) {
		fileBuffer[i] =  filename[i];
	}
	std::cout << "decode_gsm : monoBuffer[" << (length / 33) * 160 << "]" << std::endl;
	std::cout << "     '- length : " << length << std::endl;
	
	unsigned int expandedsize = 0;
	for( int i = 0 ; i < (length/33) ; i++ ) {
		unsigned char buffer33[33];
		int16         buffer160[160];

		memset(buffer33, 0, 33-1);
		memset(buffer160, 0, 160);
		std::cout << "i : " << (i*33) << " - i++ : " << (i*33+32) << std::endl;
		if( (i*33+33-1) > length ) {
			int rest = i * 33 + 33 -1 - length;
			memcpy(buffer33, fileBuffer+(i*33), rest);
			memcpy(buffer33+rest, 0, 33-rest-1);
		} else {
			memcpy(buffer33, fileBuffer+(i*33), 33-1);
		}
//		int j = 0;
		/*while( j < 33-1 ) {
			buffer33[j] = buffer33[j] << 16;
			j++;
		}*/
		expandedsize += _codec->codecDecode(buffer160, buffer33, 160);
//		std::cout << "expanded : " << expandedsize << std::endl;
		memcpy(monoBuffer+(i*160), buffer160, 160); // mettre iteration si ordering a l`envers
		/*while( j < 160-1 ) {
			monoBuffer[j] = monoBuffer[j] << 16;
			j++;
		}*/
	}
//	mono = monoBuffer;
	return expandedsize;
}


unsigned int AudioMail::decode_wav(const std::string& filename, int16 * monoBuffer) {
	unsigned int length = filename.size();
	unsigned char fileBuffer[length];
	for( int i = 0 ; i < filename.size() ; i++ ) {
		fileBuffer[i] =  filename[i];
	}
	std::cout << "decode_wav : monoBuffer[" << (length / 66) * 320 << "]" << std::endl;

	unsigned int expandedsize = 0;
	for( int i = 5 ; i < (length/66) ; i++ ) {
		unsigned char buffer66[66];
		int16         buffer320[320];

		memset(buffer66, 0, 66-1);
		int j = 0;
		/*int rest;
		if( (i*33+33-1) > length ) {
			int rest = i * 33 + 33 -1 - length;
			memcpy(buffer66, fileBuffer + i*33, rest);
			memcpy(buffer66 + rest, 0, i*33 + rest - 1);
		} else {*/
		memcpy(buffer66, fileBuffer+(i*33), 33);
		//}
		/*while( j < 33-1 ) {
			/*unsigned char c1, c2;
		    c1 = buffer66[j] & 255;
		    c2 = (buffer66[j] >> 8) & 255;
			buffer66[j] = (c1 << 8) + c2;*/
			/*buffer66[j] = buffer66[j] << 16;
			j++;
		}*/
		expandedsize += _codec->codecDecode(buffer320, buffer66, 33);
		//while( j < 160 ) {
			/*unsigned char c1, c2;
		    c1 = buffer320[j] & 255;
		    c2 = (buffer320[j] >> 8) & 255;
			buffer320[j] = (c1 << 8) + c2;*/
		/*	buffer320[j] = buffer320[j] << 16;
			j++;
		}*/
		std::cout << "codecDecode 1" << std::endl;
		memcpy(monoBuffer + (i*160), buffer320, 160-1); // mettre iteration si ordering a l`envers
		

		if( (i*66+33) > length ) {
			int rest = i * 66 -1 - length;
			memcpy(buffer66, fileBuffer + i*66, rest);
			memcpy(buffer66 + rest, 0, i*66 + rest - 1);
		} else {
			memcpy(buffer66, fileBuffer+(i*66), 33-1);
		}

		//while( j < 66-1 ) {
			/*unsigned char c1, c2;
		    c1 = buffer66[j] & 255;
		    c2 = (buffer66[j] >> 8) & 255;
			buffer66[j] = (c1 << 8) + c2;*/
			/*monoBuffer[j] = monoBuffer[j] << 16;
			j++;
		}*/
		expandedsize += _codec->codecDecode(buffer320+160, buffer66+33, 32);
		/*while( j < 320 ) {
			/*unsigned char c1, c2;
		    c1 = buffer320[j] & 255;
		    c2 = (buffer320[j] >> 8) & 255;
			buffer320[j] = (c1 << 8) + c2;*/
			/*buffer320[j] = buffer320[j] << 16;
			j++;
		}*/
		std::cout << "codecDecode 2" << std::endl;
		memcpy(monoBuffer + (i*320), buffer320+	160, 160-1); // mettre iteration si ordering a l`envers
		while( j < 320 ) {
			/*unsigned char c1, c2;
		    c1 = buffer320[j] & 255;
		    c2 = (buffer320[j] >> 8) & 255;
			buffer320[j] = (c1 << 8) + c2;*/
			buffer320[j] = buffer320[j] << 16;
			j++;
		}
	}
	return expandedsize;
}


unsigned int AudioMail::decode_ul(const std::string& filename, int16 * monoBuffer) {
	unsigned int  length       = filename.size();
	unsigned int  expandedsize = 0;
	unsigned char fileBuffer[length];

	for( int i = 0 ; i < length ; i++ ) {
		fileBuffer[i] = filename[i];
//		expandedsize = _codec->codecDecode(monoBuffer+i, fileBuffer+i, 1);
	}
	expandedsize = _codec->codecDecode(monoBuffer, fileBuffer, length);
	std::cout << "decode_ul : monoBuffer[" << length << "]" << std::endl;
	std::cout << "decode_ul : expandedsize => " << expandedsize << std::endl;
//	expandedsize = _codec->codecDecode(monoBuffer, fileBuffer, 33);
//	memcpy(monoBuffer, fileBuffer, length); // mettre iteration si ordering a l`envers*/
	return expandedsize;
}
