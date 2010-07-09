include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideodecoder.la

libvideodecoder_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideodecoder_la_SOURCES = \
		H264GstDecoder.cpp \
		GstDecoder.cpp \
		GstDecoderH264.cpp
				
noinst_HEADERS = \
		VideoDecoder.h \
		NullDecoder.h \
		GstDecoder.h \
		H264GstDecoder.h \
		H264GstDecoder.h
		