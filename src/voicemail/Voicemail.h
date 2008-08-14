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
		
		/** Inline Getters / Setters */
		std::string getIdString();
		inline void setId(int id) { _id = id; };
		
		inline void setCallerchan(const std::string& chan) { _callerchan = chan; };
		
		inline std::string getCallerid() { return _callerid; };
		inline void        setCallerid(const std::string& cal) { _callerid = cal; };
		
		inline void setCategory(const std::string& cat) { _category = cat; };
		
		inline void setContext(const std::string& cont) { _context = cont; };
		
		std::string getDurationString();
		inline void setDuration(int dur) { _duration = dur; };
		
		inline std::string getExten() {return _exten; };
		inline void        setExten(const std::string& ext) { _exten = ext; };
		
		inline void setMacrocontext(const std::string& mac) { _macrocontext = mac; };
		
		inline std::string getName() { return _name; };
		inline void        setName(const std::string& nam) { _name = nam; };
		
		inline std::string getOrigdate() { return _origdate; };
		inline void        setOrigdate(const std::string& dat) { _origdate = dat; };
		
		std::string getOrigmailboxString();
		inline void setOrigmailbox(int m) { _origmailbox = m; };
		
		std::string getOrigtimeString();
		inline void setOrigtime(int t) { _origtime = t; };
		
		std::string getPriorityString();
		inline void setPriority(int p) { _priority = p; };
		
		/** Getting voicemail's sound files */
		VoicemailSound * getVMSoundAt(int);
		VoicemailSound * getVMSoundByFormat(const std::string&);
		
		/** ToString */
		std::string toShortString();
		std::string toString();

};

#endif
