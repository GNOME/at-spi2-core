NULL=

noinst_PROGRAMS = at app

at_SOURCES = at.c

app_SOURCES = app.c

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
           -I$(top_srcdir)/libspi    \
           -I$(top_builddir)/libspi  \
	   $(TESTS_CFLAGS)

DEBUG_CFLAGS=-DSPI_DEBUG

CFLAGS += $(TESTS_CFLAGS) $(DEBUG_CFLAGS)

LDADD = ../libspi/libspi.la $(TESTS_LIBS)
