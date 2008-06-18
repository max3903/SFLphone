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

#ifndef __VoicemailFolder_H_
#define __VoicemailFolder_H_

#include <string>
#include <vector>
#include "Voicemail.h"

class VoicemailFolder {
	
	private :
		std::vector<Voicemail *> _lst_vm;
		std::string              _name;
		int                      _count;
	
	public :
		VoicemailFolder();
		~VoicemailFolder();
		
		void        setName(std::string);
		std::string getName();
		
		void        setCount(int);
		int         getCount();
		std::string getCountString();

		std::vector<Voicemail *> getLstVM();
		void                     addVM(Voicemail *);
		bool                     removeVM(Voicemail *);
		Voicemail *              getVMAt(int);
		Voicemail *              getVMByName(std::string);
		
		std::string toString();

};

#endif
