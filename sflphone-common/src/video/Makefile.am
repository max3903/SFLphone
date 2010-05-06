include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideo.la

libvideo_la_SOURCES = \
		VideoInputSource.cpp \
		VideoInputSourceGst.cpp 

noinst_HEADERS = \
		VideoInputSource.h \
		VideoInputSourceGst.h
		
libvideo_la_LDFLAGS = $(GSTREAMER_LIBS)
