include $(top_srcdir)/globals.mak

noinst_LTLIBRARIES = libutil.la

libutil_la_SOURCES = \
		SharedMemoryPosix.cpp \
		SemaphorePosix.cpp \
		FileDescriptorPasser.cpp

noinst_HEADERS = \
		SharedMemoryPosix.h \
		SemaphorePosix.h \
		FileDescriptorPasser.h \
		QueuedBuffer.h

libutil_la_LDFLAGS = $(CCGNU2_LIBS)