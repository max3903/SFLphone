bin_PROGRAMS = sflphone-gtk

#if GTK_CHECK_VERSION(2,10,0)  
STATUSICON_HEADER = statusicon.h
STATUSICON_SOURCE = statusicon.c
#elif
#STATUSICON_HEADER = 
#STATUSICON_SOURCE = 
#endif

sflphone_gtk_SOURCES = \
  main.c \
  errors.c \
  dbus.c \
  sflnotify.c \
  assistant.c \
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
  $(STATUSICON_SOURCE) \
  codeclist.c \
  videocodeclist.c \
  glwidget.c \
  MemManager.c \
  timestamp.c 

noinst_HEADERS =  actions.h dbus.h sflnotify.h mainwindow.h calllist.h dialpad.h codeclist.h assistant.h\
                  callmanager-glue.h errors.h sflphone_const.h configurationmanager-glue.h instance-glue.h menus.h calltab.h calltree.h configwindow.h \
                  videocodeclist.h contactmanager-glue.h contactlist.h contactwindow.h callconsolewindow.h \
                  accountlist.h accountwindow.h marshaller.h sliders.h $(STATUSICON_HEADER) timestamp.h
EXTRA_DIST   = marshaller.list  

sflphone_gtk_LDADD = $(DEPS_LIBS) -lnotify

AM_CPPFLAGS = $(DEPS_CFLAGS) \
              -DICONS_DIR=\""$(datadir)/sflphone"\" \
	      -DCODECS_DIR=\""$(libdir)/sflphone/codecs"\" 

# add symbolic link	     
install-exec-local:
	ln -sf $(bindir)/sflphone-gtk $(bindir)/sflphone 

uninstall-local:
	-test -h $(bindir)/sflphone && rm $(bindir)/sflphone
