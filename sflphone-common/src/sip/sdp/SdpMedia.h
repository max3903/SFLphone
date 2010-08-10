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
#ifndef __SDP_MEDIA_H__
#define __SDP_MEDIA_H__

#include <vector>
#include <set>

#include "CodecFactory.h"

#define DEFAULT_STREAM_DIRECTION    "sendrecv"

/*
 * @file sdpmedia.h
 * @brief   A class to describe a media. It can be either a video codec or an audio codec.
 *          it maintains internally a list of codecs to use in the SDP session and negociation
 */

enum MimeType {
	MIME_TYPE_AUDIO, MIME_TYPE_VIDEO, MIME_TYPE_UNKNOWN
};

/*
 * This enum contains the different media types.
 * To be added in the SDP attributes
 * The last one is only here to have to size information, otherwise the enum struct doesn't provide any means to know it
 */
enum MediaType {
	AUDIOMEDIA, VIDEO, APPLICATION, TEXT, IMAGE, MESSAGE, MEDIA_COUNT
};

/*
 * This enum contains the different media stream direction.
 * To be added in the SDP attributes
 * The last one is only here to have to size information, otherwise the enum struct doesn't provide any means to know it
 */
enum StreamDirection {
	SEND_RECEIVE, SEND_ONLY, RECEIVE_ONLY, INACTIVE, DIR_COUNT
};

typedef enum StreamDirection streamDirection;
typedef enum MediaType mediaType;

#include "audio/codecs/AudioCodec.h"

class SdpMedia {
public:
	SdpMedia(int type);

	SdpMedia(std::string type, int port, std::string dir =
			DEFAULT_STREAM_DIRECTION);

	~SdpMedia();

	/**
	 * @return the type of media
	 */
	MediaType getMediaType();

	/**
	 *@return the type of media
	 */
	std::string getMediaTypeStr() const;

	/**
	 * @param type The given media type to use.
	 */
	void setMediaType(int type);

	/**
	 * @return the transport port
	 */
	int getPort() const;

	/**
	 * @param port The transport port.
	 */
	void setPort(int port);

	/**
	 * Add a codec in the current media codecs vector
	 * @param codec The codec to be added.
	 */
	void addCodec(const sfl::Codec *codec);

	/**
	 * @param pt The payload type for the codec to look for.
	 * @return The codec uniquely identified by the given payload type.
	 */
	const sfl::Codec* getCodec(ost::PayloadType pt);

	/**
	 * @return the list of codecs
	 */
	std::vector<const sfl::Codec*> getMediaCodecList();

	/**
	 * Remove all the codecs from the list
	 */
	void clearCodecList();

	/**
	 * Set the stream direction of the current media
	 * @param direction The  stream direction. Eg. : sendrecv, sendonly ...
	 */
	void setStreamDirection(int direction);

	/**
	 * @return The stream direction of the current media
	 */
	StreamDirection getStreamDirection();

	/**
	 * @return the stream direction string description of the current media.
	 */
	std::string getStreamDirectionStr();

	/**
	 * @return a string description of the current media
	 */
	std::string toString();
private:

	/* The type of media */
	MediaType _media_type;

	/* The media codec vector */
	std::vector<const sfl::Codec*> _codecList;
	typedef std::vector<const sfl::Codec*>::iterator CodecListIterator;

	/* We maintain this data structure to ensure uniqueness in payload types */
	std::set<uint8> _payloadList;

	/* the transport port */
	int _port;

	/* The stream direction */
	StreamDirection _stream_type;

	/**
	 * Payload predicate to match a payload type in a codec.
	 */
	class IsSamePayload {
	public:
		IsSamePayload(ost::PayloadType pt) :
			pt(pt) {
		}
		bool operator()(const sfl::Codec*& codec) const {
			return codec->getPayloadType() == pt;
		}
	private:
		unsigned long pt;
	};

	/**
	 * Decorator object used in case where two codecs have the same payload types.
	 */
	class CodecPayloadDecorator: public sfl::Codec {
	public:
		CodecPayloadDecorator(const sfl::Codec* decoratedObject, uint8 payload) {
			wrappee = const_cast<sfl::Codec*> (decoratedObject);
			this->payload = payload;
		}

		/**
		 * @Override
		 */
		double getBitRate() const {
			return wrappee->getBitRate();
		}

		/**
		 * @Override
		 */
		double getBandwidth() const {
			return wrappee->getBandwidth();
		}

		/**
		 * @Override
		 */
		std::string getDescription() const {
			return wrappee->getDescription();
		}

		/**
		 * @Override
		 */
		Codec* clone() const {
			return new CodecPayloadDecorator(*this);
		}

		/**
		 * @Override
		 */
		std::string getMimeType() const { return wrappee->getMimeType(); }

		/**
		 * @Override
		 */
		std::string getMimeSubtype() const { return wrappee->getMimeSubtype(); }

		/**
		 * @Override
		 */
		uint8 getPayloadType() const { return payload; }

		/**
		 * @Override
		 */
		uint32 getClockRate() const { return wrappee->getClockRate(); }

		/**
		 * @Override
		 */
		void setParameter (const std::string& name, const std::string& value) {
			wrappee->setParameter(name, value);
		}

		/**
		 * @Override
		 */
		std::string getParameter (const std::string& name) { return wrappee->getParameter(name); }

	private:
		sfl::Codec* wrappee;
		uint8 payload;
	};

};

#endif // _SDP_MEDIA
