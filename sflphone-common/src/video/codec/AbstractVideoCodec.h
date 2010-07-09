/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
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

#ifndef __SFL_ABSTRACT_VIDEO_CODEC_H__
#define __SFL_ABSTRACT_VIDEO_CODEC_H__

#include "VideoCodec.h"
#include "video/VideoFrame.h"
#include "video/decoder/VideoDecoder.h"
#include "video/source/VideoInputSource.h"

#include <map>

namespace sfl {

template<class EncoderType, class DecoderType>
class AbstractVideoCodec: public VideoCodec {
public:
	AbstractVideoCodec() {};
	virtual ~AbstractVideoCodec() {};

	/**
	 * @Override
	 */
	void setEncoderVideoSource(VideoInputSource& source) {
		encoder->setVideoInputSource(source);
	}

	/**
	 * @Override
	 */
	void addVideoFrameEncodedObserver(VideoFrameEncodedObserver& observer) {
		encoder->addObserver(&observer);
	}

	/**
	 * @Override
	 */
	void addVideoFrameDecodedObserver(VideoFrameDecodedObserver& observer) {
		decoder->addObserver(&observer);
	}

	/**
	 * @Override
	 */
	void encode(const VideoFrame* frame) throw(VideoEncodingException) {
		encoder->encode(frame);
	}

	/**
	 * @Override
	 */
	inline void decode(ManagedBuffer<uint8>& data) throw (VideoDecodingException) {
		decoder->decode(data);
	}

	/**
	 * @Override
	 */
	void activate() {
		encoder->activate();
		decoder->activate();
	}

	/**
	 * @Override
	 */
	void deactivate() {
		encoder->deactivate();
		decoder->deactivate();
	}

	/**
	 * @Override
	 */
	void setProperty(const std::string& propName, const std::string& propValue) {
		SetterIterator it = propertyTable.find(propName);
		if (it != propertyTable.end()) {
			setProperty((*it).second, propValue);
		}
	}

protected:

	/**
	 * Called by the higher level setProperty() to dispatch to the corresponding setter method of some interface.
	 *
	 * @param index The index corresponding to this property.
	 * @param value The value to set.
	 * @precondition The implementer should have installed some property associated with the given index.
	 */
	virtual void setProperty(int index, const std::string& value) = 0;

	/**
	 * Add a mapping so that the given property name triggers some callback method associated with an index.
	 * @param propName The property
	 */
	void installProperty(const std::string propName, int index) {
		propertyTable.insert(SetterEntry(propName, index));
	}

	/**
	 * @param enc The encoder to be used.
	 */
	void setEncoder(EncoderType* enc) {
		encoder = enc;
	}

	/**
	 * @param
	 */
	void setDecoder(EncoderType* enc) {
		encoder = enc;
	}

private:

	// The following is used for mapping a property name to a setter method.
	typedef std::pair<std::string, int> SetterEntry;

	typedef std::map<std::string, int>::iterator SetterIterator;

	std::map<std::string, int> propertyTable;

	// Impose constraint on the type. Something that we can do in Java, and other languages, but not that easily in C++.
	typedef typename EncoderType::IsDerivedFromVideoEncoder
			DerivedFromEncoderGuard;

	typedef typename DecoderType::IsDerivedFromVideoDecoder
			DerivedFromDecoderGuard;

	EncoderType* encoder;

	DecoderType* decoder;

};

}

#endif
