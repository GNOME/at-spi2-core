lib_LTLIBRARIES = libat-bridge.la

libat_bridge_la_SOURCES = 	\
	bridge.c	\
	application.h

INCLUDES = -I $(top_srcdir) \
           -I $(top_builddir)         	\
	   -I $(top_srcdir)/libspi	\
	   -I $(top_builddir)/libspi	\
           -I $(top_builddir)/at-bridge \
           $(AT_COMMON_CFLAGS)

LDADD = $(AT_COMMON_LIBS) ../libspi/libspi.la
