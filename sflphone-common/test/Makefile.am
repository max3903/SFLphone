include ../globals.mak

check_PROGRAMS = test

test_CXXFLAGS = $(CPPUNIT_CFLAGS) $(GSTREAMER_CFLAGS)
test_LDADD = $(LLIBS) $(GSTREAMER_LIBS)

test_SOURCES = \
	main.cpp \
	validator.cpp \
	validator.h \
	accounttest.h \
	accounttest.cpp \
	audiolayertest.h \
	audiolayertest.cpp \
	configurationtest.h \
	configurationtest.cpp \
	historytest.h \
	historytest.cpp \
	hookmanagertest.h \
	hookmanagertest.cpp \
	numbercleanertest.h \
	numbercleanertest.cpp \
	pluginmanagertest.h \
	pluginmanagertest.cpp \
	rtptest.h \
	rtptest.cpp \
	sdesnegotiatortest.h \
	sdesnegotiatortest.cpp \
	VideoCaptureTest.h \
	VideoCaptureTest.cpp \
	SemaphorePosixTest.h \
	SemaphorePosixTest.cpp \
	VideoEndpointTest.h \
	VideoEndpointTest.cpp \
	H264DecoderTest.h \
	H264DecoderTest.cpp \
	H264EncoderTest.h \
	H264EncoderTest.cpp \
	GstEncoderTest.h \
	GstEncoderTest.cpp \
	GstDecoderTest.h \
	GstDecoderTest.cpp

LLIBS=$(CPPUNIT_LIBS) \
	../src/sflphoned-logger.o \
	../src/sflphoned-managerimpl.o \
	../src/sflphoned-account.o\
	../src/sflphoned-accountcreator.o \
	../src/sflphoned-call.o \
	../src/sflphoned-conference.o \
	../src/sflphoned-eventthread.o \
	../src/sflphoned-managerimpl_registration.o \
	../src/sflphoned-numbercleaner.o \
	../src/sflphoned-observer.o \
	../src/sflphoned-voiplink.o \
	../src/libsflphone.la
	