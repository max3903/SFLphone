include $(top_srcdir)/globals.mak

SUBDIRS = source codec rtp

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideo.la

libvideo_la_SOURCES = \
	VideoEndpoint.cpp
	
libvideo_la_LIBADD = \
	source/libvideosource.la \
	codec/libvideocodec.la \
	rtp/libvideortp.la

libvideo_la_LDFLAGS = $(GSTREAMER_LIBS) $(LIBS)

noinst_HEADERS = \
	VideoEndpoint.h \
	VideoPlugin.h \
	VideoExceptions.h
	