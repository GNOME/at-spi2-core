libexec_PROGRAMS = at-spi2-registryd

at_spi2_registryd_CFLAGS =	\
	$(GLIB_CFLAGS)		\
	$(GIO_CFLAGS)		\
	$(DBUS_CFLAGS)	\
	$(GOBJ_CFLAGS)		\
	$(ATK_CFLAGS)		\
	-I$(top_srcdir)		\
	-DATSPI_INTROSPECTION_PATH=\"$(pkgdatadir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"

at_spi2_registryd_LDADD =	\
	../atspi/libatspi.la \
	$(GLIB_LIBS)		\
	$(GIO_LIBS)		\
	$(DBUS_LIBS)	\
	$(GOBJ_CFLAGS)		\
	$(ATK_LIBS)		\
	$(X_LIBS)		\
	$(SM_LIBS)		\
	$(XTST_LIBS)		\
	$(XEVIE_LIBS)		\
	$(DL_LIBS)

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
	introspection.h         \
	introspection.c         \
	deviceeventcontroller.c	\
	deviceeventcontroller.h	\
	reentrant-list.c	\
	reentrant-list.h	\
	ucs2keysym.c

servicedir       = $(DBUS_SERVICES_DIR)
service_in_files = org.a11y.atspi.Registry.service.in
service_DATA     = $(service_in_files:.service.in=.service)

$(service_DATA): $(service_in_files) Makefile
	sed -e "s|[@]LIBEXECDIR[@]|$(libexecdir)|" $(srcdir)/$@.in > $@


DISTCLEANFILES = org.a11y.atspi.Registry.service
EXTRA_DIST = org.a11y.atspi.Registry.service.in
