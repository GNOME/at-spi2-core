lib_LTLIBRARIES = libat-bridge.la

libat_bridge_la_SOURCES = 	\
	bridge.c

libat_bridge_la_DEPENDENCIES =	\
	$(top_srcdir)/libspi/application.h

INCLUDES = -I $(top_srcdir) \
           -I $(top_builddir)         	\
	   -I $(top_srcdir)/libspi	\
	   -I $(top_builddir)/libspi	\
           -I $(top_builddir)/at-bridge \
           $(AT_BRIDGE_CFLAGS)

DEPS = 	\
	$(libatk)	\
	$(libbonobo-2)	\
	$(liboaf-2)	\
	$(libORBit-2)	\
	$(libat-spi)


LDFLAGS += -no-undefined

libat_bridge_la_LIBADD = $(AT_BRIDGE_LIBS) ../libspi/libspi.la

CFLAGS += $(AT_BRIDGE_CFLAGS)
