NULL=

noinst_PROGRAMS = at app simple-at

at_SOURCES = at.c

app_SOURCES = app.c

simple_at_SOURCES = simple-at.c 

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
           -I$(top_srcdir)/libspi    \
           -I$(top_builddir)/libspi  \
	   -I$(top_srcdir)/cspi	     \
	   -I$(top_builddir)/cspi    \
	   $(TESTS_CFLAGS)

DEBUG_CFLAGS=-DSPI_DEBUG

CFLAGS += $(TESTS_CFLAGS) $(DEBUG_CFLAGS)

LDADD = ../util/libat-util.la ../libspi/libspi.la ../cspi/libcspi.la $(TESTS_LIBS)



