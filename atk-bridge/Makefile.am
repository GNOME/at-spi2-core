moduledir = $(libdir)/gtk-2.0/modules
module_LTLIBRARIES = libatk-bridge.la

libatk_bridge_la_SOURCES = 	\
	bridge.c

libatk_bridge_la_DEPENDENCIES =	\
	$(top_srcdir)/libspi/application.h

INCLUDES = -I $(top_srcdir) \
           -I $(top_builddir)         	\
	   -I $(top_srcdir)/libspi	\
	   -I $(top_builddir)/libspi	\
           -I $(top_builddir)/atk-bridge \
           $(ATK_BRIDGE_CFLAGS)

DEPS = 	\
	$(libatk)	\
	$(libbonobo-2)	\
	$(liboaf-2)	\
	$(libORBit-2)	\
	$(libat-spi)


LDFLAGS = @LDFLAGS@ -no-undefined -module -avoid-version

libatk_bridge_la_LIBADD = $(ATK_BRIDGE_LIBS) $(top_builddir)/libspi/libspi.la

CFLAGS += $(ATK_BRIDGE_CFLAGS)
