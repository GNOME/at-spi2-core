
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
	atktable.c		\
	atktext.c		\
	atkutil.c		\
	atkvalue.c

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
        atktable.h		\
        atktext.h		\
        atkutil.h		\
        atkvalue.h
