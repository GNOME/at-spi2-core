bin_PROGRAMS = registryd

INCLUDES = -I$(top_srcdir)          \
           -I$(top_builddir)        \
           -I$(top_builddir)/libspi \
           -I$(top_srcdir)/libspi   \
           $(REGISTRYD_CFLAGS)

registryd_SOURCES = registryd.c

LDADD = ../libspi/libspi.la -lXtst $(REGISTRYD_LIBS) 

serverinfodir = $(libdir)/bonobo/servers
serverinfo_DATA = Accessibility_Registry.server

@INTLTOOL_SERVER_RULE@

EXTRA_DIST = Accessibility_Registry.server.in $(serverinfo_DATA)
