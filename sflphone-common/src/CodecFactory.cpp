/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *  Author: Laurielle Lea <laurielle.lea@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
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

#include "CodecFactory.h"

#include <iostream>

#include <cstdlib>

CodecFactory* CodecFactory::instance = 0;

CodecFactory& CodecFactory::getInstance() {
	if (instance == 0) {
		instance = new CodecFactory();
	}

	return *instance;
}

CodecFactory::CodecFactory() :
	_cache(), _codecInMemory(), _codecsMap() {
	init();
}

CodecFactory::~CodecFactory() {
	deleteHandlePointer();
}

std::vector<std::string> CodecFactory::getAllMimeSubtypes() {
	std::vector<std::string> output;
	IdentifierToCodecInstanceMapIterator it;
	for (it = _codecsMap.begin(); it != _codecsMap.end(); it++) {
		output.push_back(((*it).second)->getMimeSubtype());
	}

	return output;
}

const sfl::Codec* CodecFactory::getCodec(const std::string& id)
{
	IdentifierToCodecInstanceMap::iterator iter = _codecsMap.find(id);

	if (iter != _codecsMap.end()) {
		// _debug("Found codec %i _codecsMap from codec descriptor", payload);
		return (iter->second);
	}

	_error ("Error cannot find codec %i in Codec Factory", id.c_str());

	return NULL;
}

sfl::Codec* CodecFactory::getCodecInstance(const std::string& id)
{
	const sfl::Codec* codec = getCodec(id);
	if (codec != NULL) {
		return codec->clone();
	}

	return NULL;
}

const sfl::Codec* CodecFactory::getCodecByMimeSubtype(const std::string& mimeSubtype) {
	IdentifierToCodecInstanceMap::iterator it;
	for (it = _codecsMap.begin() ; it != _codecsMap.end(); it++) {
		if (((*it).second)->getMimeSubtype() == mimeSubtype) {
			return (*it).second;
		}
	}

	return NULL; // TODO throw something instead.
}

const sfl::Codec* CodecFactory::getCodecByPayloadType(ost::PayloadType type) {
	IdentifierToCodecInstanceMap::iterator it;
	for (it = _codecsMap.begin() ; it != _codecsMap.end(); it++) {
		if (((*it).second)->getPayloadType() == type) {
			return (*it).second;
		}
	}

	return NULL; // TODO throw something instead.
}

const sfl::Codec* CodecFactory::getCodecByPayloadFormat(const ost::PayloadFormat& format) {
	IdentifierToCodecInstanceMap::iterator it;
	for (it = _codecsMap.begin() ; it != _codecsMap.end(); it++) {
		sfl::Codec* codec = (*it).second;
		if (codec->getPayloadType() == format.getPayloadType()
				&&
			codec->getClockRate() == format.getRTPClockRate()) {
			return (*it).second;
		}
	}

	return NULL; // TODO throw something instead.
}

const AudioCodec* CodecFactory::getFirstAvailableAudioCodec() {
	IdentifierToCodecInstanceMap::iterator it = _codecsMap.begin();
	for (it = _codecsMap.begin() ; it != _codecsMap.end(); it++) {
		if (((*it).second)->getMimeSubtype() == "audio") {
			const AudioCodec* codec = static_cast<AudioCodec*>((*it).second);
			return codec;
		}
	}

	return NULL;
}

std::vector<const AudioCodec*> CodecFactory::getAllAudioCodecs()
{
	std::vector<const AudioCodec*> output;

	IdentifierToCodecInstanceMap::iterator it;
	for (it = _codecsMap.begin(); it != _codecsMap.end(); it++) {
		if (((*it).second)->getMimeType() == "audio") {
			const AudioCodec* codec = static_cast<AudioCodec*>((*it).second);
			output.push_back(codec);
		}
	}

	return output;
}

CodecOrder CodecFactory::getDefaultAudioCodecOrder()
{
	CodecOrder output;

	IdentifierToCodecInstanceMap::iterator it;
	for (it = _codecsMap.begin(); it != _codecsMap.end(); it++) {
		if (((*it).second)->getMimeType() == "audio") {
			output.push_back(((*it).second)->hashCode());
		}
	}

	return output;
}

void CodecFactory::init() {
	std::vector<sfl::Codec*> codecs = scanCodecDirectory();

	std::sort(codecs.begin(), codecs.end(), CodecComparator);

	if (codecs.size() != 0) {
		std::vector<sfl::Codec*>::iterator it;
		for (it = codecs.begin(); it != codecs.end(); it++) {
			sfl::Codec* codec = *it;
			std::string id = (*it)->hashCode();
			_codecsMap.insert(std::pair<std::string, sfl::Codec*>(id, codec));
		}
	} else {
		_error("No plugin could be found.");
		// TODO Throw something.
	}
}

