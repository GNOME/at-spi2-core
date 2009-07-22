lib_LTLIBRARIES = libdbind.la

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"dbind\" \
	-I$(top_srcdir) \
	$(WARN_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GLIB_CFLAGS)

libdbindinclude_HEADERS = \
	dbind-any.h \
	dbind.h
libdbindincludedir=$(includedir)/dbind-$(DBIND_VERSION)/dbind

libdbind_la_SOURCES = \
	dbind.c \
	dbind-any.c
libdbind_la_LIBADD = $(DBUS_LIBS) $(GLIB_LIBS)

TESTS = dbtest

check_PROGRAMS = dbtest
dbtest_SOURCES = dbtest.c
dbtest_LDFLAGS = libdbind.la
