
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

ATK_MARSHALFILES = atkmarshal.h atkmarshal.list atkmarshal.c
EXTRA_DIST += atkmarshal.c atkmarshal.list
MAINTAINERCLEANFILES += atkmarshal.c

Makefile: $(ATK_MARSHALFILES)

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

# initial creation of the real stamp-* files
atk.defs atkmarshal.h :	  # never add deps here
	test -f "$(srcdir)/$@" || touch $(srcdir)/$@

$(srcdir)/stamp-atkmarshal.h: @REBUILD@ atkmarshal.list atkmarshal.h
	cwd=`pwd` \
	&& cd $(srcdir) \
	&& $$cwd/@GLIB_GENMARSHAL@ --prefix=atk_marshal atkmarshal.list --header >> xgen-gmh \
	&& (cmp -s xgen-gmh atkmarshal.h || cp xgen-gmh atkmarshal.h) \
	&& rm -f xgen-gmh xgen-gmh~ \
	&& echo timestamp > $(@F)
$(srcdir)/atkmarshal.c: @REBUILD@ $(srcdir)/stamp-atkmarshal.h
	cwd=`pwd` \
	&& cd $(srcdir) \
	&& $$cwd/@GLIB_GENMARSHAL@ --prefix=atk_marshal atkmarshal.list --body >> xgen-gmc \
	&& cp xgen-gmc atkmarshal.c \
	&& rm -f xgen-gmc xgen-gmc~

