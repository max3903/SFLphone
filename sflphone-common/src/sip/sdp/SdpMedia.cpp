/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */
#include "SdpMedia.h"
#include <string.h>
#include <sstream>
#include <iostream>

static const char* streamDirectionStr[] = { "sendrecv", "sendonly", "recvonly",
		"inactive" };

static const char* mediaTypeStr[] = { "audio", "video", "application", "text",
		"image", "message" };

SdpMedia::SdpMedia(int type) :
	_media_type((MediaType) type), _codec_list(0), _port(0), _stream_type(
			SEND_RECEIVE) {
}

SdpMedia::SdpMedia(std::string type, int port, std::string dir) :
	_media_type((MediaType) -1), _codec_list(0), _port(port), _stream_type(
			(StreamDirection) -1) {
	unsigned int i;
	const char* tmp;

	for (i = 0; i < MEDIA_COUNT; i++) {
		tmp = mediaTypeStr[i];

		if (strcmp(type.c_str(), tmp) == 0) {
			_media_type = (MediaType) i;
			break;
		}
	}

	if (strcmp(dir.c_str(), "default") == 0)
		dir = DEFAULT_STREAM_DIRECTION;

	for (i = 0; i < DIR_COUNT; i++) {
		tmp = streamDirectionStr[i];

		if (strcmp(dir.c_str(), tmp) == 0) {
			_stream_type = (StreamDirection) i;
			break;
		}
	}
}

SdpMedia::~SdpMedia() {
	clearCodecList();
}

std::string SdpMedia::getMediaTypeStr(void) {
	std::string value;
	// Test the range to be sure we know the media
	if (_media_type >= 0 && _media_type < MEDIA_COUNT)
		value = mediaTypeStr[_media_type];
	else
		value = "unknown";

	return value;
}

void SdpMedia::addCodec(sfl::Codec* codec) {
	_codec_list.push_back(codec);
}

void SdpMedia::removeCodec(std::string codecName) {
	// Look for the codec by its encoding name
	int i;
	int size;
	std::string enc_name;
	std::vector<sfl::Codec*>::iterator iter;

	size = _codec_list.size();
	std::cout << "vector size: " << size << std::endl;

	for (i = 0; i < size; i++) {
		std::cout << _codec_list[i]->getMimeSubtype().c_str() << std::endl;

		if (strcmp(_codec_list[i]->getMimeSubtype().c_str(), codecName.c_str())
				== 0) {
			std::cout << "erase " << _codec_list[i]->getMimeSubtype()
					<< std::endl;
			iter = _codec_list.begin() + i;
			_codec_list.erase(iter);
			break;
		}
	}
}

void SdpMedia::clearCodecList(void) {
	// Erase every codecs from the list
	_codec_list.clear();
}

std::string SdpMedia::getStreamDirectionStr(void) {
	std::string value;

	// Test the range of the value

	if (_stream_type >= 0 && _stream_type < DIR_COUNT)
		value = streamDirectionStr[_stream_type];
	else
		value = "unknown";

	return value;
}

std::string SdpMedia::toString(void) {
	std::ostringstream display;
	int size, i;

	size = _codec_list.size();

	display << getMediaTypeStr();
	display << ":" << getPort();
	display << ":";

	for (i = 0; i < size; i++) {
		display << _codec_list[i]->getMimeSubtype() << "/";
	}

	display << ":" << getStreamDirectionStr() << std::endl;

	return display.str();
}

