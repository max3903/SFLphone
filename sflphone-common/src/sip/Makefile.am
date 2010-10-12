include $(top_srcdir)/globals.mak

SUBDIRS = sdp

noinst_LTLIBRARIES = libsiplink.la

libsiplink_la_SOURCES = \
		sipaccount.cpp \
		sipcall.cpp \
		sipvoiplink.cpp

libsiplink_la_LIBADD = \
	sdp/libsdp.la
	
noinst_HEADERS = \
		sipaccount.h \
		sipcall.h \
		sipvoiplink.h

