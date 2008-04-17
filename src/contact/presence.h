/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
 *  Author: Guillaume Carmel-Archambault <guillaume.carmel-archambault@savoirfairelinux.com>
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

#ifndef PRESENCE_H
#define PRESENCE_H

#include <string>

/// Presence information for a contact entry
/**
 * Presence information on the entry if supported by the account
 * Can be an active, passive or special status defined in presencestatus.h
 * Additional information attribute still unused could be used to pass extensive
 * information on the current entry status, as for example ringing from 140
 * Initially when contacts are loaded presence will remain null
 * until an event is received on VoIP link or changed via the GUI
 */
class Presence {
	
public:
	Presence();
	Presence(std::string state, std::string additionalInfo);
	virtual ~Presence();
	
	std::string getState() { return _state; }
	std::string getAdditionalInfo() { return _additionalInfo; }
	
	void setState(std::string state) { _state = state; }
	void setAdditionalInfo(std::string additionalInfo) { _additionalInfo = additionalInfo; }
		
protected:
	
private:
	std::string _state;				/// Active, passive or special status defined in presencestatus.h 
	std::string _additionalInfo;	/// Additional information on the status, for example the person on the phone with
};

#endif
