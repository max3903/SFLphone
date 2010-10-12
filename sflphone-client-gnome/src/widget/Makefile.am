include ../../globals.mak

noinst_LTLIBRARIES = libwidget.la

libwidget_la_SOURCES = \
		gtkscrollbook.c \
		minidialog.c \
		imwidget.c \
		cheese-flash.c \
		cheese-flash.h \
		PidginScrollBook.c \
		PidginMiniDialog.c \
		SFLMessageBar.c \
		SFLMainSection.c \
		SFLVideoSession.c \
		SFLVideoSessionControl.c \
		SFLVideoDevicePreview.c \
		SFLVideoCairoShm.c
				
libwidget_la_LDFLAGS = @DEPS_LDFLAGS@
libwidget_la_LIDADD = ./util/libutil.la					  
libwidget_la_CFLAGS = @DEPS_CFLAGS@ 
