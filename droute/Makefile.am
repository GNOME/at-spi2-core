noinst_LTLIBRARIES = libdroute.la

libdroute_la_CFLAGS = $(DBUS_GLIB_CFLAGS) \
                      -DATSPI_INTROSPECTION_PATH=\"$(pkgdatadir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"\
                      -I$(top_builddir)\
                      -I$(top_srcdir)

libdrouteinclude_HEADERS = \
		droute.h
		droute-variant.h
libdrouteincludedir=$(includedir)/droute

libdroute_la_SOURCES =\
		droute.c\
		droute.h\
		droute-variant.c\
		droute-variant.h\
		droute-pairhash.c\
		droute-pairhash.h

TESTS = droute-test

check_PROGRAMS = droute-test
droute_test_SOURCES  = droute-test.c
droute_test_CFLAGS = $(DBUS_GLIB_CFLAGS) \
		     -I$(top_builddir)\
		     -I$(top_srcdir)\
		     -DTEST_INTROSPECTION_DIRECTORY=\"$(top_srcdir)/droute\"

droute_test_LDFLAGS  = $(top_builddir)/dbind/libdbind.la\
		       libdroute.la\
		       $(DBUS_GLIB_LIBS)
