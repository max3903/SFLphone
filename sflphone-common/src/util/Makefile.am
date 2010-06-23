include $(top_srcdir)/globals.mak

noinst_LTLIBRARIES = libutil.la

libutil_la_SOURCES = \
		SharedMemoryPosix.cpp \
		SemaphorePosix.cpp \
		FileDescriptorPasser.cpp \
		Pattern.cpp

noinst_HEADERS = \
		SharedMemoryPosix.h \
		SemaphorePosix.h \
		FileDescriptorPasser.h \
		BufferBuilder.h \
		Observer.h \
		Observable.h \
		AbstractObservable.h \
		Buffer.h \
		BufferBuilder.h \
		Pattern.h \
		Dimension.h

libutil_la_LDFLAGS = $(CCGNU2_LIBS)
libutil_la_CXXFLAGS = $(PCRE_LIBS)