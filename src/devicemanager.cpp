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

#include "devicemanager.h"

DeviceManager::DeviceManager() {
  dbus = dbus_bus_get( DBUS_BUS_SYSTEM, NULL );
  if( dbus ) {
    halContext = libhal_ctx_new();
    if( halContext )
      libhal_ctx_set_dbus_connection(halContext , dbus);
    else
      _debug(" Error: Cannot initialize HAL connection\n");
  }
  else
      _debug(" Error: Cannot initialize DBus connection\n");
}

DeviceManager::~DeviceManager(){
  halContext = NULL;
}

std::vector<std::string> DeviceManager::list_v4l_devices(){
  _debug( "Enumerating V4L devices ...\n" ); 
  
  int numDevices = 0;
  std::vector<std::string> list;
  char** devices = libhal_find_device_by_capability(halContext,"video4linux", &numDevices, NULL);
  for(int i= 0; i < numDevices; i++){
        char* aDevice;
        aDevice= libhal_device_get_property_string (halContext, devices[i], "video4linux.device", NULL);
        list.push_back(std::string(aDevice));
        libhal_free_string(aDevice);
  }
  for(int i= 0; i < numDevices; i++)
        delete devices[i];
  delete devices;
  return list;
}
