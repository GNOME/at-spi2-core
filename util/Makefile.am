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
	mag_client.h\
	Magnifier-common.c	\
	Magnifier-stubs.c

INCLUDES =            \
	   $(UTILS_CFLAGS) \
	   -I$(top_srcdir)/util/idl

CFLAGS += $(UTILS_CFLAGS) 

IDL_OUT = Magnifier.h Magnifier-skels.c Magnifier-stubs.c Magnifier-common.c

IDL = $(idl_DATA)

BUILT_SOURCES = $(IDL_OUT)
CLEANFILES+=$(IDL_OUT)

IDLFLAGS = -I$(BONOBO_ACTIVATION_IDL_DIR) 	\
	-I$(LIBBONOBO_IDL_DIR)			\
	-I$(top_srcdir)/util/idl		

orbittypelibdir = $(libdir)/orbit

LDADD = $(UTILS_LIBS) -L$(prefix)/lib -lgdk_pixbuf_xlib-1.3

serverinfodir = $(libdir)/bonobo/servers
serverinfo_DATA = Accessibility_Util.server

@XML_I18N_MERGE_SERVER_RULE@

EXTRA_DIST = Accessibility_Util.server.in $(serverinfo_DATA) $(idl_DATA)

$(IDL_OUT) : $(IDL) $(ORBIT_IDL)
	$(ORBIT_IDL) $(IDLFLAGS) $(IDL)
