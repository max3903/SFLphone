SUBDIRS = VideoCodec V4L

noinst_LTLIBRARIES = libvideo.la

libvideo_la_SOURCES =  	VideoCodecDescriptor.cpp \
			VideoCodecDescriptor.cpp \
			VideoRtp.cpp \
			VideoRtpRTX.cpp

AM_CXXFLAGS = -I$(top_srcdir)/include $(libccext2_CFLAGS) $(LIBAVCODEC_CFLAGS) $(hal_CFLAGS) $(dbus_1_CFLAGS) $(ortp_CFLAGS) $(USER_INCLUDES) -I/usr/include/ortp -D_ORTP_SOURCE
libvideo_la_LIBADD= $(ortp_LIBS)