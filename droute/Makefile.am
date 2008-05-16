noinst_LTLIBRARIES = libdroute.la

libdroute_la_CFLAGS = $(DBUS_GLIB_CFLAGS) $(ATK_CFLAGS)\
		     -DATSPI_INTROSPECTION_PATH=\"$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"\
		     -I$(top_builddir)\
		     -I$(top_srcdir)

libdroute_la_SOURCES =		\
	droute.c                \
	droute.h		\
	introspect-loader.c	\
	introspect-loader.h
