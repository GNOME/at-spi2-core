
lib_LTLIBRARIES = libatk.la

libatk_la_SOURCES = 		\
	atkaction.c		\
	atkcomponent.c		\
	atkeditabletext.c	\
	atkhyperlink.c		\
	atkhypertext.c		\
	atkimage.c		\
	atknoopobject.c		\
        atknoopobject.h		\
	atknoopobjectfactory.c	\
        atknoopobjectfactory.h	\
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
MAINTAINERCLEANFILES += atkmarshal.c

INCLUDES = \
	-I$(top_srcdir)		\
	@DEP_CFLAGS@

LDFLAGS =  \
	-no-undefined	\
	@DEP_LIBS@

libatkincludedir=$(includedir)/atk-1.0/atk

libatkinclude_HEADERS =	\
        atk.h			\
        atkaction.h		\
        atkcomponent.h		\
        atkeditabletext.h	\
        atkhyperlink.h		\
        atkhypertext.h		\
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
	   .*) glib_genmarshal=`cd $(top_builddir) && pwd`/@GLIB_GENMARSHAL@ ;;				\
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
	   .*) glib_genmarshal=`cd $(top_builddir) && pwd`/@GLIB_GENMARSHAL@ ;;				\
	   *) glib_genmarshal=@GLIB_GENMARSHAL@ ;;							\
	esac;												\
	$$glib_genmarshal --prefix=atk_marshal $(srcdir)/atkmarshal.list --body >> xgen-gmc		\
	&& (cmp -s xgen-gmc $(srcdir)/atkmarshal.c || cp xgen-gmc $(srcdir)/atkmarshal.c)		\
	&& rm -f xgen-gmc xgen-gmc~									\
	&& echo timestamp > $(@F)
