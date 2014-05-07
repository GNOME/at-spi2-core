lib_LTLIBRARIES = libatspi.la

libatspi_la_LDFLAGS = @LDFLAGS@ @LT_VERSION_INFO@ @LIBTOOL_EXPORT_OPTIONS@ -no-undefined

libatspi_la_CFLAGS = $(DBUS_CFLAGS) \
		    $(DBIND_CFLAGS)     \
		    $(GLIB_CFLAGS)     \
		    -I$(top_srcdir)/registryd \
		    -I$(top_builddir)/registryd \
                    -I$(top_builddir) \
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
	atspi-enum-types.h \
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
	atspi-table-cell.h \
	atspi-text.h \
	atspi-types.h \
	atspi-value.h

libatspi_la_SOURCES =		\
	$(BUILT_SOURCES) \
	$(libatspiinclude_HEADERS) \
	atspi.h	\
	atspi-accessible.c \
	atspi-accessible.h \
	atspi-accessible-private.h \
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
	atspi-table-cell.c \
	atspi-table.h \
	atspi-text.c \
	atspi-text.h \
	atspi-types.h \
	atspi-value.c \
	atspi-value.h

BUILT_SOURCES = \
	atspi-enum-types.c \
	atspi-enum-types.h

#CLEANFILES = atspi-constants.h

#atspi-constants.h: $(top_srcdir)/xml/spec.xml $(top_srcdir)/tools/c-constants-gen.py
#	python $(top_srcdir)/tools/c-constants-gen.py Atspi $(top_srcdir)/xml/spec.xml atspi-constants

ENUM_TYPES = \
	atspi-constants.h \
	atspi-types.h

atspi-enum-types.h: atspi-enum-types.h.template $(ENUM_TYPES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template atspi-enum-types.h.template $(ENUM_TYPES)) > $@

atspi-enum-types.c: atspi-enum-types.c.template $(ENUM_TYPES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template atspi-enum-types.c.template $(ENUM_TYPES)) > $@

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

EXTRA_DIST = \
	atspi-enum-types.c.template \
	atspi-enum-types.h.template

if HAVE_INTROSPECTION
Atspi-2.0.gir: libatspi.la
Atspi_2_0_gir_INCLUDES = GLib-2.0 GObject-2.0
Atspi_2_0_gir_PACKAGES = dbus-1
Atspi_2_0_gir_EXPORT_PACKAGES = atspi-2
Atspi_2_0_gir_CFLAGS = -I$(top_builddir) -I$(top_srcdir)
Atspi_2_0_gir_LIBS = libatspi.la $(top_builddir)/dbind/libdbind.la
Atspi_2_0_gir_FILES = $(libatspi_la_SOURCES)
Atspi_2_0_gir_SCANNERFLAGS = --namespace Atspi --nsversion=2.0
INTROSPECTION_SCANNER_ARGS += --c-include='atspi/atspi.h'
INTROSPECTION_GIRS += Atspi-2.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = \
    $(gir_DATA) \
    $(typelib_DATA)

endif

-include $(top_srcdir)/git.mk
