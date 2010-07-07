include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideoencoder.la

libvideoencoder_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideoencoder_la_SOURCES = \
		VideoEncoder.h \
		H264GstEncoder.cpp
		
noinst_HEADERS = \
		VideoEncoder.h \
		NullEncoder.h \
		H264GstEncoder.h
		