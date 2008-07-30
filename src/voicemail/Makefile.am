SUBDIRS = 

PKG_CONFIG = /usr/bin/pkg-config

XML_FLAG   = -lexpat
SOUP_LIB   = `$(PKG_CONFIG) --libs libsoup-2.4`
SOUP_INCLUDES = `$(PKG_CONFIG) --cflags libsoup-2.4`

noinst_LTLIBRARIES = libvoicemail.la

libvoicemail_la_SOURCES = \
	VMViewer.cpp \
	Voicemail.cpp VoicemailFolder.cpp VoicemailSound.cpp

noinst_HEADERS = \
	VMViewer.h \
	Voicemail.h VoicemailFolder.h VoicemailSound.h

libvoicemail_la_LIBADD = $(XML_FLAG)

libvoicemail_la_CXXFLAGS = $(SOUP_INCLUDES)

libvoicemail_la_LDFLAGS = $(SOUP_LIB)

