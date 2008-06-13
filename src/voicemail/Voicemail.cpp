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
#include <string>
#include "Voicemail.h"


Voicemail::Voicemail() {
}

Voicemail::~Voicemail() {
//	std::cout << "~Voicemail" << std::endl;
}

void Voicemail::setId(int id) {
	_id = id;
}

void Voicemail::setCallerchan(std::string chan) {
	
}

void Voicemail::setCallerid(int cal) {
	_callerid = cal;
}

void Voicemail::setCategory(std::string cat) {
	_category = cat;
}

void Voicemail::setContext(std::string cont) {
	_context = cont;
}

void Voicemail::setDuration(int dur) {
	_duration = dur;
}

void Voicemail::setExten() {
}

void Voicemail::setMacrocontext(std::string mac) {
	_macrocontext = mac;
}

void Voicemail::setName(std::string nam) {
	_name = nam;
}

std::string Voicemail::getName() {
	return _name;
}

void Voicemail::setOrigdate() {
}

void Voicemail::setOrigmailbox() {
}

void Voicemail::setOrigtime() {
}

void Voicemail::setPriority() {
}

void Voicemail::toString() {
	cout << "      [ VOICEMAIL ]" << endl;
	cout << "        '-name         : " << _name << endl;
	cout << "        '-id           : " << _id << endl;
	cout << "        '-callerchan   : " << _callerchan << endl;
	cout << "        '-callerid     : " << _callerid << endl;
	cout << "        '-category     : " << _category << endl;
	cout << "        '-context      : " << _context << endl;
	cout << "        '-duration     : " << _duration << endl;
	cout << "        '-exten        : " << _exten << endl;
	cout << "        '-macrocontext : " << _macrocontext << endl;
	cout << "        '-origdate     : " << _origdate << endl;
	cout << "        '-origmailbox  : " << _origmailbox << endl;
	cout << "        '-origtime     : " << _origtime << endl;
	cout << "        '-priority     : " << _priority << endl;
}



/*
int main(int argc, char *argv[]) {
	std::cout << " -- VoicemailMain --" << std::endl;
	return 0;
}
*/