bool CodecFactory::isAlreadyInCache(std::string lib) {
	int i;

	for (i = 0; (unsigned int) i < _cache.size(); i++) {
		if (_cache[i] == lib) {
			return true;
		}
	}

	return false;
}

bool CodecFactory::isLibraryValid(std::string lib) {

	// The name of the shared library seems valid  <==> it looks like libcodec_xxx.so
	// We check this
	std::string begin = SFL_CODEC_VALID_PREFIX;
	std::string end = SFL_CODEC_VALID_EXTEN;

	// First : check the length of the file name.
	// If it is shorter than begin.length() + end.length() , not a SFL shared library

	if (lib.length() <= begin.length() + end.length())
		return false;

	// Second: check the extension of the file name.
	// If it is different than SFL_CODEC_VALID_EXTEN , not a SFL shared library
	if (lib.substr(lib.length() - end.length(), lib.length()) != end)
		return false;

#ifdef HAVE_SPEEX_CODEC
	// Nothing special
#else
	if (lib.substr(begin.length(), lib.length() - begin.length() - end.length())
			== SPEEX_STRING_DESCRIPTION)
		return false;

#endif

#ifdef HAVE_GSM_CODEC
	// Nothing special
#else
	if (lib.substr(begin.length(), lib.length() - begin.length() - end.length())
			== GSM_STRING_DESCRIPTION)
		return false;

#endif

#ifdef BUILD_ILBC
	// Nothing special
#else
	if (lib.substr(begin.length(), lib.length() - begin.length() - end.length())
			== ILBC_STRING_DESCRIPTION)
		return false;

#endif

	if (lib.substr(0, begin.length()) == begin)
		if (lib.substr(lib.length() - end.length(), end.length()) == end)
			return true;
		else
			return false;
	else
		return false;
}

void CodecFactory::deleteHandlePointer(void) {
	_debug("CodecDesccriptor: Delete codec handle pointers");

	for (int i = 0; (unsigned int) i < _codecInMemory.size(); i++) {
		unloadCodec(_codecInMemory[i]);
	}

	_codecInMemory.clear();
}

std::vector<sfl::Codec*> CodecFactory::scanCodecDirectory(void) {

	std::vector<sfl::Codec*> codecs;
	std::string tmp;
	int i;

	std::string libDir = std::string(CODECS_DIR).append("/");
	std::string homeDir = std::string(HOMEDIR) + DIR_SEPARATOR_STR + "."
			+ PROGDIR + "/";
	std::vector<std::string> dirToScan;
	dirToScan.push_back(homeDir);
	dirToScan.push_back(libDir);

	for (i = 0; (unsigned int) i < dirToScan.size(); i++) {
		std::string dirStr = dirToScan[i];

		_debug ("CodecDescriptor: Scanning %s to find audio codecs....", dirStr.c_str());

		DIR *dir = opendir(dirStr.c_str());
		if (dir) {
			dirent *dirStruct;
			while ((dirStruct = readdir(dir))) {
				tmp = dirStruct->d_name;
				if (!(tmp == CURRENT_DIR || tmp == PARENT_DIR)) {
					if (isLibraryValid(tmp) && !isAlreadyInCache(tmp)) {
						_cache.push_back(tmp);

						_debug("Library %s added to the cache.", tmp.c_str());

						sfl::Codec* audioCodec = loadCodec(dirStr.append(tmp));

						codecs.push_back(audioCodec);

						_debug("Codec %s/%s %d (%s) loaded.",
								audioCodec->getMimeType().c_str(),
								audioCodec->getMimeSubtype().c_str(),
								audioCodec->getClockRate(),
								audioCodec->getDescription().c_str());

						dirStr = dirToScan[i];
					}
				}
			}
		}

		closedir(dir);
	}

	return codecs;
}

sfl::Codec* CodecFactory::loadCodec(std::string path) {
	CodecHandlePointer p;
	void* codecHandle = dlopen(path.c_str(), RTLD_LAZY);
	if (!codecHandle) {
		_error("%s", dlerror());
	}

	create_t* createCodec = (create_t*) dlsym(codecHandle, "create");
	if (createCodec == NULL) {
		_error("%s (%s:%d)", dlerror(), __FILE__, __LINE__);
	}

	sfl::Codec* codec = createCodec();
	_codecInMemory.push_back(CodecHandlePointer(codec, codecHandle));

	return codec;
}

void CodecFactory::unloadCodec(CodecHandlePointer p) {
	destroy_t* destroyCodec = (destroy_t*) dlsym(p.second, "destroy");
	if (destroyCodec == NULL) {
		_error("%s (%s:%d)", dlerror(), __FILE__, __LINE__);
	}

	destroyCodec(p.first);

	dlclose(p.second);
}
