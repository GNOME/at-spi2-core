SUBDIRS = bonobo

NULL=

lib_LTLIBRARIES = libcspi.la

INCLUDES = -I $(top_srcdir)           \
           -I $(top_builddir)         \
           $(X_CFLAGS)                \
           $(LIBCSPI_CFLAGS)

LDFLAGS = @LDFLAGS@ $(LIBCSPI_LIBS) @LT_VERSION_INFO@

LDADD =				\
	../libspi/libspi.la	\
	$(LIBCSPI_LIBS) $(X_LIBS)

libcspidir = $(includedir)/at-spi-1.0/cspi
libcspi_HEADERS =		\
	spi.h			\
	spi-impl.h		\
	spi-listener.h		\
	spi-roletypes.h		\
	spi-statetypes.h

libcspi_la_SOURCES =		\
	cspi-lowlevel.h		\
	spi_accessible.c	\
	spi_action.c		\
	spi_application.c	\
	spi_component.c		\
	spi_editabletext.c	\
	spi_event.c		\
	spi_hyperlink.c		\
	spi_hypertext.c		\
	spi_image.c		\
	spi_main.c		\
	spi-private.h		\
	spi_registry.c		\
	spi_selection.c		\
	spi_table.c		\
	spi_text.c		\
	spi_value.c

libcspi_la_LIBADD =		\
	bonobo/libcspi-bonobo.la
