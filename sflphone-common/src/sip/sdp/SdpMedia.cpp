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
        _mediaType ( (MediaType) type), _codecList (0), _port (0), _streamType (
            SEND_RECEIVE)
{
}

SdpMedia::SdpMedia (std::string type, int port, std::string dir) :
        _mediaType ( (MediaType) -1), _codecList (0), _port (port), _streamType (
            (StreamDirection) -1)
{
    unsigned int i;
    const char* tmp;

    for (i = 0; i < MEDIA_COUNT; i++) {
        tmp = mediaTypeStr[i];

        if (strcmp (type.c_str(), tmp) == 0) {
            _mediaType = (MediaType) i;
            break;
        }
    }

    if (strcmp (dir.c_str(), "default") == 0)
        dir = DEFAULT_STREAM_DIRECTION;

    for (i = 0; i < DIR_COUNT; i++) {
        tmp = streamDirectionStr[i];

        if (strcmp (dir.c_str(), tmp) == 0) {
            _streamType = (StreamDirection) i;
            break;
        }
    }
}

SdpMedia::~SdpMedia()
{
    clearCodecList();
}

MediaType SdpMedia::getMediaType()
{
    return _mediaType;
}

std::string SdpMedia::getMediaTypeStr (void) const
{
    std::string value;

    // Test the range to be sure we know the media
    if (_mediaType >= 0 && _mediaType < MEDIA_COUNT) {
        value = mediaTypeStr[_mediaType];
    } else {
        value = "unknown";
    }

    return value;
}

void SdpMedia::setMediaType (int type)
{
    _mediaType = (MediaType) type;
}

const sfl::Codec* SdpMedia::getCodec (ost::PayloadType pt)
{
    CodecListIterator it = std::find_if (_codecList.begin(), _codecList.end(), IsSamePayload (pt));

    return (*it);
}

const sfl::Codec* SdpMedia::operator[] (uint8 payloadType)
{
	return getCodec(payloadType);
}

std::vector<const sfl::Codec*> SdpMedia::getMediaCodecList()
{
	std::vector<const sfl::Codec*> output;
	output.assign(_codecList.begin(), _codecList.end());
    return output;
}

void SdpMedia::clearCodecList (void)
{
    // Erase every codecs from the list
	CodecListIterator it;
	for (it = _codecList.begin(); it != _codecList.end(); it++) {
		delete (*it);
	}
    _codecList.clear();
    _payloadList.clear();
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
    	// We avoid ownership issues by cloning the codec
        _codecList.push_back (codec->clone());
    }
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
    _streamType = (StreamDirection) direction;
}

StreamDirection SdpMedia::getStreamDirection()
{
    return _streamType;
}

std::string SdpMedia::getStreamDirectionStr (void)
{
    std::string value;

    // Test the range of the value
    if (_streamType >= 0 && _streamType < DIR_COUNT) {
        value = streamDirectionStr[_streamType];
    } else {
        value = "unknown";
    }

    return value;
}

void SdpMedia::setVideoFormat(const sfl::VideoFormat& format)
{
	_debug("SdpMedia: Setting video format on every video codecs ...");
	CodecListIterator it;
	for (it = _codecList.begin(); it != _codecList.end(); it++) {
		if ((*it)->getMimeType() == "video") {
			// As we will possibly modify the codec state by this operation,
			// create a new instance and replace the codec at the current position.
			_debug("SdpMedia: Setting format on codec %s", (*it)->getMimeSubtype().c_str());
			sfl::VideoCodec* videoCodec = dynamic_cast<sfl::VideoCodec*>((*it));
			videoCodec->setVideoInputFormat(format);
		}
	}
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

