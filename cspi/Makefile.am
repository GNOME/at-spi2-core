NULL=

lib_LTLIBRARIES = libcspi.la

libcspi_la_LDFLAGS = @LDFLAGS@ @LT_VERSION_INFO@ @LIBTOOL_EXPORT_OPTIONS@ -no-undefined

libcspi_la_CFLAGS = $(DBUS_GLIB_CFLAGS) $(ATK_CFLAGS) -I$(top_srcdir)

LDADD = $(DBUS_GLIB_LIBS)			\
		      $(top_builddir)/spi-common/libspicommon.la

libcspidir = $(includedir)/at-spi-1.0/cspi
libcspi_HEADERS =		\
	spi.h			\
	spi-impl.h		\
	spi-listener.h		\
	spi-roletypes.h		\
	spi-statetypes.h

libcspi_la_SOURCES =		\
	cspi-lowlevel.h		\
	spi_accessible.c \
	spi_action.c \
	spi_component.c \
	spi_editabletext.c \
	spi_event.c \
	spi_hyperlink.c \
	spi_hypertext.c \
	spi_listener.c \
	spi_main.c		\
	spi_registry.c \
	spi_selection.c \
	spi_stateset.c \
	spi_streamablecontent.c \
	spi_table.c \
	spi_text.c \
	spi_value.c \
	spi-private.h

libcspi_la_LIBADD =		\
	$(top_builddir)/spi-common/libspicommon.la \
		      $(top_builddir)/dbind/libdbind.la\
	$(LIBCSPI_LIBS) $(X_LIBS)
