# We set GPATH here; this gives us semantics for GNU make
# which are more like other make's VPATH.
GPATH = $(srcdir)

include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ENV = CC="$(CC)"

CLEANFILES =

if PLATFORM_WIN32
no_undefined = -no-undefined
else
no_undefined =
endif

AM_CPPFLAGS = \
	-I$(top_srcdir)		\
	-DG_DISABLE_DEPRECATED	\
	-DATK_DISABLE_DEPRECATED	\
	-DATK_COMPILATION	\
	-DATK_LOCALEDIR="\"$(datadir)/locale\""	\
	$(DEP_CFLAGS)	\
	$(ATK_HIDDEN_VISIBILITY_CFLAGS)

lib_LTLIBRARIES = libatk-1.0.la

libatk_1_0_la_LDFLAGS = 			\
	-version-info $(LT_VERSION_INFO)	\
	$(no_undefined)
libatk_1_0_la_LIBADD = \
       $(DEP_LIBS)     \
       $(INTLLIBS)

atk_sources = 	\
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
        atkvalue.h \
        atkwindow.h

libatkinclude_HEADERS =		\
        atk.h			\
	$(atk_headers)		\
	$(top_builddir)/atk/atkversion.h \
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

atk-enum-types.h: s-enum-types-h
	@true
s-enum-types-h: @REBUILD@ $(atk_headers) Makefile
	$(AM_V_GEN) ( cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#if defined(ATK_DISABLE_SINGLE_INCLUDES) && !defined (__ATK_H_INSIDE__) && !defined (ATK_COMPILATION)\n#error \"Only <atk/atk.h> can be included directly.\"\n#endif\n\n#ifndef __ATK_ENUM_TYPES_H__\n#define __ATK_ENUM_TYPES_H__\n\n#include <atk/atkversion.h>\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "ATK_AVAILABLE_IN_ALL\nGType @enum_name@_get_type (void);\n#define ATK_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n"         \
			--ftail "G_END_DECLS\n\n#endif /* __ATK_ENUM_TYPES_H__ */" \
		$(atk_headers) ) > tmp-atk-enum-types.h \
	&& (cmp -s tmp-atk-enum-types.h atk-enum-types.h || cp tmp-atk-enum-types.h atk-enum-types.h ) \
	&& rm -f tmp-atk-enum-types.h \
	&& echo timestamp > $(@F)

atk-enum-types.c: s-enum-types-c
	@true
s-enum-types-c: @REBUILD@ $(atk_headers) Makefile
	$(AM_V_GEN) ( cd $(srcdir) && $(GLIB_MKENUMS) \
			--fhead "#include \"config.h\"\n\n#include <atk.h>" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {"     \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(atk_headers) ) > tmp-atk-enum-types.c \
	&& (cmp -s tmp-atk-enum-types.c atk-enum-types.c || cp tmp-atk-enum-types.c atk-enum-types.c ) \
	&& rm -f tmp-atk-enum-types.c \
	&& echo timestamp > $(@F)


if HAVE_INTROSPECTION
introspection_sources = \
    $(atk_headers)	\
    $(atk_sources)
introspection_generated_sources = \
    $(top_builddir)/atk/atkversion.h 	\
    atk-enum-types.c	\
    atk-enum-types.h

Atk-1.0.gir: libatk-1.0.la Makefile
Atk_1_0_gir_INCLUDES = GObject-2.0
Atk_1_0_gir_CFLAGS = $(AM_CPPFLAGS) -I ..
Atk_1_0_gir_LIBS = libatk-1.0.la
Atk_1_0_gir_SCANNERFLAGS = --pkg-export atk --warn-all --c-include "atk/atk.h"
Atk_1_0_gir_FILES = \
    $(addprefix $(srcdir)/, $(introspection_sources))	\
    $(introspection_generated_sources)
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

EXTRA_DIST = atk.rc.in atkmarshal.list atk.rc atkversion.h.in

DISTCLEANFILES = \
  stamp-atkmarshal.h stamp-atkmarshal.c \
  s-enum-types-h s-enum-types-c

distclean-local:
	if test $(srcdir) = .; then :; else \
	  rm -f atkmarshal.h atkmarshal.c atk-enum-types.h atk-enum-types.c; \
	fi
