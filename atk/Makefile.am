# We set GPATH here; this gives us semantics for GNU make
# which are more like other make's VPATH.
GPATH = $(srcdir)

include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ENV = CC="$(CC)"

EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

if PLATFORM_WIN32
no_undefined = -no-undefined
else
no_undefined =
endif

AM_CPPFLAGS = \
	-I$(top_srcdir)	\
	-I$(top_builddir) \
	-DGLIB_DISABLE_DEPRECATION_WARNINGS \
	-DATK_DISABLE_DEPRECATIO_WARNINGS \
	-DATK_COMPILATION \
	-DATK_LOCALEDIR="\"$(datadir)/locale\""	\
	$(DEP_CFLAGS) \
	$(ATK_HIDDEN_VISIBILITY_CFLAGS)

lib_LTLIBRARIES = libatk-1.0.la

libatk_1_0_la_LDFLAGS = \
	-version-info $(LT_VERSION_INFO) \
	$(no_undefined)
libatk_1_0_la_LIBADD = \
       $(DEP_LIBS) \
       $(INTLLIBS)

atk_sources = \
	atkaction.c		\
	atkcomponent.c		\
	atkdocument.c		\
	atkeditabletext.c	\
	atkgobjectaccessible.c	\
	atkhyperlink.c		\
	atkhyperlinkimpl.c	\
	atkhypertext.c		\
	atkimage.c		\
	atknoopobject.c		\
	atknoopobjectfactory.c	\
	atkobject.c		\
	atkobjectfactory.c	\
	atkplug.c		\
	atkprivate.h		\
	atkprivate.c		\
	atkrange.c		\
	atkregistry.c		\
	atkrelation.c		\
	atkrelationset.c	\
	atkselection.c		\
	atksocket.c		\
	atkstate.c		\
	atkstateset.c		\
	atkstreamablecontent.c	\
	atktable.c		\
	atktablecell.c		\
	atktext.c		\
	atkutil.c		\
	atkmisc.c		\
	atkvalue.c 		\
	atkversion.c 		\
	atkwindow.c

libatk_1_0_la_SOURCES = 	\
	$(atk_sources)		\
	atkmarshal.c		\
	atk-enum-types.c

libatkincludedir=$(includedir)/atk-1.0/atk

atk_headers = \
        atkaction.h		\
        atkcomponent.h		\
        atkdocument.h		\
        atkeditabletext.h	\
        atkgobjectaccessible.h	\
        atkhyperlink.h		\
        atkhyperlinkimpl.h	\
        atkhypertext.h		\
        atknoopobject.h		\
        atknoopobjectfactory.h	\
        atkobject.h		\
	atkobjectfactory.h	\
        atkplug.h		\
        atkimage.h		\
	atkrange.h		\
        atkregistry.h		\
        atkrelation.h		\
        atkrelationtype.h	\
        atkrelationset.h	\
        atkselection.h		\
        atksocket.h		\
        atkstate.h		\
        atkstateset.h		\
        atkstreamablecontent.h	\
        atktable.h		\
        atktablecell.h		\
        atktext.h		\
        atkutil.h		\
        atkmisc.h		\
        atkvalue.h 		\
        atkwindow.h

libatkinclude_HEADERS =		\
        atk.h			\
	$(atk_headers)		\
	atkversion.h 		\
	atk-enum-types.h


# ---------- Handle built sources ----------

BUILT_SOURCES = atkmarshal.h atkmarshal.c atk-enum-types.h atk-enum-types.c

atkmarshal.h: stamp-atkmarshal.h
	@true
stamp-atkmarshal.h: @REBUILD@ atkmarshal.list
	$(AM_V_GEN) $(GLIB_GENMARSHAL) --prefix=atk_marshal $(srcdir)/atkmarshal.list --header >> xgen-gmh \
	&& (cmp -s xgen-gmh atkmarshal.h || cp xgen-gmh atkmarshal.h) \
	&& rm -f xgen-gmh xgen-gmh~					\
	&& echo timestamp > $(@F)

atkmarshal.c: stamp-atkmarshal.c
	@true
stamp-atkmarshal.c: @REBUILD@ atkmarshal.list
	$(AM_V_GEN) $(GLIB_GENMARSHAL) --prefix=atk_marshal $(srcdir)/atkmarshal.list --body >> xgen-gmc \
	&& (cmp -s xgen-gmc atkmarshal.c || cp xgen-gmc atkmarshal.c) \
	&& rm -f xgen-gmc xgen-gmc~					\
	&& echo timestamp > $(@F)

EXTRA_DIST += atkmarshal.list
CLEANFILES += stamp-atkmarshal.h stamp-atkmarshal.c
DISTCLEANFILES += atkmarshal.h atkmarshal.c

