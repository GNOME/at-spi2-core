EXTRA_DIST = atk.def
if OS_WIN32
export_symbols = -export-symbols atk.def

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

libatk_la_SOURCES = 		\
	atkaction.c		\
	atkcomponent.c		\
	atkdocument.c		\
	atkeditabletext.c	\
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
	atkvalue.c

EXTRA_DIST += atkmarshal.list
MAINTAINERCLEANFILES += 	\
	atkmarshal.c		\
	atkmarshal.h		\
	stamp-atkmarshal.c	\
	stamp-atkmarshal.h

INCLUDES = \
	-I$(top_srcdir)		\
	@DEP_CFLAGS@

LDFLAGS =  \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	-no-undefined						\
	$(export_symbols)					\
	@DEP_LIBS@

libatkincludedir=$(includedir)/atk-1.0/atk

libatkinclude_HEADERS =	\
        atk.h			\
        atkaction.h		\
        atkcomponent.h		\
        atkdocument.h		\
        atkeditabletext.h	\
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

$(libatk_la_OBJECTS): atkmarshal.c atkmarshal.h

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

atk.lib: libatk.la atk.def
	lib -name:libatk-@LT_CURRENT_MINUS_AGE@.dll -def:atk.def -out:$@

install-data-local: install-ms-lib install-libtool-import-lib

uninstall-local: uninstall-ms-lib uninstall-libtool-import-lib
