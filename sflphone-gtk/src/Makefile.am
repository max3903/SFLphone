SUBDIRS=po

bin_PROGRAMS = sflphone-gtk

sflphone_gtk_SOURCES = \
  main.c \
  dbus.c \
  mainwindow.c \
  calllist.c \
  dialpad.c \
  menus.c \
  calltree.c \
  screen.c \
  actions.c \
  configwindow.c \
  contactlist.c \
  accountlist.c \
  accountwindow.c \
  contactwindow.c \
  callconsolewindow.c \
  marshaller.c \
  sliders.c \
  statusicon.c \
  codeclist.c \
  videocodeclist.c \
  glwidget.c \
  MemManager.c 

noinst_HEADERS =  actions.h dbus.h mainwindow.h calllist.h dialpad.h codeclist.h videocodeclist.h \
                  callmanager-glue.h configurationmanager-glue.h contactmanager-glue.h instance-glue.h \
                  menus.h calltree.h screen.h configwindow.h contactlist.h contactwindow.h callconsolewindow.c \
                  accountlist.h accountwindow.h marshaller.h sliders.h statusicon.h 

EXTRA_DIST   = marshaller.list
sflphone_gtk_LDADD = $(DEPS_LIBS)

AM_CPPFLAGS = $(DEPS_CFLAGS) \
              -DICONS_DIR=\""$(datadir)/sflphone"\" \
              -DICON_DIR=\""$(datadir)/pixmaps"\" \
	      -DCODECS_DIR=\""$(libdir)/sflphone/codecs"\"
