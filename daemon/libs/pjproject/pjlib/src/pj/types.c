/* $Id: types.c 2394 2008-12-23 17:27:53Z bennylp $ */
/* 
 * Copyright (C) 2008-2009 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Teluu Inc. (http://www.teluu.com)
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */
#include <pj/types.h>
#include <pj/os.h>

PJ_DEF(void) pj_time_val_normalize(pj_time_val *t)
{
    PJ_CHECK_STACK();

    if (t->msec >= 1000) {
	t->sec += (t->msec / 1000);
	t->msec = (t->msec % 1000);
    }
    else if (t->msec <= -1000) {
	do {
	    t->sec--;
	    t->msec += 1000;
        } while (t->msec <= -1000);
    }

    if (t->sec >= 1 && t->msec < 0) {
	t->sec--;
	t->msec += 1000;

    } else if (t->sec < 0 && t->msec > 0) {
	t->sec++;
	t->msec -= 1000;
    }
}
