include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideosource.la

libvideosource_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideosource_la_SOURCES = \
	VideoInputSource.cpp \
	VideoInputSourceGst.cpp  \
	VideoDevice.cpp \
	GstVideoDevice.cpp
	
noinst_HEADERS = \
	VideoInputSource.h \
	VideoInputSourceGst.h \
	VideoDevice.h \
	GstVideoDevice.h
	