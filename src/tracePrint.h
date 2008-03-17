/*
 *  Copyright (C) 2004-2007 Savoir-Faire Linux inc.
 *  Author: Jean Tessier <jean.tessier@polymtl.ca>
 *                                                                              
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
 
#ifndef TRACEPRINT_H_
#define TRACEPRINT_H_

#include <stdio.h>
#include <stdlib.h>

#define TRACE_LEVEL		1				/*Default trace level*/

#define HEADER_ERROR	"Error: "		/*Error message Tag*/
#define HEADER_WARNING	"Warning: "		/*Warning message Tag*/
#define HEADER_INFO		"Info: "		/*Info message Tag*/
#define HEADER_FATAL	"Fatal Error: "	/*Fatal error message Tag*/

#define SOFT_HEADER		"slfphoned "	/*Name of the software*/

#define MT_INFO			0x1				/*Outputs Informattionnal message*/
#define	MT_WARNING		0x2				/*Ouputs Warning message*/
#define	MT_ERROR		0x3				/*Ouputs Error message*/
#define	MT_FATAL		0x4				/*Ouputs Fatal Error message and shuts down the program*/
#define	MT_NONE			0x5				/*No header is added, acts like a normal printf*/


/** Functor that acts like a print function
 * 
 * Used as a Trace system with level filtering. By setting the constant TRACE_LEVEL, you change the level of tracibility that you will get. Anything with a level below or equal to TRACE_LEVEL will be displayed. If type is equal to MT_FATAL the trace shows an error and quits the program returning -1.
 * */  
class tracePrint
{
public:

	/** Method to print the message
	 * @param message The message to be printed
	 * @param type The type of the message can be: MT_INFO, MT_WARNING, MT_ERROR, MT_FATAL
	 * @param level	The level of the message
	 * @param ret If you want to print a line return
	 */
	void operator()(const char* message, int type, int level, bool ret= true);
	
};

#define ptrace tracePrint() /*The instance of the functor*/

#endif /*TRACEPRINT_H_*/
