# We set GPATH here; this gives us semantics for GNU make
# which are more like other make's VPATH.
GPATH = $(srcdir)

EXTRA_DIST = atk.def
if OS_WIN32
export_symbols = -export-symbols atk.def
no_undefined = -no-undefined

install-libtool-import-lib:
	$(INSTALL) .libs/libatk.dll.a $(DESTDIR)$(libdir)
uninstall-libtool-import-lib:
	-rm $(DESTDIR)$(libdir)/libatk.dll.a
else
install-libtool-import-lib:
uninstall-libtool-import-lib:
endif

lib_LTLIBRARIES = libatk.la

if MS_LIB_AVAILABLE
noinst_DATA = atk.lib

install-ms-lib:
	$(INSTALL) atk.lib $(DESTDIR)$(libdir)

uninstall-ms-lib:
	-rm $(DESTDIR)$(libdir)/atk.lib
else
install-ms-lib:
uninstall-ms-lib:
endif

atk_built_headers = atk-enum-types.h
atk_built_cfiles = atk-enum-types.c

$(OBJECTS): $(atk_built_headers)

libatk_la_SOURCES = 		\
	atkaction.c		\
	atkcomponent.c		\
	atkdocument.c		\
	atkeditabletext.c	\
	atkgobjectaccessible.c	\
	atkhyperlink.c		\
	atkhypertext.c		\
	atkimage.c		\
	atknoopobject.c		\
	atknoopobjectfactory.c	\
	atkobject.c		\
	atkobjectfactory.c	\
	atkregistry.c		\
	atkrelation.c		\
	atkrelationset.c	\
	atkselection.c		\
	atkstate.c		\
	atkstateset.c		\
	atkstreamablecontent.c	\
	atktable.c		\
	atktext.c		\
	atkutil.c		\
	atkvalue.c		\
	$(atk_built_cfiles)

EXTRA_DIST += atkmarshal.list
MAINTAINERCLEANFILES += 	\
	atkmarshal.c		\
	atkmarshal.h		\
	stamp-atkmarshal.c	\
	stamp-atkmarshal.h

INCLUDES = \
	-I$(top_srcdir)		\
	-DG_DISABLE_DEPRECATED	\
	@DEP_CFLAGS@

LDFLAGS =  \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	$(export_symbols)					\
	$(no_undefined)						\
	@DEP_LIBS@

libatkincludedir=$(includedir)/atk-1.0/atk

atk_headers = \
        atk.h			\
        atkaction.h		\
        atkcomponent.h		\
        atkdocument.h		\
        atkeditabletext.h	\
        atkgobjectaccessible.h	\
        atkhyperlink.h		\
        atkhypertext.h		\
        atknoopobject.h		\
        atknoopobjectfactory.h	\
        atkobject.h		\
	atkobjectfactory.h	\
        atkimage.h		\
        atkregistry.h		\
        atkrelation.h		\
        atkrelationset.h	\
        atkselection.h		\
        atkstate.h		\
        atkstateset.h		\
        atkstreamablecontent.h	\
        atktable.h		\
        atktext.h		\
        atkutil.h		\
        atkvalue.h

libatkinclude_HEADERS =		\
	$(atk_headers)		\
	$(atk_built_headers)

$(libatk_la_OBJECTS): atkmarshal.c atkmarshal.h $(atk_built_headers)

atkmarshal.h: @REBUILD@ stamp-atkmarshal.h
	@true
stamp-atkmarshal.h: atkmarshal.list
	case @GLIB_GENMARSHAL@ in									\
	   .*) glib_genmarshal=`pwd`/@GLIB_GENMARSHAL@ ;;				\
	   *) glib_genmarshal=@GLIB_GENMARSHAL@ ;;							\
	esac;												\
	$$glib_genmarshal --prefix=atk_marshal $(srcdir)/atkmarshal.list --header >> xgen-gmh		\
	&& (cmp -s xgen-gmh $(srcdir)/atkmarshal.h || cp xgen-gmh $(srcdir)/atkmarshal.h)		\
	&& rm -f xgen-gmh xgen-gmh~									\
	&& echo timestamp > $(@F)

atkmarshal.c: @REBUILD@ stamp-atkmarshal.c
	@true
stamp-atkmarshal.c: atkmarshal.list
	case @GLIB_GENMARSHAL@ in									\
	   .*) glib_genmarshal=`pwd`/@GLIB_GENMARSHAL@ ;;				\
	   *) glib_genmarshal=@GLIB_GENMARSHAL@ ;;							\
	esac;												\
	$$glib_genmarshal --prefix=atk_marshal $(srcdir)/atkmarshal.list --body >> xgen-gmc		\
	&& (cmp -s xgen-gmc $(srcdir)/atkmarshal.c || cp xgen-gmc $(srcdir)/atkmarshal.c)		\
	&& rm -f xgen-gmc xgen-gmc~									\
	&& echo timestamp > $(@F)

atk-enum-types.h: s-enum-types-h
	@true

s-enum-types-h: @REBUILD@ $(atk_headers) Makefile
	( cd $(srcdir) && glib-mkenums \
			--fhead "#ifndef __ATK_ENUM_TYPES_H__\n#define __ATK_ENUM_TYPES_H__\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void);\n#define ATK_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n"         \
			--ftail "G_END_DECLS\n\n#endif /* __ATK_ENUM_TYPES_H__ */" \
		$(atk_headers) ) > tmp-atk-enum-types.h \
	&& (cmp -s tmp-atk-enum-types.h $(srcdir)/atk-enum-types.h || cp tmp-atk-enum-types.h $(srcdir)/atk-enum-types.h ) \
	&& rm -f tmp-atk-enum-types.h \
	&& echo timestamp > $(@F)

atk-enum-types.c: s-enum-types-c
	@true

s-enum-types-c: @REBUILD@ $(atk_headers) Makefile
	( cd $(srcdir) && glib-mkenums \
			--fhead "#include <atk.h>" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {"     \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(atk_headers) ) > tmp-atk-enum-types.c \
	&& (cmp -s tmp-atk-enum-types.c $(srcdir)/atk-enum-types.c || cp tmp-atk-enum-types.c $(srcdir)/atk-enum-types.c ) \
	&& rm -f tmp-atk-enum-types.c \
	&& echo timestamp > $(@F)


atk.lib: libatk.la atk.def
	lib -name:libatk-@LT_CURRENT_MINUS_AGE@.dll -def:atk.def -out:$@

install-data-local: install-ms-lib install-libtool-import-lib

uninstall-local: uninstall-ms-lib uninstall-libtool-import-lib
