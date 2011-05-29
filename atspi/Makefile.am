lib_LTLIBRARIES = libatspi.la

libatspi_la_LDFLAGS = @LDFLAGS@ @LT_VERSION_INFO@ @LIBTOOL_EXPORT_OPTIONS@ -no-undefined

libatspi_la_CFLAGS = $(DBUS_CFLAGS) \
		    $(DBIND_CFLAGS)     \
		    $(GLIB_CFLAGS)     \
		    -I$(top_srcdir)/registryd \
                    -I$(top_srcdir)

libatspi_la_LIBADD = $(DBUS_LIBS) \
	$(GOBJ_LIBS) \
	$(X_LIBS) \
	$(top_builddir)/dbind/libdbind.la

libatspiincludedir = $(includedir)/at-spi-2.0/atspi

libatspiinclude_HEADERS = \
	atspi.h	\
	atspi-accessible.h \
	atspi-action.h \
	atspi-application.h \
	atspi-collection.h \
	atspi-component.h \
	atspi-constants.h \
	atspi-device-listener.h \
	atspi-device-listener-private.h \
	atspi-document.h \
	atspi-editabletext.h \
	atspi-event-listener.h \
	atspi-event-listener-private.h \
atspi-gmain.c \
atspi-gmain.h \
	atspi-hyperlink.h \
	atspi-hypertext.h \
	atspi-image.h \
	atspi-matchrule.h \
	atspi-misc.h \
	atspi-object.h \
	atspi-private.h \
	atspi-registry.h \
	atspi-relation.h \
	atspi-selection.h \
	atspi-stateset.h \
	atspi-table.h \
	atspi-text.h \
	atspi-types.h \
	atspi-value.h

libatspi_la_SOURCES =		\
	$(libatspiinclude_HEADERS) \
	atspi.h	\
	atspi-accessible.c \
	atspi-accessible.h \
	atspi-action.c \
	atspi-action.h \
	atspi-application.c \
	atspi-application.h \
	atspi-collection.c \
	atspi-collection.h \
	atspi-component.c \
	atspi-component.h \
	atspi-constants.h \
	atspi-device-listener.c \
	atspi-device-listener.h \
	atspi-device-listener-private.h \
	atspi-document.c \
	atspi-document.h \
	atspi-editabletext.c \
	atspi-editabletext.h \
	atspi-event-listener.c \
	atspi-event-listener.h \
	atspi-event-listener-private.h \
	atspi-hyperlink.c \
	atspi-hyperlink.h \
	atspi-hypertext.c \
	atspi-hypertext.h \
	atspi-image.c \
	atspi-image.h \
	atspi-matchrule.c \
	atspi-matchrule.h \
	atspi-matchrule-private.h \
	atspi-misc.c \
	atspi-misc.h \
	atspi-misc-private.h \
	atspi-object.c \
	atspi-object.h \
	atspi-private.h \
	atspi-registry.c \
	atspi-registry.h \
	atspi-relation.c \
	atspi-relation.h \
	atspi-selection.c \
	atspi-selection.h \
	atspi-stateset.c \
	atspi-stateset.h \
	atspi-table.c \
	atspi-table.h \
	atspi-text.c \
	atspi-text.h \
	atspi-types.h \
	atspi-value.c \
	atspi-value.h

#BUILT_SOURCES = atspi-constants.h

#CLEANFILES = atspi-constants.h

#atspi-constants.h: $(top_srcdir)/xml/spec.xml $(top_srcdir)/tools/c-constants-gen.py
#	python $(top_srcdir)/tools/c-constants-gen.py Atspi $(top_srcdir)/xml/spec.xml atspi-constants

if HAVE_INTROSPECTION
INTROSPECTION_FILES = $(libatspi_la_SOURCES)

Atspi-2.0.gir: $(INTROSPECTION_SCANNER) \
		libatspi.la \
		$(INTROSPECTION_FILES)
	$(AM_V_GEN)$(INTROSPECTION_SCANNER) -v \
		--namespace Atspi \
		--nsversion=2.0 \
		-I$(top_builddir) \
		-I$(top_srcdir) \
		--include=GLib-2.0 \
		--include=GObject-2.0 \
		--libtool="$(top_builddir)/libtool" \
		--library=libatspi.la \
		--library=$(top_builddir)/dbind/libdbind.la \
		--pkg dbus-1 \
		--warn-all \
		--output $@ \
		$(addprefix $(srcdir)/, $(INTROSPECTION_FILES))

girdir = $(datadir)/gir-1.0
gir_DATA = Atspi-2.0.gir

%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(AM_V_GEN)$(top_builddir)/libtool --mode=execute \
	    $(INTROSPECTION_COMPILER) \
	        --includedir=$(srcdir) \
	        --includedir=. \
	        $(INTROSPECTION_COMPILER_OPTS) $< -o $(@F)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(gir_DATA:.gir=.typelib)

CLEANFILES = \
    $(gir_DATA) \
    $(typelibs_DATA)

endif
