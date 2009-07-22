libexec_PROGRAMS = at-spi-registryd

at_spi_registryd_CFLAGS = $(DBUS_GLIB_CFLAGS)	\
			  $(GOBJ_CFLAGS)	\
			  $(ATK_CFLAGS)		\
			  $(GDK_CFLAGS)		\
			  -I$(top_srcdir)	\
			  -DATSPI_INTROSPECTION_PATH=\"$(pkgdatadir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"

at_spi_registryd_LDADD = $(DBUS_GLIB_LIBS)	\
			 $(GOBJ_CFLAGS)		\
			 $(ATK_LIBS)		\
			 $(GDK_LIBS)		\
			 $(X_LIBS)		\
			 $(SM_LIBS)		\
			 $(XTST_LIBS)		\
			 $(XEVIE_LIBS)		\
			 $(top_builddir)/droute/libdroute.la\
			 $(top_builddir)/dbind/libdbind.la

at_spi_registryd_SOURCES =	\
	de-marshaller.h         \
	de-marshaller.c         \
	de-types.h              \
	keymasks.h              \
	paths.h                 \
	registry-main.c		\
	registry.c		\
	registry.h		\
	deviceeventcontroller.c	\
	deviceeventcontroller.h	\
	reentrant-list.c	\
	reentrant-list.h	\
	ucs2keysym.c

default_sessiondir = $(sysconfdir)/xdg/autostart
default_session_in_in_files = 				\
	at-spi-registryd.desktop.in.in
default_session_in_files = $(default_session_in_in_files:.desktop.in.in=.desktop.in)
default_session_DATA = $(default_session_in_files:.desktop.in=.desktop)

%.desktop: %.desktop.in Makefile.am
	sed -e "s,\@registryddir\@,$(libexecdir)," \
	< $< > $@
%.desktop.in: %.desktop.in.in Makefile.am
	sed -e "s,\@registryddir\@,$(libexecdir)," \
	< $< > $@

CLEANFILES = 			\
	$(default_session_DATA)	\
	$(default_session_in_files)

DISTCLEANFILES =		\
	$(default_session_in_in_files)
