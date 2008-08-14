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

#ifndef __VoicemailSound_H_
#define __VoicemailSound_H_ 1

#include <string>

class VoicemailSound {
	
	private :
		/** The format (file extension) of the voicemail */
		std::string _format;
		/** The name of the folder where the voicemail is stored */
		std::string _folder;
		/** The name of the associated voicemail message */
		std::string _file;
		/** The data from the audio file */
		std::string _data;
	
	public :
		/** Cstor / Dstor */
		VoicemailSound();
		~VoicemailSound();
		
		/** Inline Getters / Setters */
		inline std::string getFormat() { return _format; };
		inline void        setFormat(const std::string& form) { _format = form; };
		
		inline std::string getFolder() { return _folder; };
		inline void        setFolder(const std::string& fol) { _folder = fol; };
		
		inline std::string getFile() {return _file; };
		inline void        setFile(const std::string& fil) { _file = fil; };
		
		inline void        setData(const std::string& dat) { _data = dat; };
		
		/** Dealing with data which represent the audio file */
		std::string decode();
		std::string toString();

};

#endif
