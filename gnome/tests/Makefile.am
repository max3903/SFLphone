include ../globals.mak

TESTS = check_global check_contacts check_config check_dbus
check_PROGRAMS = check_global check_contacts check_config check_dbus

SFLPHONE_LIBS = $(top_builddir)/src/contacts/libcontacts.la \
				$(top_builddir)/src/dbus/libdbus.la \
				$(top_builddir)/src/config/libconfig.la

###########################################################

check_global_SOURCES =		check_global.c $(top_srcdir)/src/accountlist.c $(top_srcdir)/src/logger.c
check_global_CFLAGS =		@CHECK_CFLAGS@ @SFLGTK_CFLAGS@ @DBUSGLIB_CFLAGS@
check_global_LDADD =		$(SFLPHONE_LIBS) @CHECK_LIBS@ @LIBSEXY_LIBS@ @SFLGTK_LIBS@ @DBUSGLIB_LIBS@

###########################################################

check_contacts_SOURCES =	check_contacts.c
check_contacts_CFLAGS =		@CHECK_CFLAGS@ @SFLGTK_CFLAGS@
check_contacts_LDADD =		$(SFLPHONE_LIBS) @CHECK_LIBS@ @LIBSEXY_LIBS@

###########################################################

check_config_SOURCES =		check_config.c
check_config_CFLAGS = 		@CHECK_CFLAGS@ @SFLGTK_CFLAGS@
check_config_LDADD =		$(SFLPHONE_LIBS) @CHECK_LIBS@ @LIBSEXY_LIBS@

###########################################################

check_dbus_SOURCES =		check_dbus.c $(top_srcdir)/src/logger.c $(top_srcdir)/src/accountlist.c $(top_srcdir)/src/actions.c $(top_srcdir)/src/codeclist.c $(top_srcdir)/src/dialpad.c $(top_srcdir)/src/errors.c $(top_srcdir)/src/mainwindow.c $(top_srcdir)/src/reqaccount.c $(top_srcdir)/src/sflnotify.c $(top_srcdir)/src/sliders.c $(top_srcdir)/src/statusicon.c $(top_srcdir)/src/callable_obj.c $(top_srcdir)/src/eel-gconf-extensions.c $(top_srcdir)/src/uimanager.c $(top_srcdir)/src/conference_obj.c $(top_srcdir)/src/shortcuts.c $(top_srcdir)/src/imwindow.c

check_dbus_CFLAGS = 		@CHECK_CFLAGS@ @SFLGTK_CFLAGS@ @DBUSGLIB_CFLAGS@ @WEBKIT_CFLAGS@ @LIBGNOMEUI_CFLAGS@
check_dbus_LDADD =			$(SFLPHONE_LIBS) @CHECK_LIBS@ @LIBSEXY_LIBS@ $(top_builddir)/src/widget/libwidget.la $(top_builddir)/src/icons/libicons.la

###########################################################
