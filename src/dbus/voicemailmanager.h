/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
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

//#ifdef USE_VOICEMAIL
 
#ifndef VOICEMAILMANAGER_H
#define VOICEMAILMANAGER_H

#include "voicemailmanager-glue.h"
#include <dbus-c++/dbus.h>


class VoicemailManager : 
	public org::sflphone::SFLphone::VoicemailManager,
	public DBus::IntrospectableAdaptor,
	public DBus::ObjectAdaptor
{

	public:
		VoicemailManager(DBus::Connection&);
		~VoicemailManager();

		static const char* SERVER_PATH;

	public:
		std::vector< ::DBus::String > getListFolders( void );
		int                           getFolderCount( const ::DBus::String& );
		std::vector< ::DBus::String > getListMails( const ::DBus::String& );
		std::vector< ::DBus::String > getListErrors( void );
		::DBus::String                getVoicemailInfo( const ::DBus::String& , const ::DBus::String& );
		void                          playVoicemail( const ::DBus::String& , const ::DBus::String& );
		void                          stopVoicemail();
		
		// Methods
		::DBus::Bool   isVoicemailServerEnabled();
		void           voicemailServerEnable();
		::DBus::String getVoicemailConfigAddress();
		void           setVoicemailConfigAddress( const ::DBus::String& );
		::DBus::String getVoicemailConfigPath();
		void           setVoicemailConfigPath( const ::DBus::String& );
		::DBus::Int32  getVoicemailConfigPort();
		void           setVoicemailConfigPort( const ::DBus::Int32& );
		::DBus::Bool   isVoicemailConfigHttpsEnabled();
		void           voicemailConfigHttpsEnable( const ::DBus::Bool& );

};

#endif // VOICEMAILMANAGER_H

//#endif // USE_VOICEMAIL
