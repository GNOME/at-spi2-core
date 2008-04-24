lib_LTLIBRARIES = libspi.la

#LIBSPI_DEBUG_CFLAGS=-DSPI_DEBUG

INCLUDES = -I$(top_srcdir)	\
           -I$(top_builddir)	\
           $(LIBSPI_DEBUG_CFLAGS)	\
           $(WARN_CFLAGS)	\
           $(LIBSPI_CFLAGS)     \
           $(X_CFLAGS)

libspi_la_LDFLAGS = @LT_VERSION_INFO@ -no-undefined
libspi_la_LIBADD = $(LIBSPI_LIBS) $(XTST_LIBS)

libspiincludedir = $(includedir)/at-spi-1.0/libspi

libspiinclude_HEADERS =		\
	Accessibility.h		\
	accessible.h \
	bitarray.h \
	dbus.h \
	droute.h

BUILT_SOURCES = Accessibility.h
CLEANFILES = Accessibility.h

libspi_la_SOURCES =		\
	Accessibility.h \
	accessible.c		\
	action.c \
	application.c		\
	component.c		\
	dbus.c \
	document.c		\
	droute.c \
	editabletext.c		\
	hyperlink.c		\
	hypertext.c		\
	image.c			\
	selection.c		\
	table.c			\
	text.c			\
	value.c

Accessibility.h: ../xml/spec.xml ../tools/c-constants-generator.xsl
	xsltproc --stringparam mixed-case-prefix Accessibility_ ../tools/c-constants-generator.xsl $< >$@

DONT_DIST_SOURCE = Accessibility.h

dist-hook:
	for file in $(DONT_DIST_SOURCE) ; do \
	    rm -f $(distdir)/$$file ; \
	done
