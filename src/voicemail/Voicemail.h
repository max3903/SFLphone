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

#ifndef __Voicemail_H_
#define __Voicemail_H_

#include <string>

using namespace std;

class Voicemail {
	
	private :
		int     _id;
		string  _callerchan;
		int     _callerid;
		string  _category;
		string  _context;
		int     _duration;
		string  _exten;
		string  _macrocontext;
		string  _name;
		string  _origdate;
		int     _origmailbox;
		int     _origtime;
		int     _priority;

	public :
		Voicemail();
		~Voicemail();
		
		void   setId(int);
		void   setCallerchan(string);
		void   setCallerid(int);
		void   setCategory(string);
		void   setContext(string);
		void   setDuration(int);
		void   setExten();
		void   setMacrocontext(string);
		void   setName(string);
		string getName();
		void   setOrigdate();
		void   setOrigmailbox();
		void   setOrigtime();
		void   setPriority();
		
		
		void toString();

};

#endif
