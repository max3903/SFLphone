/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Julien Bonjean <julien.bonjean@savoirfairelinux.com>
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
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <syslog.h>
#include <stdarg.h>
#include <string>
#include <stdio.h>

#define _error(...)	Logger::getInstance()->log(LOG_ERR, __VA_ARGS__)
#define _warn(...)	Logger::getInstance()->log(LOG_WARNING, __VA_ARGS__)
#define _info(...)	Logger::getInstance()->log(LOG_INFO, __VA_ARGS__)
#define _debug(...)	Logger::getInstance()->log(LOG_DEBUG, __VA_ARGS__)

#define _debugException(...) Logger::getInstance()->log(LOG_DEBUG, __VA_ARGS__)
#define _debugInit(...)		 Logger::getInstance()->log(LOG_DEBUG, __VA_ARGS__)
#define _debugAlsa(...)		 Logger::getInstance()->log(LOG_DEBUG, __VA_ARGS__)

#define BLACK "\033[22;30m"
#define RED "\033[22;31m"
#define GREEN "\033[22;32m"
#define BROWN "\033[22;33m"
#define BLUE "\033[22;34m"
#define MAGENTA "\033[22;35m"
#define CYAN "\033[22;36m"
#define GREY "\033[22;37m"
#define DARK_GREY "\033[01;30m"
#define LIGHT_RED "\033[01;31m"
#define LIGHT_SCREEN "\033[01;32m"
#define YELLOW "\033[01;33m"
#define LIGHT_BLUE "\033[01;34m"
#define LIGHT_MAGENTA "\033[01;35m"
#define LIGHT_CYAN "\033[01;36m"
#define WHITE "\033[01;37m"
#define END_COLOR "\033[0m"

class Logger {
public:
	virtual inline ~Logger() {};

	/**
	 * Log some message under the given severity mode.
	 * @param level The serverity level.
	 * @param format The format string.
	 * @param ... The variable argument list.
	 */
	void log (const int level, const char* format, ...){
	    if (!debugMode && level == LOG_DEBUG)
	        return;

	    va_list ap;
	    std::string prefix = "<> ";
	    char buffer[4096];
	    std::string message = "";
	    std::string color_prefix = "";

	    switch (level) {
	        case LOG_ERR: {
	            prefix = "<error> ";
	            color_prefix = RED;
	            break;
	        }
	        case LOG_WARNING: {
	            prefix = "<warning> ";
	            color_prefix = YELLOW;
	            break;
	        }
	        case LOG_INFO: {
	            prefix = "<info> ";
	            color_prefix = GREEN;
	            break;
	        }
	        case LOG_DEBUG: {
	            prefix = "<debug> ";
	            color_prefix = "";
	            break;
	        }
	    }

	    va_start (ap, format);
	    vsprintf (buffer, format, ap);
	    va_end (ap);

	    message = buffer;
	    message = prefix + message;

	    syslog (level, "%s", message.c_str());

	    if (consoleLog) {
	        message = color_prefix + message + END_COLOR + "\n";
	        fprintf (stderr, "%s", message.c_str());
	    }
	}

	/**
	 * @param c Will log to console if set to true.
	 */
	void setConsoleLog (bool c)
	{
	    consoleLog = c;
	}

	/**
	 * @param d Will print debug message if set to true.
	 */
	void setDebugMode (bool d)
	{
	    debugMode = d;
	}

	/**
	 * @return An instance of the Logger.
	 */
	static Logger* getInstance() {
		if (instance == NULL) {
			instance = new Logger();
		}

		return instance;
	}

protected:
	Logger() : consoleLog(false), debugMode(false) {}

private:
	bool consoleLog;
	bool debugMode;

	static Logger* instance;
};

#endif

