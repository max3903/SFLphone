/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Florian DESPORTES <florian.desportes@savoirfairelinux.com>
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
#include "base64.h"


VoicemailSound::VoicemailSound() {
}

VoicemailSound::~VoicemailSound() {
//	std::cout << "~VoicemailSound" << std::endl;
}

void VoicemailSound::setFormat(std::string form) {
	_format = form;
}

void VoicemailSound::setFile(std::string fil) {
	_file = fil;
}

void VoicemailSound::setFolder(std::string fol) {
	_folder = fol;
}

std::string VoicemailSound::getFormat() {
	return _format;
}

void VoicemailSound::setDatas( std::string dat ) {
	_datas = dat;
}


std::string VoicemailSound::toDecodeString() {
	std::string decoded = Base64::decode( _datas );
	std::ofstream file;

	std::string filename = "/tmp/"+ _file +"."+ _format;
	cout << "created : " << filename << endl;
	file.open( filename.c_str() , fstream::out );
	file << decoded;
	file.close();

	return decoded;
}


std::string VoicemailSound::toString() {
	std::string res("   [ VOICEMAILSOUND ]");
	res.append("\n'-format : ");
	res.append( _format );
	res.append("\n'-folder : ");
	res.append( _folder );
	res.append("\n'-file   : ");
	res.append( _file );
//	res.append("\n'-datas (length) : ");
//	res.append( _datas.size() );
	std::cout << res << std::endl;
	return res;
}

