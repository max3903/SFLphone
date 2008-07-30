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

#ifndef __Voicemail_H_
#define __Voicemail_H_ 1

#include <string>
#include <vector>
#include <string>
#include "VoicemailSound.h"

class Voicemail {
	
	private :
		int          _id;
		std::string  _callerchan;
		std::string  _callerid;
		std::string  _category;
		std::string  _context;
		int          _duration;
		std::string  _exten;
		std::string  _macrocontext;
		std::string  _name;
		std::string  _origdate;
		int          _origmailbox;
		int          _origtime;
		int          _priority;
		
		std::vector<VoicemailSound *> _lst_sounds;

	public :
		/** Cstor / Dstor */
		Voicemail();
		~Voicemail();
		
		/** Getters / Setters */
		std::string getIdString();
		void        setId(int);
		void        setCallerchan(const std::string&);
		std::string getCallerid();
		void        setCallerid(const std::string&);
		void        setCategory(const std::string&);
		void        setContext(const std::string&);
		std::string getDurationString();
		void        setDuration(int);
		void        setExten(const std::string&);
		void        setMacrocontext(const std::string&);
		void        setName(const std::string&);
		std::string getName();
		std::string getOrigdate();
		void        setOrigdate(const std::string&);
		std::string getOrigmailboxString();
		void        setOrigmailbox(int);
		std::string getOrigtimeString();
		void        setOrigtime(int);
		std::string getPriorityString();
		void        setPriority(int);
		
		/** Getting voicemail's sound files */
		VoicemailSound * getVMSoundAt(int);
		VoicemailSound * getVMSoundByFormat(const std::string&);
		
		/** ToString */
		std::string toShortString();
		std::string toString();

};

#endif
