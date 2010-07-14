include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideodecoder.la

libvideodecoder_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideodecoder_la_SOURCES = \
		GstDecoder.cpp \
		GstDecoderH264.cpp \
		GstDecoderJpeg.cpp \
		GstDecoderTheora.cpp
				
noinst_HEADERS = \
		VideoDecoder.h \
		NullDecoder.h \
		GstDecoder.h \
		GstDecoderH264.h \
		GstDecoderTheora.h \
		GstDecoderJpeg.h
		