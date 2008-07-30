/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
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

void Voicemail::setId(int id) {
	_id = id;
}

void Voicemail::setCallerchan(const string& chan) {
	_callerchan = chan;
}

string Voicemail::getCallerid() {
	return _callerid;
}

void Voicemail::setCallerid(const string& cal) {
	_callerid = cal;
}

void Voicemail::setCategory(const string& cat) {
	_category = cat;
}

void Voicemail::setContext(const string& cont) {
	_context = cont;
}

string Voicemail::getDurationString() {
	ostringstream oss;
	oss << _duration;
	return oss.str();
}

void Voicemail::setDuration(int dur) {
	_duration = dur;
}

void Voicemail::setExten(const string& ext) {
	_exten = ext;
}

void Voicemail::setMacrocontext(const string& mac) {
	_macrocontext = mac;
}

void Voicemail::setName(const string& nam) {
	_name = nam;
}

string Voicemail::getName() {
	return _name;
}

string Voicemail::getOrigdate() {
	return _origdate;
}

void Voicemail::setOrigdate(const string& dat) {
	_origdate = dat;
}

string Voicemail::getOrigmailboxString() {
	ostringstream oss;
	oss << _origmailbox;
	return oss.str();
}

void Voicemail::setOrigmailbox(int m) {
	_origmailbox = m;
}

string Voicemail::getOrigtimeString() {
	ostringstream oss;
	oss << _origtime;
	return oss.str();
}

void Voicemail::setOrigtime(int t) {
	_origtime = t;
}

string Voicemail::getPriorityString() {
	ostringstream oss;
	oss << _priority;
	return oss.str();
}

void Voicemail::setPriority(int p) {
	_priority = p;
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
	res.append(_name);
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
	res.append(_exten);
//	res.append("\nmacrocontext : ");
//	res.append(_macrocontext);
	res.append("\norigdate     : ");
	res.append(_origdate);
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

