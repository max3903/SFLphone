include ../../globals.mak

noinst_LTLIBRARIES = libwidget.la

libwidget_la_SOURCES = \
		PidginScrollBook.c \
		PidginMiniDialog.c \
		VideoCairo.c \
		SFLMessageBar.c \
		SFLMainSection.c
				
libwidget_la_LDFLAGS = @DEPS_LDFLAGS@
libwidget_la_LIDADD = ./util/libutil.la					  
libwidget_la_CFLAGS = @DEPS_CFLAGS@ 
