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

// Callbacks
void DeviceManager::device_added_cb( LibHalContext *ctx, const char *udi )
{
  _debug( "Device added %s\n" , udi);
}

void device_removed_cb( LibHalContext *ctx, const char *udi )
{
  _debug( "Device removed %s\n" , udi );
}

void device_property_changed_cb( LibHalContext *ctx , const char *udi, const char *key ,  dbus_bool_t is_removed, dbus_bool_t is_added )
{
  _debug(";anvlkansvlsfjnvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
}

DeviceManager::DeviceManager( int type ) {
  DBusError error;
  _type = type;
  dbus = dbus_bus_get( DBUS_BUS_SYSTEM, NULL );
  if( dbus ) {
    halContext = libhal_ctx_new();
    dbus_error_init( &error );
    if( halContext )
    {
      libhal_ctx_set_dbus_connection(halContext , dbus);
    } 
    else
      _debug(" Error: Cannot initialize HAL connection\n");
  }
  else
      _debug(" Error: Cannot initialize DBus connection\n");
   
  if( !libhal_ctx_init( halContext , &error ) )
  {
    _debug("libhal_ctx_init failed\nMake sure the Hal daemon is running\n");
  }
 
  // Set the callbacks
  // When a new device is added
  if( !libhal_ctx_set_device_added( halContext , device_added_cb ) )  _debug("error while setting callback\n");
  // When a device is removed
  libhal_ctx_set_device_removed( halContext , device_removed_cb );
  libhal_ctx_set_device_property_modified( halContext , device_property_changed_cb );

  libhal_device_property_watch_all( halContext , &error );
}

DeviceManager::~DeviceManager(){
  libhal_ctx_shutdown( halContext, NULL );
  libhal_ctx_free( halContext );
  halContext = NULL;
}

std::vector<std::string> DeviceManager::listDevices( void ){
  switch( _type ){
    case ALSA:
      break;
    case VIDEO4LINUX:
      return list_v4l_devices();
    default:
      break;
  }
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
