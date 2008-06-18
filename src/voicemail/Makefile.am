SUBDIRS = 

XMLFLAG = -lexpat

noinst_LTLIBRARIES = libvoicemail.la

libvoicemail_la_SOURCES = \
	VMViewerd.cpp \
	Voicemail.cpp VoicemailFolder.cpp VoicemailSound.cpp

noinst_HEADERS = \
	VMViewerd.h \
	Voicemail.h VoicemailFolder.h VoicemailSound.h

libvoicemail_la_LIBADD = $(XMLFLAG) 

