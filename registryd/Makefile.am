server_dot_in = Accessibility_Registry.server.in

$(server_dot_in): $(server_dot_in).in
	sed -e "s|\@REGISTRYD_PATH\@|$(libexecdir)|" $< > $@ 

libexec_PROGRAMS = at-spi-registryd

INCLUDES = -I$(top_srcdir)          \
           -I$(top_builddir)        \
           -I$(top_builddir)/libspi \
           -I$(top_srcdir)/libspi   \
           $(REGISTRYD_CFLAGS)

at_spi_registryd_SOURCES =		\
	desktop.c		\
	desktop.h		\
	deviceeventcontroller.c	\
	deviceeventcontroller.h	\
	registry-main.c		\
	registry.c		\
	registry.h

LDADD = ../libspi/libspi.la $(X_LIBS) $(XTST_LIBS) $(REGISTRYD_LIBS) 

serverinfodir = $(libdir)/bonobo/servers
serverinfo_DATA = Accessibility_Registry.server

@INTLTOOL_SERVER_RULE@

EXTRA_DIST = Accessibility_Registry.server.in.in $(serverinfo_DATA)
