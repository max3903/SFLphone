include $(top_srcdir)/globals.mak

SUBDIRS = memory pattern regex ipc gstreamer

noinst_LTLIBRARIES = libutil.la

libutil_la_SOURCES = \
		FileDescriptorPasser.cpp
		
noinst_HEADERS = \
		FileDescriptorPasser.h \
		Dimension.h
		
libutil_la_LIBADD = \
		ipc/libipcutil.la \
		regex/libregexutil.la \
		pattern/libpatternutil.la \
		memory/libmemoryutil.la \
		gstreamer/libgstreamerutil.la		

libutil_la_LDFLAGS = $(CCGNU2_LIBS)		 
