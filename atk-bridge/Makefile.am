NULL =

moduledir = $(libdir)/gtk-2.0/modules
module_LTLIBRARIES = libatk-bridge.la

libatk_bridge_la_SOURCES = 	\
	bridge.c

libatk_bridge_la_DEPENDENCIES =	\
	$(top_srcdir)/libspi/application.h

INCLUDES = -I $(top_srcdir) \
	   -I $(top_builddir) \
	   -I $(top_srcdir)/libspi \
	   -I $(top_builddir)/libspi \
	   -I $(top_builddir)/atk-bridge \
	   $(WARN_CFLAGS) \
	   $(ATK_BRIDGE_CFLAGS) \
	   -DG_LOG_DOMAIN=\"atk-bridge\" \
	   $(NULL)

DEPS = 	\
	$(libatk)	\
	$(libbonobo-2)	\
	$(liboaf-2)	\
	$(libORBit-2)	\
	$(libat-spi)

libatk_bridge_la_LDFLAGS =  	\
	-rpath $(moduledir) -module -avoid-version -no-undefined \
	@LDFLAGS@

libatk_bridge_la_LIBADD = \
	$(top_builddir)/libspi/libspi.la	\
	$(ATK_BRIDGE_LIBS)	
