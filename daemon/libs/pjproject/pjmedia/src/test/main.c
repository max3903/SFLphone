/* $Id: main.c 2408 2009-01-01 22:08:21Z bennylp $ */
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
#include "test.h"

 
/* Any tests that want to build a linked executable for RTEMS must include
   this header to get a default config for the network stack. */
#if defined(PJ_RTEMS) 
#   include <bsp.h>
#   include <rtems.h>
#   include <rtems/rtems_bsdnet.h>
#   include "../../../pjlib/include/rtems-network-config.h"
#endif


int main(int argc, char *argv[])
{
    int rc;
    char s[10];

    rc = test_main();

    if (argc == 2 && argv[1][0]=='-' && argv[1][1]=='i') {
	puts("\nPress <ENTER> to quit");
	if (fgets(s, sizeof(s), stdin) == NULL)
	    return rc;
    }

    return rc;
}


