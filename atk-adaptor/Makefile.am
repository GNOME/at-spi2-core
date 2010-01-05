SUBDIRS=adaptors

if RELOCATE
    gtkmoduledir = $(libdir)/gtk-2.0/modules/at-spi-dbus/modules
else
    gtkmoduledir = $(libdir)/gtk-2.0/modules
endif
gtkmodule_LTLIBRARIES = libatk-bridge.la

libatk_bridge_la_CFLAGS = $(DBUS_GLIB_CFLAGS) \
		          $(ATK_CFLAGS)       \
		          -I$(top_srcdir)     \
		          -I$(top_srcdir)/atk-adaptor/adaptors \
		          -DATSPI_INTROSPECTION_PATH=\"$(datarootdir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"

libatk_bridge_la_LDFLAGS = -no-undefined  \
		           -module        \
		           -avoid-version \
		           -rpath $(gtkmoduledir)

libatk_bridge_la_LIBADD = $(DBUS_GLIB_LIBS) \
		          $(ATK_LIBS)       \
			  $(X_LIBS)         \
		          $(top_builddir)/droute/libdroute.la \
		          $(top_builddir)/common/libspicommon.la \
			  $(top_builddir)/atk-adaptor/adaptors/libatk-bridge-adaptors.la

libatk_bridge_la_SOURCES =	\
        accessible-leasing.c    \
        accessible-leasing.h    \
        accessible-cache.c      \
        accessible-cache.h      \
	accessible-register.c	\
	accessible-register.h	\
	bridge.c		\
	bridge.h		\
	object.c		\
	object.h		\
	event.c			\
	event.h

if RELOCATE
gtkpathdir = $(gtkmoduledir)/..
gnomeautostartdir = $(datadir)/gnome/autostart
gnomeautostart_DATA = atk-bridge.desktop
EXTRA_DIST = atk-bridge.desktop.in
CLEANFILES = $(gnome_autostart_DATA)

%.desktop: %.desktop.in Makefile.am
	sed -e "s,\@gtkpathdir\@,$(gtkpathdir)," \
	< $< > $@
endif
