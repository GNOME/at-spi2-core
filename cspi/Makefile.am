lib_LTLIBRARIES = libcspi.la

libcspi_la_LDFLAGS = @LDFLAGS@ @LT_VERSION_INFO@ @LIBTOOL_EXPORT_OPTIONS@ -no-undefined

libcspi_la_CFLAGS = $(DBUS_GLIB_CFLAGS) \
                    $(ATK_CFLAGS)       \
		    $(DBIND_CFLAGS)     \
		    $(DROUTE_CFLAGS)    \
                    -I$(top_srcdir)


libcspidir = $(includedir)/at-spi-1.0/cspi
libcspi_HEADERS =		\
	spi.h			\
	spi-impl.h		\
	spi-listener.h		\
	spi-roletypes.h		\
	spi-statetypes.h

libcspi_la_SOURCES =		\
	cspi-lowlevel.h		\
	spi-accessible.c \
	spi-action.c \
	spi-application.c \
	spi-component.c \
	spi-editabletext.c \
	spi-event.c \
	spi-hyperlink.c \
	spi-hypertext.c \
	spi-image.c \
	spi-listener.c \
	spi-main.c		\
	spi-registry.c \
	spi-selection.c \
	spi-streamablecontent.c \
	spi-table.c \
	spi-text.c \
	spi-value.c \
	spi-private.h

libcspi_la_LIBADD =		        \
	$(LIBCSPI_LIBS)                 \
        $(DBIND_LIBS)			\
        $(X_LIBS)                       \
        $(DBUS_GLIB_LIBS)		\
        $(ATK_LIBS)                     \
        $(top_builddir)/droute/libdbind.la \
        $(top_builddir)/droute/libdroute.la \
        $(top_builddir)/common/libspicommon.la
