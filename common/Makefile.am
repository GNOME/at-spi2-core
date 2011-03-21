noinst_LTLIBRARIES = libspicommon.la

libspicommon_la_CFLAGS = $(DBUS_CFLAGS)\
			$(ATK_CFLAGS) \
			-DATSPI_INTROSPECTION_PATH=\"$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"\
			-I$(top_builddir)\
			-I$(top_srcdir)

libspicommon_la_LIBADD = $(DBUS_LIBS)

libspicommon_la_SOURCES =	\
	bitarray.h              \
        keymasks.h              \
        generated-types.h       \
	event-types.h		\
	spi-types.h		\
	spi-dbus.h		\
	spi-dbus.c              \
	spi-stateset.h          \
	spi-stateset.c

#BUILT_SOURCES = generated-types.h
#CLEANFILES = generated-types.h

#generated-types.h: $(top_srcdir)/xml/spec.xml $(top_srcdir)/tools/c-constants-generator.xsl
#	xsltproc --stringparam mixed-case-prefix Accessibility_ $(top_srcdir)/tools/c-constants-generator.xsl $< >$@
