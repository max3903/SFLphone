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

#include <algorithm>

static const char* streamDirectionStr[] = { "sendrecv", "sendonly", "recvonly",
        "inactive"
                                          };

static const char* mediaTypeStr[] = { "audio", "video", "application", "text",
                                      "image", "message"
                                    };

SdpMedia::SdpMedia (int type) :
        _media_type ( (MediaType) type), _codecList (0), _port (0), _stream_type (
            SEND_RECEIVE)
{
}

SdpMedia::SdpMedia (std::string type, int port, std::string dir) :
        _media_type ( (MediaType) -1), _codecList (0), _port (port), _stream_type (
            (StreamDirection) -1)
{
    unsigned int i;
    const char* tmp;

    for (i = 0; i < MEDIA_COUNT; i++) {
        tmp = mediaTypeStr[i];

        if (strcmp (type.c_str(), tmp) == 0) {
            _media_type = (MediaType) i;
            break;
        }
    }

    if (strcmp (dir.c_str(), "default") == 0)
        dir = DEFAULT_STREAM_DIRECTION;

    for (i = 0; i < DIR_COUNT; i++) {
        tmp = streamDirectionStr[i];

        if (strcmp (dir.c_str(), tmp) == 0) {
            _stream_type = (StreamDirection) i;
            break;
        }
    }
}

SdpMedia::~SdpMedia()
{
    clearCodecList();
}

std::vector<const sfl::Codec*> SdpMedia::getMediaCodecList()
{
    return _codecList;
}

MediaType SdpMedia::getMediaType()
{
    return _media_type;
}

std::string SdpMedia::getMediaTypeStr (void) const
{
    std::string value;

    // Test the range to be sure we know the media
    if (_media_type >= 0 && _media_type < MEDIA_COUNT)
        value = mediaTypeStr[_media_type];
    else
        value = "unknown";

    return value;
}

void SdpMedia::setMediaType (int type)
{
    _media_type = (MediaType) type;
}

void SdpMedia::addCodec (const sfl::Codec* codec)
{

    std::pair<std::set<uint8>::iterator, bool> ret;
    ret = _payloadList.insert(codec->getPayloadType());

    if (ret.second == false) {
    	// A element with that payload was already present in the set. Find a suitable codec.
    	uint8 nonConflictingPayload = 96;
	    ret = _payloadList.insert(nonConflictingPayload);
    	while(ret.second == false) {
    		nonConflictingPayload += 1;
    		if (nonConflictingPayload == 128) {
    			return;
    		}
    	    ret = _payloadList.insert(nonConflictingPayload);
    	}

    	_codecList.push_back(new CodecPayloadDecorator(codec, nonConflictingPayload));
    } else {
        _codecList.push_back (codec);
    }
}

void SdpMedia::clearCodecList (void)
{
    // Erase every codecs from the list
    _codecList.clear();
    _payloadList.clear();
}

int SdpMedia::getPort() const
{
    return _port;
}

void SdpMedia::setPort (int port)
{
    _port = port;
}

void SdpMedia::setStreamDirection (int direction)
{
    _stream_type = (StreamDirection) direction;
}

StreamDirection SdpMedia::getStreamDirection()
{
    return _stream_type;
}

const sfl::Codec* SdpMedia::getCodec (ost::PayloadType pt)
{
    CodecListIterator it = std::find_if (_codecList.begin(), _codecList.end(), IsSamePayload (pt));

    return (*it);
}

std::string SdpMedia::getStreamDirectionStr (void)
{
    std::string value;

    // Test the range of the value
    if (_stream_type >= 0 && _stream_type < DIR_COUNT) {
        value = streamDirectionStr[_stream_type];
    } else {
        value = "unknown";
    }

    return value;
}

std::string SdpMedia::toString (void)
{
    std::ostringstream display;
    int size, i;

    size = _codecList.size();

    display << getMediaTypeStr();
    display << ":" << getPort();
    display << ":";

    for (i = 0; i < size; i++) {
        display << _codecList[i]->getMimeSubtype() << "/";
    }

    display << ":" << getStreamDirectionStr() << std::endl;

    return display.str();
}

