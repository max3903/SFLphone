bin_PROGRAMS = sflphone-gtk

sflphone_gtk_SOURCES = \
  main.c \
  errors.c \
  dbus.c \
  sflnotify.c \
  druid.c \
  mainwindow.c \
  calllist.c \
  dialpad.c \
  menus.c \
  calltab.c \
  calltree.c \
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
  
noinst_HEADERS =  actions.h dbus.h sflnotify.h mainwindow.h calllist.h dialpad.h codeclist.h druid.h \
                  callmanager-glue.h errors.h sflphone_const.h configurationmanager-glue.h instance-glue.h menus.h calltab.h calltree.h configwindow.h \
                  videocodeclist.h contactmanager-glue.h contactlist.h contactwindow.h callconsolewindow.h \
                  accountlist.h accountwindow.h marshaller.h sliders.h statusicon.h

EXTRA_DIST   = marshaller.list  

sflphone_gtk_LDADD = $(DEPS_LIBS) -lnotify

AM_CPPFLAGS = $(DEPS_CFLAGS) \
              -DICONS_DIR=\""$(datadir)/sflphone"\" \
              -DICON_DIR=\""$(datadir)/pixmaps"\" \
	      -DCODECS_DIR=\""$(libdir)/sflphone/codecs"\"
