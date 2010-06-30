include $(top_srcdir)/globals.mak

SUBDIRS = source decoder encoder rtp

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideo.la

libvideo_la_SOURCES = \
	VideoEndpoint.cpp \
	VideoFrame.cpp  \
	VideoFormat.cpp

libvideo_la_LIBADD = \
	source/libvideosource.la \
	decoder/libvideodecoder.la \
	encoder/libvideoencoder.la \
	rtp/libvideortp.la

libvideo_la_LDFLAGS = $(GSTREAMER_LIBS) $(LIBS)

noinst_HEADERS = \
	VideoEndpoint.h \
	VideoFormat.h \
	VideoFrame.h \
	VideoExceptions.h
	