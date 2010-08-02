include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideosource.la

libvideosource_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideosource_la_SOURCES = \
	VideoInputSource.cpp \
	NullVideoInputSource.h \
	VideoInputSourceGst.cpp  \
	VideoDevice.cpp \
	GstVideoDevice.cpp \
	VideoFrame.cpp  \
	VideoFormat.cpp
	
noinst_HEADERS = \
	VideoInputSource.h \
	NullVideoInputSource.h \
	VideoInputSourceGst.h \
	VideoDevice.h \
	GstVideoDevice.h \
	VideoFrame.h  \
	VideoFormat.h
