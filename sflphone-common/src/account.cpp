/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
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

#include "account.h"
#include "manager.h"

#include "CodecFactory.h"

Account::Account (const AccountID& accountID, std::string type) :
	_accountID (accountID)
	, _link (NULL)
	, _enabled (true)
	, _type (type)
	, _codecAudioSerialized("")
	, _codecVideoSerialized("")
	, _ringtonePath("/usr/share/sflphone/ringtones/konga.ul")
	, _ringtoneEnabled(true)
	, _displayName("")
	, _useragent("SFLphone")
	, _preferredVideoDevice("")
	, _alwaysOfferVideo(false)
{
	setRegistrationState (Unregistered);
}

Account::~Account()
{
}

void Account::loadConfig() {

  // If IAX is not supported, do not register this account	
#ifndef USE_IAX
  if (_type == "IAX")
    _enabled = false;
#endif

  loadCodecs ();
}

void Account::setRegistrationState (RegistrationState state) {

	if (state != _registrationState) {
		_debug ("Account: set registration state");
		_registrationState = state;

		// Notify the client
		Manager::instance().connectionStatusNotification();
	}
}

void Account::loadCodecs (void) {
	_debug("Serialized codecs : %s", _codecAudioSerialized.c_str());

    if(_codecAudioSerialized == "") {
		_info ("No audio codecs were configured for this account. Using the default list.");
		setActiveAudioCodecs(CodecFactory::getInstance().getDefaultAudioCodecOrder());
	} else {
		setActiveAudioCodecs (Manager::instance ().unserialize (_codecAudioSerialized));
	}

    if(_codecVideoSerialized == "") {
		_info ("No video codecs were configured for this account. Using the default list.");
		setActiveVideoCodecs(CodecFactory::getInstance().getDefaultVideoCodecOrder());
	} else {
		setActiveVideoCodecs (Manager::instance ().unserialize (_codecVideoSerialized));
	}
}

void Account::setAlwaysOfferVideo(bool policy)
{
	_alwaysOfferVideo = policy;
}

bool Account::isAlwaysOfferVideo()
{
	return _alwaysOfferVideo;
}

void Account::setPreferredVideoDevice(const std::string& device)
{
	_preferredVideoDevice = device;
}

std::string Account::getPreferredVideoDevice()
{
	return _preferredVideoDevice;
}

void Account::setPreferredVideoFormat(const sfl::VideoFormat& format)
{
	_preferredVideoFormat = format;
}

sfl::VideoFormat Account::getPreferredVideoFormat()
{
	return _preferredVideoFormat;
}

CodecOrder& Account::getActiveVideoCodecs() {
	return _codecVideoOrder;
}

CodecOrder& Account::getActiveAudioCodecs() {
	return _codecAudioOrder;
}

void Account::setActiveVideoCodecs(CodecOrder codecs) {
	_codecVideoOrder = codecs;
	_codecVideoSerialized = Manager::instance().serialize(codecs);
	_debug("Setting active video codecs : %s", _codecVideoSerialized.c_str());
}

void Account::setActiveAudioCodecs(CodecOrder codecs) {
	_codecAudioOrder = codecs;
	_codecAudioSerialized = Manager::instance().serialize(codecs);
	_debug("Setting active audio codecs : %s", _codecAudioSerialized.c_str());
}

std::string Account::getAudioCodecsSerialized() {
	return _codecAudioSerialized;
}

void Account::setAudioCodecsSerialized(const std::string& audioCodecs)
{
	_codecAudioSerialized = audioCodecs;
}

void Account::setVideoCodecsSerialized(const std::string& videoCodecs)
{
	_codecVideoSerialized = videoCodecs;
}

std::string Account::getVideoCodecsSerialized() {
	return _codecVideoSerialized;
}
