include $(top_srcdir)/globals.mak

SUBDIRS = codec rtp source

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideo.la

libvideo_la_SOURCES = \
	VideoEndpoint.cpp \
	VideoFrame.cpp  \
	VideoFormat.cpp

libvideo_la_LIBADD = \
	codec/libvideocodec.la \
	source/libvideosource.la \
	rtp/libvideortp.la

libvideo_la_LDFLAGS = $(GSTREAMER_LIBS) $(LIBS)

noinst_HEADERS = \
	VideoEndpoint.h \
	VideoFormat.h \
	VideoFrame.h \
	VideoPlugin.h \
	VideoExceptions.h
	