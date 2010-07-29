/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author: Laurielle Lea <laurielle.lea@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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

#ifndef __CODEC_DESCRIPTOR_H__
#define __CODEC_DESCRIPTOR_H__

#include "audio/codecs/AudioCodec.h"
#include "video/codec/VideoCodec.h"

#include "global.h"
#include "user_cfg.h"

#include <map>
#include <vector>
#include <algorithm>

#include <dirent.h>

#include <ctype.h>


/**
 * Maintains a list of the codec id (hash code) in a given order.
 */
typedef std::vector<std::string> CodecOrder;

/*
 * @file codecdescriptor.h
 * @brief Handle audio codecs, load them in memory
 */
class CodecFactory {
public:
	/**
	 * @return The unique instance of the codec factory.
	 */
	static CodecFactory& getInstance();

	/**
	 * Destructor
	 */
	virtual ~CodecFactory();

	/**
	 * @param id The unique identifier for the codec to obtain.
	 * @return A pointer on an instance of this codec.
	 */
	const sfl::Codec* getCodec(const std::string& id);

	/**
	 * @param id The unique identifier for the codec to obtain.
	 * @return A pointer on a new instance of the codec. This is a
	 * shortcut for the getCodec method, as clone() is called beforehand.
	 */
	sfl::Codec* getCodecInstance(const std::string& id);

	/**
	 * @param mimeSubtype The mimesubtype (eg: "speex") identifying this codec (possible non-uniquely).
	 * @return A pointer on an instance of this codec.
	 */
	const sfl::Codec* getCodecByMimeSubtype(const std::string& mimeSubtype);

	/**
	 * @param type The payload type (static or dynamic) to search for.
	 * @return A pointer on an instance of this codec.
	 */
	const sfl::Codec* getCodecByPayloadType(ost::PayloadType type);

	/**
	 * @param format The format to search for (payload type + clock rate)
	 * @return A pointer on an instance of this codec.
	 */
	const sfl::Codec* getCodecByPayloadFormat(const ost::PayloadFormat& format);

	/**
	 * @return a list of MIME subtype types corresponding to the available codecs.
	 */
	std::vector<std::string> getAllMimeSubtypes();

	/**
	 * @return all of the supported audio codecs.
	 */
	std::vector<const AudioCodec*> getAllAudioCodecs();

	/**
	 * @return all of the supported video codecs.
	 */
	std::vector<const sfl::VideoCodec*> getAllVideoCodecs();

	/**
	 * @return The default list of codec identifiers.
	 */
	CodecOrder getDefaultAudioCodecOrder();

	/**
	 * @return The default list of codec identifiers.
	 */
	CodecOrder getDefaultVideoCodecOrder();
protected:

	/**
	 * Constructor. Private, on purpose (singleton pattern).
	 */
	CodecFactory();

private:
	struct CodecMimeSubtypeLexicalComparator{
	  bool operator() (sfl::Codec* const& c1, sfl::Codec* const& c2) {
		  return  std::lexicographical_compare(c1->getMimeSubtype().begin(),
				  c1->getMimeSubtype().end(),
				  c2->getMimeSubtype().begin(),
				  c2->getMimeSubtype().end());
	  }
	} CodecComparator;

	/**
	 * Initialiaze the map with all the supported codecs, even those inactive
	 */
	void init();

	/**
	 * Unreferences the codecs loaded in memory
	 */
	void deleteHandlePointer(void);

	/**
	 * Scan the installation directory ( --prefix configure option )
	 * And load the dynamic library
	 * @return The list of the codec object successfully loaded in memory
	 */
	std::vector<sfl::Codec*> scanCodecDirectory(void);

	/**
	 * Load a codec
	 * @param std::string	The path of the shared ( dynamic ) library.
	 * @return AudioCodec*  the pointer of the object loaded.
	 */
	sfl::Codec* loadCodec(std::string);

	/**
	 * Enable us to keep the handle pointer on the codec dynamicaly loaded so that we could destroy
	 * when we dont need it anymore
	 * */
	typedef std::pair<sfl::Codec*, void*> CodecHandlePointer;

	/**
	 * Unload a codec
	 * @param CodecHandlePointer	The map containing the pointer on the object and the pointer on the handle function
	 */
	void unloadCodec(CodecHandlePointer);

	/**
	 * Check if the files found in searched directories seems valid
	 * @param std::string	The name of the file
	 * @return bool True if the file name begins with libcodec_ and ends with .so
	 *		false otherwise
	 */
	bool isLibraryValid(std::string);

	/**
	 * Check if the codecs shared library has already been scanned during the session
	 * Useful not to load twice the same codec saved in the different directory
	 * @param std::string	The complete name of the shared directory ( without the path )
	 * @return bool True if the codecs has been scanned
	 *	    false otherwise
	 */
	bool isAlreadyInCache(std::string);

	/**
	 * Vector containing the complete name of the codec shared library scanned
	 */
	std::vector<std::string> _cache;

	/**
	 * Vector containing pairs
	 * Pair between pointer on function handle and pointer on a Codec object
	 */
	std::vector<CodecHandlePointer> _codecInMemory;

	/** Maps a codec ID to an instance of the codec */
	typedef std::map<std::string, sfl::Codec*> IdentifierToCodecInstanceMap;
	typedef std::pair<std::string, sfl::Codec*> IdentifierToCodecEntry;
	typedef std::map<std::string, sfl::Codec*>::iterator
			IdentifierToCodecInstanceMapIterator;

	/**
	 * Map the payload of a codec and the object associated ( AudioCodec * )
	 */
	IdentifierToCodecInstanceMap _codecsMap;

	/**
	 * The unique instance of the factory
	 */
	static CodecFactory* instance;
};

#endif // __CODEC_DESCRIPTOR_H__
