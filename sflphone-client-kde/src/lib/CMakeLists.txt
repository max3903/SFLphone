CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_DEFINITIONS(
	${KDE4_DEFINITIONS} 
	${QT_DEFINITIONS} 
	-fexceptions
)

PROJECT(sflphonekde)

#target_link_libraries(sflphonekde ${QT_QTCORE_LIBRARY} ${KDE4_KDEUI_LIBS})

SET(LOCAL_CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
SET(CMAKE_MODULE_PATH "${LOCAL_CMAKE_MODULE_PATH}")

FIND_PACKAGE ( KDE4 REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )

INCLUDE ( KDE4Defaults )

set(GENERIC_LIB_VERSION "0.9.8")

INCLUDE_DIRECTORIES ( ${KDE4_INCLUDES} ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})



#File to compile
set( sflphonekde_LIB_SRCS
  Account.cpp
  AccountList.cpp
  Call.cpp
  CallModel.cpp
  CallTreeItem.cpp
  Contact.cpp
  ContactItemWidget.cpp
  Item.cpp
  configurationmanager_interface_singleton.cpp
  callmanager_interface_singleton.cpp
  instance_interface_singleton.cpp
  sflphone_const.h
)

# Build dbus interfaces
SET ( dbus_xml_introspecs_path ${CMAKE_CURRENT_SOURCE_DIR}/dbus/)

# configuration manager interface
SET ( configurationmanager_xml  ${dbus_xml_introspecs_path}/configurationmanager-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${configurationmanager_xml}
   PROPERTIES 
   CLASSNAME ConfigurationManagerInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h") 

QT4_ADD_DBUS_INTERFACE(
   sflphonekde_LIB_SRCS
   ${configurationmanager_xml}
   configurationmanager_dbus_interface)

# call manager interface
SET ( callmanager_xml  ${dbus_xml_introspecs_path}/callmanager-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${callmanager_xml}
   PROPERTIES 
   CLASSNAME CallManagerInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h") 

QT4_ADD_DBUS_INTERFACE(
   sflphonekde_LIB_SRCS
   ${callmanager_xml}
   callmanager_dbus_interface)

# instance interface
SET ( instance_xml  ${dbus_xml_introspecs_path}/instance-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${instance_xml}
   PROPERTIES 
   CLASSNAME InstanceInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h") 
   
QT4_ADD_DBUS_INTERFACE(
   sflphonekde_LIB_SRCS
   ${instance_xml}
   instance_dbus_interface)
 
kde4_add_library( sflphonekde  SHARED ${sflphonekde_LIB_SRCS} )
 
target_link_libraries( sflphonekde
  ${KDE4_KDECORE_LIBS}
  ${QT_QTCORE_LIBRARY}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KABC_LIBS}
)

set_target_properties( sflphonekde
  PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION}
)

set( sflphonekde_LIB_HDRS
  Account.h
  AccountList.h
  Call.h  
  CallModel.h  
  CallTreeItem.h  
  Contact.h  
  ContactItemWidget.h  
  Item.h  
  libsflphonekde_export.h
  configurationmanager_interface_singleton.h
  callmanager_interface_singleton.h
  instance_interface_singleton.h
  sflphone_const.h
)

install( FILES ${sflphonekde_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/sflphonekde
  COMPONENT Devel
)
 
install( TARGETS sflphonekde  ${INSTALL_TARGETS_DEFAULT_ARGS} )
