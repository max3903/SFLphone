#ifndef LIBSFLPHONEKDE_EXPORT_H
#define LIBSFLPHONEKDE_EXPORT_H
 
#include <kdemacros.h>
 
#ifndef LIBSFLPHONEKDE_EXPORT
# if defined(MAKE_LIBSFLPHONEKDE_LIB)
   // We are building this library
#  define LIBSFLPHONEKDE_EXPORT KDE_EXPORT
# else
   // We are using this library
#  define LIBSFLPHONEKDE_EXPORT KDE_IMPORT
# endif
#endif
 
# ifndef LIBSFLPHONEKDE_EXPORT_DEPRECATED
#  define LIBSFLPHONEKDE_EXPORT_DEPRECATED KDE_DEPRECATED LIBSFLPHONEKDE_EXPORT
# endif
 
#endif
