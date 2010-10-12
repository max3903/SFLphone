include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(CCRTP_CFLAGS)

noinst_LTLIBRARIES = libvideortp.la

libvideortp_la_SOURCES = \
	VideoRtpSessionSimple.cpp
		
noinst_HEADERS = \
	VideoRtpSessionSimple.h
				
libvideortp_la_LDFLAGS = $(CCRTP_LIBS) $(LIBS)

