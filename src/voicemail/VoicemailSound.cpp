/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
 *  Author: Florian Desportes <florian.desportes@savoirfairelinux.com>
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
 */

#include <iostream>
#include <fstream>
#include <string>
#include "VoicemailSound.h"


VoicemailSound::VoicemailSound() {
}

VoicemailSound::~VoicemailSound() {
}

void VoicemailSound::setFormat(const std::string& form) {
	_format = form;
}

void VoicemailSound::setFile(const std::string& fil) {
	_file = fil;
}

void VoicemailSound::setFolder(const std::string& fol) {
	_folder = fol;
}

std::string VoicemailSound::getFormat() {
	return _format;
}

void VoicemailSound::setData(const std::string& dat) {
	_data = dat;
}

std::string VoicemailSound::decode() {
	/** Code is part of Base64, here is original file header */
	/****************************************************************
	 * Base64 - a simple base64 encoder and decoder.
	 *
	 * Copyright (c) 1999, Bob Withers - bwit@pobox.com
	 *
	 * This code may be freely used for any purpose, either personal
	 * or commercial, provided the authors copyright notice remains
	 * intact.
	 ****************************************************************/
	/****************************************************************
	 * file altered by : Christophe Tournayre
	 ****************************************************************/
	// Filling character
	char fillchar = '=';
	// Base64 alphabet
	std::string cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					  "abcdefghijklmnopqrstuvwxyz"
					  "0123456789+/";
	std::string::size_type i;
	char c;
	char c1;
	std::string::size_type len = _data.length();
	std::string ret;

	for( i = 0 ; i < len ; ++i ) {
		c = (char) cvt.find(_data[i]);
		++i;
		c1 = (char) cvt.find(_data[i]);
		c = (c << 2) | ((c1 >> 4) & 0x3);
		ret.append(1, c);
		if (++i < len) {
			c = _data[i];
			if (fillchar == c)
				break;
			c = (char) cvt.find(c);
			c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
			ret.append(1, c1);
		}
		if (++i < len) {
			c1 = _data[i];
			if (fillchar == c1)
				break;
			c1 = (char) cvt.find(c1);
			c = ((c << 6) & 0xc0) | c1;
			ret.append(1, c);
		}
	}
	return ret;
}

std::string VoicemailSound::toString() {
	std::string res("   [ VOICEMAILSOUND ]");
	res.append("\n'-format : ");
	res.append( _format );
	res.append("\n'-folder : ");
	res.append( _folder );
	res.append("\n'-file   : ");
	res.append( _file );
//	res.append("\n'-data (length) : ");
//	res.append( _data.size() );
	std::cout << res << std::endl;
	return res;
}

