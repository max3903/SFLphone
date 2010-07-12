include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideoencoder.la

libvideoencoder_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideoencoder_la_SOURCES = \
		VideoEncoder.h \
		GstEncoder.cpp \
		GstEncoderH264.cpp \
		GstEncoderTheora.cpp \
		GstEncoderJpeg.cpp \
		H264GstEncoder.cpp
		
noinst_HEADERS = \
		VideoEncoder.h \
		NullEncoder.h \
		GstEncoder.h \
		GstEncoderH264.h \
		GstEncoderTheora.h \
		GstEncoderJpeg.h \
		H264GstEncoder.h