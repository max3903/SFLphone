/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
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

#include "iaxcall.h"
#include "manager.h"
#include "global.h" // for _debug

IAXCall::IAXCall (const CallId& id, Call::CallType type) : Call (id, type), _session (NULL)
{
	mimeTypeToAstMacro["PCMU"] = AST_FORMAT_ULAW;
	mimeTypeToAstMacro["GSM"] = AST_FORMAT_GSM;
	mimeTypeToAstMacro["PCMA"] = AST_FORMAT_ALAW;
	mimeTypeToAstMacro["ILBC"] = AST_FORMAT_ILBC;
	mimeTypeToAstMacro["SPEEX"] = AST_FORMAT_SPEEX;

	astMacroToMimeType[AST_FORMAT_ULAW] = "PCMU";
	astMacroToMimeType[AST_FORMAT_GSM] = "GSM";
	astMacroToMimeType[AST_FORMAT_ALAW] = "PCMA";
	astMacroToMimeType[AST_FORMAT_ILBC] = "ILBC";
	astMacroToMimeType[AST_FORMAT_SPEEX] = "SPEEX";
}

IAXCall::~IAXCall()
{
	_session = NULL; // just to be sure to don't have unknown pointer, do not delete it!
}

	void
IAXCall::setFormat (int format)
{
	AstMacroToMimeTypeIterator it = astMacroToMimeType.find(format);
	if (it == astMacroToMimeType.end()) {
		_error("Failed to set set asterisk format %d in IAX call", format);
		return;
	}

	setAudioCodec((*it).second);
	_asteriskFormat = format;
}

	int
IAXCall::getSupportedFormat (std::string accountID)
{
	_info ("IAX get supported format: ");

	Account* account = Manager::instance().getAccount(accountID);
	CodecOrder codecsIdentifier;
	if (account != NULL) {
		codecsIdentifier = account->getActiveAudioCodecs();
	}
	else {
		_error ("No IAx account could be found");
		return 0;
	}

	CodecFactory& factory = CodecFactory::getInstance();
	CodecOrder::const_iterator it;

	int format = 0;
	for (it = codecsIdentifier.begin(); it != codecsIdentifier.end(); it++) {
		const sfl::Codec* codec = factory.getCodec(*it);

		MimeTypeToAstMacroIterator it = mimeTypeToAstMacro.find(codec->getMimeSubtype());
		if (it != mimeTypeToAstMacro.end()) {
			format |= (*it).second;
		}
	}

	return format;
}

int IAXCall::getFirstMatchingFormat (int needles, std::string accountID) {
	_debug ("IAX get first matching codec: ");

	Account* account = Manager::instance().getAccount (accountID);
	CodecOrder activeCodecs;
	if (account != NULL) {
		activeCodecs = account->getActiveAudioCodecs();
	} else {
		_error ("No IAX account could be found");
	}

	CodecFactory& codecFactory = CodecFactory::getInstance();
	CodecOrder::const_iterator it;
	for (it = activeCodecs.begin(); it != activeCodecs.end(); it++) {
		const sfl::Codec* codec = codecFactory.getCodec(*it);
		MimeTypeToAstMacroIterator it = mimeTypeToAstMacro.find(codec->getMimeSubtype());
		if (it != mimeTypeToAstMacro.end()) {
			if ((*it).second & needles) {
				return (*it).second;
			}
		}
	}

	return 0;
}

void IAXCall::setAudioCodec(const std::string& subtype) {
	CodecFactory& factory = CodecFactory::getInstance();
	const sfl::Codec* codec = factory.getCodecByMimeSubtype(subtype);
	_audioCodec = (static_cast<const AudioCodec*>(codec))->clone();
}

AudioCodec* IAXCall::getAudioCodec()
{
	return _audioCodec;
}



