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

#ifndef __VoicemailFolder_H_
#define __VoicemailFolder_H_ 1

#include <string>
#include <vector>
#include "Voicemail.h"

class VoicemailFolder {
	
	private :
		std::vector<Voicemail *> _lst_vm;
		std::string              _name;
		int                      _count;
	
	public :
		/** Cstor / Dstor */
		VoicemailFolder();
		~VoicemailFolder();
		
		/** Getters / Setters */
		void        setName(const std::string&);
		std::string getName();
		
		void        setCount(int);
		int         getCount();
		std::string getCountString();
		
		/** Dealing with voicemail */
		std::vector<Voicemail *> getLstVM();
		void                     addVM(Voicemail *);
		bool                     removeVM(Voicemail *);
		Voicemail *              getVMAt(int);
		Voicemail *              getVMByName(const std::string&);
		
		/** ToString */
		std::string toString();
};

#endif
