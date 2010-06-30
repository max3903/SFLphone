include $(top_srcdir)/globals.mak

noinst_LTLIBRARIES = libipcutil.la

libipcutil_la_SOURCES = \
		SharedMemoryPosix.cpp \
		SemaphorePosix.cpp
		
noinst_HEADERS = \
		SharedMemoryPosix.h \
		SemaphorePosix.h
		
libipcutil_la_CXXFLAGS = $(PCRE_LIBS)