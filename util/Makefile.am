NULL =

idldir = $(datadir)

idl_DATA = idl/Magnifier.idl

dist_SUBDIRS = idl

lib_LTLIBRARIES = libat-util.la

bin_PROGRAMS = magnifier

noinst_PROGRAMS = mag_control

libat_util_la_SOURCES = \
	mag_client.c 	\
	mag_client.h	\
	Magnifier-common.c \
	Magnifier-stubs.c

libat_util_la_DEPENDENCIES = \
	Magnifier.h

magnifier_SOURCES = magnifier.c \
	mag_image.c \
	mag_image.h \
	magnifier.h \
	Magnifier.h \
	Magnifier-skels.c \
	Magnifier-common.c

mag_control_SOURCES = mag_control.c \
	mag_client.h		\
	Magnifier-common.c	\
	Magnifier-stubs.c

INCLUDES = \
	$(UTILS_CFLAGS)		\
	-I$(top_srcdir)		\
	-I$(top_builddir)	\
	-I$(top_srcdir)/util/idl

CFLAGS += $(UTILS_CFLAGS) 

IDL_OUT = Magnifier.h Magnifier-skels.c Magnifier-stubs.c Magnifier-common.c

IDL = $(top_srcdir)/util/$(idl_DATA)

BUILT_SOURCES = $(IDL_OUT)
CLEANFILES+=$(IDL_OUT)

mag_client.c mag_client.h mag_control.c magnifier.h : Magnifier.h

IDLFLAGS = -I$(BONOBO_ACTIVATION_IDL_DIR) 	\
	-I$(LIBBONOBO_IDL_DIR)			\
	-I$(top_srcdir)/util/idl		

orbittypelibdir = $(libdir)/orbit

LDADD = $(UTILS_LIBS) $(X_LIBS)

serverinfodir = $(libdir)/bonobo/servers
serverinfo_DATA = Accessibility_Util.server

@INTLTOOL_SERVER_RULE@

EXTRA_DIST = Accessibility_Util.server.in $(serverinfo_DATA) $(idl_DATA)

$(IDL_OUT) : $(IDL) $(ORBIT_IDL)
	$(ORBIT_IDL) $(IDLFLAGS) $(IDL)

DONT_DIST_SOURCE = $(IDL_OUT)

dist-hook:
	for file in $(DONT_DIST_SOURCE) ; do \
	    rm -f $(distdir)/$$file ; \
	done
