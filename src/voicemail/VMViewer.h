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

#ifndef __VMViewer_H_
#define __VMViewer_H_ 1

#include <string>
#include <vector>
#include <map>
#include "Voicemail.h"
#include "VoicemailFolder.h"
#include "VoicemailSound.h"

using namespace std;


class VMViewer {
	
	private :
		string _logVMail;
		string _pwdVMail;
		string _context;

		bool   _srvUsesHttps;
		string _srvAddr;
		string _srvPath;
		string _srvPort;
		
		string _action;
		string _response;
		
		vector<VoicemailFolder *> _lst_folders;
		vector<VoicemailSound *>  _lst_sounds;
		vector<string>            _error_list;
	
	public :
		/** Cstor / Dstor */
		VMViewer(string logVM, string pwdVM, string ctxt, bool srvHttps, string srvAddr, string srvPath, string srvPort);
		~VMViewer();
		
		/** Getters / Setters */
		string getLogVMail();
		void   setLogVMail(const string&);
		
		string getPwdVMail();
		void   setPwdVMail(const string&);
		
		string getContext();
		void   setContext(const string&);
		
		bool isHttpsEnabled() { return _srvUsesHttps; };
		void setHttpsEnabled(bool);
		
		string getSrvAddr();
		void   setSrvAddr(const string&);
		
		string getSrvPath();
		void   setSrvPath(const string&);
		
		string getSrvPort();
		void   setSrvPort(string p) { _srvPort = p; };
		
		
		/** Maniplulation of voicemail's folders */
		vector<VoicemailFolder *> getLstFolders();
		VoicemailFolder *         getFolderAt(int);
		VoicemailFolder *         getFolderByName(const string&);
		int                       getFolderCount(const string&);
		void                      addVMF(VoicemailFolder *);
//		bool                      removeVMF(VoicemailFolder *);
		
		/** Manipulation of voicemail's sounds */
		vector<VoicemailSound *> getLstSounds();
		VoicemailSound *         getSoundAt(int);
		VoicemailSound *         getSoundByExt(const string&);
		void                     addVMS(VoicemailSound *);
		
		void                     addError(const string&);
		
		vector<string>       toArrayString();
		vector<string>       toFolderArrayString(const string&);
		string               getVoicemailInfo(const string&, const string&);
		map<string, string> getVoicemail(const string&, const string&);
		
		int            getErrorCount();
		vector<string> toErrorsArrayString();
		void toString();
		
		/** Dealing with agent */
		const string createRequest(const string&);
		bool         execAndParse(const string&);
		void         removeAll(void);
		
};

#endif
