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
		GstCodecJpeg.h \
		VideoCodecNull.h
				
noinst_HEADERS = \
		VideoCodec.h \
		AbstractVideoCodec.h \
		GstCodecJpeg.h \
		VideoCodecNull.h
	
H264_LIB = libcodec_h264.so
libcodec_h264_so_SOURCES = GstCodecH264.cpp
libcodec_h264_so_CFLAGS = -fPIC -g -Wall
libcodec_h264_so_CXXFLAGS = -fPIC -g -Wall
libcodec_h264_so_LDFLAGS = --shared -lc
libcodec_h264_so_LDADD = \
	../libvideo.la \
	libvideocodec.la

THEORA_LIB = libcodec_theora.so
libcodec_theora_so_SOURCES = GstCodecTheora.cpp
libcodec_theora_so_CFLAGS = -fPIC -g -Wall
libcodec_theora_so_CXXFLAGS = -fPIC -g -Wall
libcodec_theora_so_LDFLAGS = --shared -lc
libcodec_theora_so_LDADD = \
	../libvideo.la \
	libvideocodec.la

noinst_PROGRAMS = $(H264_LIB) $(THEORA_LIB)

install-exec-local: install-libcodec_h264_so install-libcodec_theora_so
uninstall-local: uninstall-libcodec_h264_so uninstall-libcodec_theora_so

install-libcodec_h264_so: $(H264_LIB)
	mkdir -p $(sflcodecdir)
	$(INSTALL_PROGRAM) $(H264_LIB) $(sflcodecdir)

uninstall-libcodec_h264_so:
	rm -f $(sflcodecdir)/$(H264_LIB)
	
install-libcodec_theora_so: $(THEORA_LIB)
	mkdir -p $(sflcodecdir)
	$(INSTALL_PROGRAM) $(THEORA_LIB) $(sflcodecdir)

uninstall-libcodec_theora_so:
	rm -f $(sflcodecdir)/$(THEORA_LIB)
	