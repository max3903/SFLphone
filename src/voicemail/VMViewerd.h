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

#ifndef __VMViewerd_H_
#define __VMViewerd_H_

#include <string>
#include <vector>
#include <expat.h>
#include "VoicemailFolder.h"
#include "VoicemailSound.h"

using namespace std;

class VMViewerd {
	
	private :
		int    _logVMail;
		int    _pwdVMail;
		string _context;

		string _srvAddr;
		string _srvPath;
		int    _srvPort;
		
		string _action;
		string _response;
		
		string _file_store;
		
		vector<VoicemailFolder *> _lst_folders;
		vector<VoicemailSound *>  _lst_sounds;
	
	public :
		VMViewerd(int logVM, int pwdVM, string ctxt, string srvAddr, string srvPath, int srvPort):
					_logVMail(logVM),
					_pwdVMail(pwdVM),
					_context(ctxt),
					_srvAddr(srvAddr),
					_srvPath(srvPath),
					_srvPort(srvPort) {
			_file_store = "/tmp/sflphone_vm";
		};
		~VMViewerd();
		
		/** Getters / Setters */
		int    getLogVMail();
		string getLogVMailString();
		void   setLogVMail(int);
		
		int    getPwdVMail();
		string getPwdVMailString();
		void   setPwdVMail(int);
		
		string getSrvAddr();
		void   setSrvAddr(string);
		
		int         getSrvPort();
		string      getSrvPortString();
		inline void setSrvPort(int p) {
			_srvPort = p;
		};
		
		string getFileStore();
		
		
		/** Maniplulation of voicemail's folders */
		vector<VoicemailFolder *> getLstFolders();
		VoicemailFolder *         getFolderAt(int);
		VoicemailFolder *         getFolderByName(string);
		void                      addVMF(VoicemailFolder *);
//		bool                      removeVMF(VoicemailFolder *);
		
		/** Manipulation of voicemail's sounds */
		vector<VoicemailSound *> getLstSounds();
		VoicemailSound *         getSoundAt(int);
		void                     addVMS(VoicemailSound *);
		
		void toString();
		
		/** */
		int exec(string);
		void parse();
/*		void startElement(void *, const XML_Char *, const XML_Char **);
		void endElement(void *, const XML_Char *);
		void character(void *, const char *, int);*/
		void extrat();
		
};

#endif
