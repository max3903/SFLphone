include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideodecoder.la

libvideodecoder_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideodecoder_la_SOURCES = \
		H264GstDecoder.cpp
				
noinst_HEADERS = \
		VideoDecoder.h \
		NullDecoder.h \
		H264GstDecoder.h
		
		