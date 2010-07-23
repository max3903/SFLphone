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
	, _ringtonePath("/usr/share/sflphone/ringtones/konga.ul")
	, _ringtoneEnabled(true)
	, _displayName("")
	, _useragent("SFLphone")
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

  loadAudioCodecs ();
}

void Account::setRegistrationState (RegistrationState state) {

	if (state != _registrationState) {
		_debug ("Account: set registration state");
		_registrationState = state;

		// Notify the client
		Manager::instance().connectionStatusNotification();
	}
}

void Account::loadAudioCodecs (void) {
	// if the user never set the codec list, use the default configuration for this account
       if(_codecAudioSerialized == "") {
		_info ("Account: use the default order");
		CodecFactory::getInstance().setDefaultOrder();
	}
	// else retrieve the one set in the user config file
	else {
		std::vector<std::string> active_list = Manager::instance ().retrieveActiveCodecs();

		setActiveAudioCodecs (Manager::instance ().unserialize (_codecAudioSerialized));
	}
}

CodecOrder& Account::getActiveVideoCodecs() {
	return _codecVideoOrder;
}

void Account::setActiveVideoCodecs(CodecOrder codecs) {
	_codecVideoOrder = codecs;
	_codecVideoSerialized = Manager::instance().serialize(codecs);
}

CodecOrder& Account::getActiveAudioCodecs() {
	return _codecVideoOrder;
}

void Account::setActiveAudioCodecs(CodecOrder codecs) {
	_codecAudioOrder = codecs;
	_codecAudioSerialized = Manager::instance().serialize (codecs);
}
