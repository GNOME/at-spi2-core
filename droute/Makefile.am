noinst_LTLIBRARIES = libdroute.la

libdroute_la_CFLAGS = $(DBUS_CFLAGS) \
                      $(GLIB_CFLAGS) \
                      -I$(top_builddir)\
                      -I$(top_srcdir)

libdroute_la_SOURCES =\
		droute.c\
		droute.h\
		droute-variant.c\
		droute-variant.h\
		droute-pairhash.c\
		droute-pairhash.h
libdroute_la_LIBADD = $(DBUS_LIBS)

TESTS = droute-test

check_PROGRAMS = droute-test
droute_test_SOURCES  = droute-test.c
droute_test_CFLAGS = $(DBUS_CFLAGS) \
		     -I$(top_builddir)\
		     $(GLIB_CFLAGS) \
		     $(ATSPI_CFLAGS) \
		     -I$(top_srcdir)

droute_test_LDFLAGS  = libdroute.la\
		       $(DBUS_LIBS) \
		       $(GLIB_LIBS) \
		       $(ATSPI_LIBS)
