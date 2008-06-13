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

#ifndef __VoicemailSound_H_
#define __VoicemailSound_H_

#include <string>

class VoicemailSound {
	
	private :
		std::string _format;
		std::string _folder;
		std::string _file;
	
	public :
		VoicemailSound();
		~VoicemailSound();
		
		void        setFormat(std::string);
		std::string getFormat();
		void        setFolder(std::string);
		std::string getFolder();
		void        setFile(std::string);
		std::string getFile();
		
		void toString();

};

#endif
