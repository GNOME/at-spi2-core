noinst_LTLIBRARIES = libspicommon.la

libspicommon_la_CFLAGS = $(DBUS_GLIB_CFLAGS)\
			 $(ATK_CFLAGS) \
			 -DATSPI_INTROSPECTION_PATH=\"$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"\
			 -I$(top_builddir)\
			 -I$(top_srcdir)

libspicommon_la_LIBADD = $(DBUS_GLIB_LIBS)

libspicommon_la_SOURCES =	\
	de-marshaller.h \
	de-marshaller.c \
	de-types.h \
	keymasks.h \
	paths.h

#generated-types.h: $(top_srcdir)/xml/spec.xml $(top_srcdir)/tools/c-constants-generator.xsl
#	xsltproc --stringparam mixed-case-prefix Accessibility_ $(top_srcdir)/tools/c-constants-generator.xsl $< >$@
