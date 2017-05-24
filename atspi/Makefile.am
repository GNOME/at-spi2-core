lib_LTLIBRARIES =

BUILT_SOURCES =

CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

lib_LTLIBRARIES += libatspi.la

libatspi_la_LDFLAGS = \
	@LDFLAGS@ @LT_VERSION_INFO@ @LIBTOOL_EXPORT_OPTIONS@ \
	-no-undefined

libatspi_la_CFLAGS = \
	$(DBUS_CFLAGS) \
	$(DBIND_CFLAGS) \
	$(GLIB_CFLAGS) \
	-I$(top_srcdir)/registryd \
	-I$(top_builddir)/registryd \
	-I$(top_builddir) \
	-I$(top_srcdir)

libatspi_la_LIBADD = \
	$(DBUS_LIBS) \
	$(GOBJ_LIBS) \
	$(X11_LIBS) \
	$(top_builddir)/dbind/libdbind.la

libatspi_la_DEPENDENCIES = $(top_builddir)/dbind/libdbind.la

atspi_headers = \
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

libatspi_la_SOURCES = \
	$(BUILT_SOURCES) \
	atspi-accessible.c \
	atspi-accessible-private.h \
	atspi-action.c \
	atspi-application.c \
	atspi-collection.c \
	atspi-component.c \
	atspi-device-listener.c \
	atspi-device-listener-private.h \
	atspi-document.c \
	atspi-editabletext.c \
	atspi-event-listener.c \
	atspi-event-listener-private.h \
	atspi-gmain.c \
	atspi-hyperlink.c \
	atspi-hypertext.c \
	atspi-image.c \
	atspi-matchrule.c \
	atspi-matchrule-private.h \
	atspi-misc.c \
	atspi-misc-private.h \
	atspi-object.c \
	atspi-private.h \
	atspi-registry.c \
	atspi-relation.c \
	atspi-selection.c \
	atspi-stateset.c \
	atspi-table.c \
	atspi-table-cell.c \
	atspi-text.c \
	atspi-value.c \
	$()

BUILT_SOURCES += \
	atspi-enum-types.c \
	atspi-enum-types.h

libatspiincludedir = $(includedir)/at-spi-2.0/atspi
libatspiinclude_HEADERS = $(atspi_headers) atspi-enum-types.h

#CLEANFILES = atspi-constants.h

#atspi-constants.h: $(top_srcdir)/xml/spec.xml $(top_srcdir)/tools/c-constants-gen.py
#	python $(top_srcdir)/tools/c-constants-gen.py Atspi $(top_srcdir)/xml/spec.xml atspi-constants

ENUM_TYPES = \
	atspi-constants.h \
	atspi-types.h

atspi-enum-types.h: atspi-enum-types.h.template $(ENUM_TYPES) $(GLIB_MKENUMS)
	$(AM_V_GEN) $(GLIB_MKENUMS) --template $(srcdir)/atspi-enum-types.h.template $(addprefix $(srcdir)/, $(ENUM_TYPES)) > $@.tmp \
	&& (cmp -s $@.tmp $@ || cp -f $@.tmp $@) \
	&& rm -f $@.tmp

atspi-enum-types.c: atspi-enum-types.c.template $(ENUM_TYPES) $(GLIB_MKENUMS)
	$(AM_V_GEN) $(GLIB_MKENUMS) --template $(srcdir)/atspi-enum-types.c.template $(addprefix $(srcdir)/, $(ENUM_TYPES)) > $@.tmp \
	&& (cmp -s $@.tmp $@ || cp -f $@.tmp $@) \
	&& rm -f $@.tmp

CLEANFILES += \
	atspi-enum-types.c \
	atspi-enum-types.h

EXTRA_DIST += \
	atspi-enum-types.c.template \
	atspi-enum-types.h.template

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

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

CLEANFILES += \
    $(gir_DATA) \
    $(typelib_DATA)
endif

-include $(top_srcdir)/git.mk
