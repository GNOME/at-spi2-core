NULL=

lib_LTLIBRARIES = libcspi.la

INCLUDES = -I $(top_srcdir)           \
           -I $(top_builddir)         \
           -I $(top_srcdir)/libspi    \
           -I $(top_builddir)/libspi  \
           $(LIBCSPI_CFLAGS)

CFLAGS += $(LIBCSPI_CFLAGS)

LDFLAGS = $(LIBCSPI_LIBS) @LT_VERSION_INFO@

LDADD = ../libspi/libspi.la $(LIBCSPI_LIBS)

libcspi_la_SOURCES = spi.c              \
                     spi.h         	\
                     spi-impl.h        	\
		     spi-listener.h 	\
		     spi-statetypes.h	\
		     spi-roletypes.h

SPICSOURCES =        spi_main.c		\
		     spi_event.c 	\
		     spi_registry.c	\
		     spi_accessible.c	\
		     spi_application.c	\
		     spi_component.c

spi.c : $(SPICSOURCES)