atk-enum-types.h: s-enum-types-h
	@true
s-enum-types-h: @REBUILD@ $(atk_headers) $(top_srcdir)/atk/atk-enum-types.h.template Makefile
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--template $(top_srcdir)/atk/atk-enum-types.h.template \
	$(addprefix $(srcdir)/, $(atk_headers)) > tmp-atk-enum-types.h \
	&& (cmp -s tmp-atk-enum-types.h atk-enum-types.h || cp -f tmp-atk-enum-types.h atk-enum-types.h ) \
	&& rm -f tmp-atk-enum-types.h \
	&& echo timestamp > $(@F)

atk-enum-types.c: s-enum-types-c
	@true
s-enum-types-c: @REBUILD@ $(atk_headers) $(top_srcdir)/atk/atk-enum-types.c.template Makefile
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--template $(top_srcdir)/atk/atk-enum-types.c.template \
	$(addprefix $(srcdir)/, $(atk_headers)) > tmp-atk-enum-types.c \
	&& (cmp -s tmp-atk-enum-types.c atk-enum-types.c || cp tmp-atk-enum-types.c atk-enum-types.c ) \
	&& rm -f tmp-atk-enum-types.c \
	&& echo timestamp > $(@F)

EXTRA_DIST += atk-enum-types.h.template atk-enum-types.c.template
CLEANFILES += s-enum-types-h s-enum-types-c
DISTCLEANFILES += atk-enum-types.h atk-enum-types.c

if HAVE_INTROSPECTION
introspection_sources = \
    $(atk_headers)	\
    $(atk_sources)
introspection_generated_sources = \
    atkversion.h 	\
    atk-enum-types.c	\
    atk-enum-types.h

Atk-1.0.gir: libatk-1.0.la Makefile
Atk_1_0_gir_INCLUDES = GObject-2.0
Atk_1_0_gir_CFLAGS = $(AM_CPPFLAGS)
Atk_1_0_gir_LIBS = libatk-1.0.la
Atk_1_0_gir_SCANNERFLAGS = --pkg-export=atk --warn-all --c-include="atk/atk.h"
Atk_1_0_gir_FILES = $(introspection_sources) $(introspection_generated_sources)
INTROSPECTION_GIRS += Atk-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)
endif

# ---------- Win32 stuff ----------
if OS_WIN32
libatk_1_0_la_LDFLAGS += -no-undefined -Wl,atk-win32-res.o
libatk_1_0_la_DEPENDENCIES = atk-win32-res.o
endif

atk-win32-res.o: atk.rc
	$(WINDRES) $< $@

EXTRA_DIST += atk.rc.in atk.rc

# ---------- MSVC Project Items ---------
MSVCPROJS = atk

atk_FILES = $(libatk_1_0_la_SOURCES)
atk_EXCLUDES = atkdummy

atk_HEADERS_DIR = $(libatkincludedir)
atk_HEADERS_INST = $(libatkinclude_HEADERS) atkversion.h
atk_HEADERS_EXCLUDES = $(builddir)/atkversion.h

include $(top_srcdir)/win32/Makefile.msvcproj

if HAVE_INTROSPECTION
# Introspection Items for MSVC
MSVC_INTROSPECT_GIRS = Atk-1.0.gir

INTROSPECTION_INTERMEDIATE_ITEMS = \
	$(top_builddir)/win32/Atk-1.0.gir.msvc.introspect	\
	$(top_builddir)/win32/Atk_1_0_gir_list

Atk_1_0_gir_MSVC_EXPORT_PACKAGES = atk
Atk_1_0_gir_MSVC_INCLUDE_GIRS = $(Atk_1_0_gir_INCLUDES)
Atk_1_0_gir_MSVC_LIBS = atk-1.0
Atk_1_0_gir_MSVC_FILES =		\
	$(introspection_sources)	\
	atkversion.h			\
	atk-enum-types.c		\
	atk-enum-types.h

Atk_1_0_gir_MSVC_CFLAGS =		\
	-I..				\
	-DG_DISABLE_DEPRECATED	\
	-DATK_DISABLE_DEPRECATED	\
	-DATK_COMPILATION	\
	-DATK_LOCALEDIR="\\\"/dummy/share/locale\\\""

Atk_1_0_gir_MSVC_SCANNERFLAGS = --warn-all --c-include "atk/atk.h"

include $(top_srcdir)/win32/Makefile.msvc-introspection

else
INTROSPECTION_INTERMEDIATE_ITEMS =
endif

dist-hook: \
	$(top_builddir)/win32/vs9/atk.vcproj	\
	$(top_builddir)/win32/vs9/atk.headers	\
	$(INTROSPECTION_INTERMEDIATE_ITEMS)
