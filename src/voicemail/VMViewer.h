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
		
		/** Inline Getters / Setters */
		inline string getLogVMail() { return _logVMail; };
		inline void   setLogVMail(const string& log) { _logVMail = log; };
		
		inline string getPwdVMail() { return _pwdVMail; };
		inline void   setPwdVMail(const string& pass) { _pwdVMail = pass; };
		
		inline string getContext() { return _context; };
		inline void   setContext(const string& ctxt) { _context = ctxt; };
		
		inline bool isHttpsEnabled() { return _srvUsesHttps; };
		inline void setHttpsEnabled(bool enable) { _srvUsesHttps = enable; };
		
		inline string getSrvAddr() { return _srvAddr; };
		inline void   setSrvAddr(const string& addr) { _srvAddr = addr; };
		
		inline string getSrvPath() { return _srvPath; };
		inline void   setSrvPath(const string& path) { _srvPath = path; };
		
		inline string getSrvPort() { return _srvPort; };
		inline void   setSrvPort(const string& port) { _srvPort = port; };
		
		
		/** Maniplulation of voicemail's folders */
		vector<VoicemailFolder *> getLstFolders();
		VoicemailFolder *         getFolderAt(int);
		VoicemailFolder *         getFolderByName(const string&);
		int                       getFolderCount(const string&);
		void                      addVMF(VoicemailFolder *);
//		bool                      removeVMF(VoicemailFolder *);
		
		/** Manipulation of voicemail's sounds */
		vector<VoicemailSound *> getLstSounds();
		void                     cleanSounds();
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
