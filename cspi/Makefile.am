NULL=

lib_LTLIBRARIES = libcspi.la

INCLUDES = -I $(top_srcdir)           \
           -I $(top_builddir)         \
           $(LIBCSPI_CFLAGS)

LDFLAGS = $(LIBCSPI_LIBS) @LT_VERSION_INFO@

LDADD = ../libspi/libspi.la $(LIBCSPI_LIBS)

libcspidir = $(includedir)/at-spi-1.0/cspi
libcspi_HEADERS =		\
	spi.h			\
	spi-impl.h		\
	spi-listener.h		\
	spi-roletypes.h		\
	spi-statetypes.h

libcspi_la_SOURCES =		\
	spi_main.c		\
	spi_event.c		\
	spi_registry.c		\
	spi-util.c		\
	spi_accessible.c	\
	spi_action.c		\
	spi_application.c	\
	spi_component.c		\
	spi_editabletext.c	\
	spi_hyperlink.c		\
	spi_hypertext.c		\
	spi_image.c		\
	spi-private.h		\
	spi_selection.c		\
	spi_table.c		\
	spi_text.c		\
	spi_value.c
