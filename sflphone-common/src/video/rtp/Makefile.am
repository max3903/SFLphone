include $(top_srcdir)/globals.mak

AM_CXXFLAGS = $(CCRTP_CFLAGS)

noinst_LTLIBRARIES = libvideortp.la

libvideortp_la_SOURCES = \
	VideoRtpSession.cpp \
	VideoRtpSessionSimple.cpp
		
noinst_HEADERS = \
	VideoRtpSession.h \
	VideoRtpSessionSimple.h
				
libvideortp_la_LDFLAGS = $(CCRTP_LIBS) $(LIBS)

