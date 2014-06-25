# We build atspi-gmain.c which is located in a different directory.
AUTOMAKE_OPTIONS = subdir-objects

noinst_LTLIBRARIES = libdbind.la

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"dbind\" \
	-I$(top_srcdir) \
	$(WARN_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GLIB_CFLAGS)

libdbind_la_SOURCES = \
        dbind.h \
	dbind.c \
        dbind-any.h \
	dbind-any.c
libdbind_la_LIBADD = $(DBUS_LIBS) $(GLIB_LIBS)

TESTS = dbtest

check_PROGRAMS = dbtest
dbtest_SOURCES = dbtest.c ../atspi/atspi-gmain.c
dbtest_LDFLAGS = libdbind.la
dbtest_LDADD = $(DBUS_LIBS)

-include $(top_srcdir)/git.mk
