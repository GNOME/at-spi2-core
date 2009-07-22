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
libdbindincludedir=$(includedir)/dbind-0.1

libdbind_la_SOURCES = \
	dbind.c \
	dbind-any.c
libdbind_la_LIBADD = $(DBUS_LIBS) $(GLIB_LIBS)

TESTS = dbtest

check_PROGRAMS = dbtest
dbtest_SOURCES = dbtest.c
dbtest_LDFLAGS = libdbind.la

servicedir       = $(DBUS_SERVICES_DIR)
service_in_files = org.freedesktop.atspi.Registry.service.in
service_DATA     = $(service_in_files:.service.in=.service)

$(service_DATA): $(service_in_files) Makefile
	@sed -e "s|\@LIBEXECDIR\@|$(libexecdir)|" $< > $@
