include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(GSTREAMER_CFLAGS)

SUBDIRS = decoder encoder mime

noinst_LTLIBRARIES = libvideocodec.la

libvideocodec_la_LDFLAGS = $(GSTREAMER_LIBS)

libvideocodec_la_LIBADD = \
		decoder/libvideodecoder.la \
		encoder/libvideoencoder.la \
		mime/libvideomimeparameters.la
	
libvideocodec_la_SOURCES = \
		AbstractVideoCodec.h \
		GstCodecH264.h \
		GstCodecTheora.h \
		GstCodecJpeg.h \
		VideoCodecNull.h
				
noinst_HEADERS = \
		VideoCodec.h \
		AbstractVideoCodec.h \
		GstCodecJpeg.h \
		VideoCodecNull.h
		