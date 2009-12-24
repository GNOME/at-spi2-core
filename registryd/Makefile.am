libexec_PROGRAMS = at-spi2-registryd

at_spi2_registryd_CFLAGS = $(DBUS_GLIB_CFLAGS)	\
			   $(GOBJ_CFLAGS)	\
			   $(ATK_CFLAGS)	\
			   -I$(top_srcdir)	\
			   -DATSPI_INTROSPECTION_PATH=\"$(pkgdatadir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"

at_spi2_registryd_LDADD = $(DBUS_GLIB_LIBS)	\
 			  $(GOBJ_CFLAGS)	\
			  $(ATK_LIBS)		\
			  $(X_LIBS)		\
			  $(SM_LIBS)		\
			  $(XTST_LIBS)		\
			  $(XEVIE_LIBS)

at_spi2_registryd_SOURCES =	\
	de-marshaller.h         \
	de-marshaller.c         \
	de-types.h              \
	display.h		\
	display.c		\
        event-source.h          \
        event-source.c          \
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

servicedir       = $(DBUS_SERVICES_DIR)
service_in_files = org.freedesktop.atspi.Registry.service.in
service_DATA     = $(service_in_files:.service.in=.service)

$(service_DATA): $(service_in_files) Makefile
	@sed -e "s|\@LIBEXECDIR\@|$(libexecdir)|" $< > $@

DISTCLEANFILES = org.freedesktop.atspi.Registry.service
EXTRA_DIST = org.freedesktop.atspi.Registry.service.in
