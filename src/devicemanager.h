/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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

#ifndef _DEVICE_MANAGER_H
#define _DEVICE_MANAGER_H

#include <hal/libhal.h>
#include <dbus/dbus.h>

#include <string>
#include <vector>

#include "global.h"

#define VIDEO4LINUX	  "video4linux"

class DeviceManager {
  public:
    /** Constructor */
    DeviceManager();
    /** Destructor */
    ~DeviceManager();

    /** List V4L devices */
    std::vector<std::string> list_v4l_devices( void );

  private:
    LibHalContext *halContext ;
    DBusConnection *dbus;

};

#endif // _DEVICE_MANAGER_H

