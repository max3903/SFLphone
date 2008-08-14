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
#include <sstream>
#include <string>
#include "Voicemail.h"

using namespace std;

Voicemail::Voicemail() {
}

Voicemail::~Voicemail() {
}

string Voicemail::getIdString() {
	ostringstream oss;
	oss << _id;
	return oss.str();
}

string Voicemail::getDurationString() {
	ostringstream oss;
	oss << _duration;
	return oss.str();
}

string Voicemail::getOrigmailboxString() {
	ostringstream oss;
	oss << _origmailbox;
	return oss.str();
}

string Voicemail::getOrigtimeString() {
	ostringstream oss;
	oss << _origtime;
	return oss.str();
}

string Voicemail::getPriorityString() {
	ostringstream oss;
	oss << _priority;
	return oss.str();
}

VoicemailSound * Voicemail::getVMSoundAt(int i) {
	if( i < 0 || i >= _lst_sounds.size() ) {
		return NULL;
	} else {
		return _lst_sounds[i];
	}
}

VoicemailSound * Voicemail::getVMSoundByFormat(const string& ext) {
	for( int i = 0 ; i < _lst_sounds.size() ; i++ ) {
		if( getVMSoundAt(i)->getFormat().compare(ext) == 0 ) {
			return getVMSoundAt(i);
		}
	}
	return NULL;
}

string Voicemail::toShortString() {
	string res("From <b>");
	res.append(getCallerid());
	res.append("</b>\non <i>");
	res.append(getOrigdate());
	res.append("</i>|");
	res.append(_name);
/*	res.append(" - " );
	res.append(getIdString());
	res.append(" - " );
	res.append(getDurationString());*/
//	cout << res << endl;
	return res;
}

string Voicemail::toString() {
	string res("name           : ");
	res.append(getName());
	res.append("\nid           : ");
	res.append(getIdString());
	res.append("\ncallerchan   : ");
	res.append(_callerchan);
	res.append("\ncallerid     : ");
	res.append(getCallerid());
//	res.append("\ncategory     : ");
//	res.append(_category);
//	res.append("\ncontext      : ");
//	res.append(_context);
	res.append("\nduration     : ");
	res.append(getDurationString());
	res.append("\nexten        : ");
	res.append(getExten());
//	res.append("\nmacrocontext : ");
//	res.append(_macrocontext);
	res.append("\norigdate     : ");
	res.append(getOrigdate());
//	res.append("\norigmailbox  : ");
//	res.append(getOrigmailboxString());
//	res.append("\norigtime     : ");
//	res.append(getOrigtimeString());
//	res.append("\npriority     : ");
//	res.append( getPriorityString());
//	cout << "'-[ VOICEMAIL ]" << endl;
//	cout << res << endl;
	return res;
}

