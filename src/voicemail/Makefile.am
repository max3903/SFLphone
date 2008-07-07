SUBDIRS = 

XMLFLAG = -lexpat

noinst_LTLIBRARIES = libvoicemail.la

libvoicemail_la_SOURCES = \
	VMViewerd.cpp \
	Voicemail.cpp VoicemailFolder.cpp VoicemailSound.cpp base64.cpp

noinst_HEADERS = \
	VMViewerd.h \
	Voicemail.h VoicemailFolder.h VoicemailSound.h base64.h

libvoicemail_la_LIBADD = $(XMLFLAG) 

