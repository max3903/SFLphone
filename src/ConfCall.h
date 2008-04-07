/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Jean Tessier <jean.tessier@polymtl.ca> 
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
 
#ifndef CONFCALL_H_
#define CONFCALL_H_

#include <string>
#include "call.h"
#include "mixer/Mixer.h"
#include "manager.h"

/** Defines the type that reprensents the ID of a confCall
 */
typedef std::string ConfID;

/** Container class that links both calls to a conference it creates a new local Mixer and 
 * changes the states of the calls in the conferences. Normally local mixers are shut down 
 * in both calls dans a new local mixer replaces them. When the conference if over the local 
 * mixers in the call are started back up. 
 */
class ConfCall
{
public:

	/** Constructor
	 * 
	 * Initiates the conference.
	 * \param call1 One of the calls in the conference
	 * \param call2 The other call in the conference
	 */
	ConfCall( Call* call1, Call* call2 );
	
	/** Destructor
	 */
	virtual ~ConfCall();
		
	/** Access method to the first Call in the conference
	 * \return A pointer to the first Call
	 */		
	Call* getCall1();
	
	/** Access method to the second Call in the conference
	 * \return A pointer to the second Call
	 */
	Call* getCall2();
	
	/** Method to end the conference
	 * 
	 * Must be called before deleting ConfCall object
	 */
	void endConf();
	
	/** Method to access the ID of the conference
	 */
	ConfID getConfId() const;
		
private:

	/** Default constructor
	 * 
	 * You cannot declare a ConfCall objet without the wright information
	 */
	ConfCall();
	
	/** Method to initialize the conference
	 * 
	 * Called by the constructor
	 */
	void init();
	
	/** The first call in the conference
	 */
	Call* call_1;
	
	/** The second call in the conference
	 */
	Call* call_2;
	
	/** The local Mixer in the conference
	 */
	Mixer* localMixer;
	
	/** The local audio output
	 */
	LocalAudioOuput *audioOut;
	
	/** The local video output
	 */
	LocalVideoOuput *videoOut;
	
	/** ID of the conference
	 */
	ConfID ConfId;

};

#endif /*CONFCALL_H_*/
