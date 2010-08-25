include ../../globals.mak

noinst_LTLIBRARIES = libwidget.la

libwidget_la_SOURCES = \
		PidginScrollBook.c \
		PidginMiniDialog.c \
		SFLVideoCairo.c \
		SFLMessageBar.c \
		SFLMainSection.c \
		SFLVideoSession.c \
		SFLVideoSessionControl.c \
		SFLVideoCairoShm.c
				
libwidget_la_LDFLAGS = @DEPS_LDFLAGS@
libwidget_la_LIDADD = ./util/libutil.la					  
libwidget_la_CFLAGS = @DEPS_CFLAGS@ 
