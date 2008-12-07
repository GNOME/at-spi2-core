noinst_LTLIBRARIES = libdroute.la

libdroute_la_CFLAGS = $(DBUS_GLIB_CFLAGS) \
                      -DATSPI_INTROSPECTION_PATH=\"$(pkgdatadir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"\
                      -I$(top_builddir)\
                      -I$(top_srcdir)

libdrouteinclude_HEADERS = \
		droute.h
		droute-variant.h
libdrouteincludedir=$(includedir)/dbind-0.1

libdroute_la_SOURCES =\
		droute.c\
		droute.h\
		droute-variant.c\
		droute-variant.h\
		droute-pairhash.c\
		droute-pairhash.h
