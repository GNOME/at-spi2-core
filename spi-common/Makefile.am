noinst_LTLIBRARIES = libspicommon.la

libspicommon_la_CFLAGS = $(DBUS_GLIB_CFLAGS) $(ATK_CFLAGS)\
			-DATSPI_INTROSPECTION_PATH=\"$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"\
			-I$(top_builddir)\
			-I$(top_srcdir)

BUILT_SOURCES = generated-types.h
CLEANFILES = generated-types.h

nodist_libspicommon_la_SOURCES = generated-types.h

libspicommon_la_SOURCES =	\
	event-types.h		\
	spi-types.h		\
	spi-dbus.h		\
	spi-dbus.c

generated-types.h: $(top_srcdir)/xml/spec.xml $(top_srcdir)/tools/c-constants-generator.xsl
	xsltproc --stringparam mixed-case-prefix Accessibility_ $(top_srcdir)/tools/c-constants-generator.xsl $< >$@
