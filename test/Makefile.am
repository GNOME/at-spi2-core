NULL=

noinst_PROGRAMS = at app

at_SOURCES = at.c

app_SOURCES = app.c

DEBUG_CFLAGS=-DSPI_DEBUG

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
           -I$(top_srcdir)/libspi    \
           -I$(top_builddir)/libspi  \
	   $(AT_COMMON_CFLAGS)          \
	   $(DEBUG_CFLAGS)

LDADD = ../libspi/libspi.la $(AT_COMMON_LIBS)
