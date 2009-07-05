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
			 $(XTST_LIBS)		\
			 $(XEVIE_LIBS)		\
			 $(top_builddir)/droute/libdroute.la\
			 $(top_builddir)/dbind/libdbind.la\
			 $(top_builddir)/common/libspicommon.la

at_spi_registryd_SOURCES =	\
	registry-main.c		\
	registry.c		\
	registry.h		\
	deviceeventcontroller.c	\
	deviceeventcontroller.h	\
	reentrant-list.c	\
	reentrant-list.h	\
	ucs2keysym.c
