include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

noinst_LTLIBRARIES = libvideocodec.la

libvideocodec_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideocodec_la_SOURCES = \
		AbstractVideoCodec.h \
		GstCodecH264.cpp \
		VideoCodecNull.h
				
noinst_HEADERS = \
		VideoCodec.h \
		VideoCodecNull.h \
		AbstractVideoCodec.h \
		GstCodecH264.h
		