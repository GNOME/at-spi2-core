if RELOCATE
    gtkmoduledir = $(libdir)/gtk-2.0/modules/at-spi-dbus/modules
else
    gtkmoduledir = $(libdir)/gtk-2.0/modules
endif
gtkmodule_LTLIBRARIES = libatk-bridge.la

libatk_bridge_la_CFLAGS = $(DBUS_GLIB_CFLAGS) \
		          $(ATK_CFLAGS)       \
		          $(DBIND_CFLAGS)     \
		          $(DROUTE_CFLAGS)    \
		          -I$(top_srcdir)     \
		          -DATSPI_INTROSPECTION_PATH=\"$(pkgdatadir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"

libatk_bridge_la_LDFLAGS = -no-undefined  \
		           -module        \
		           -avoid-version \
		           -rpath $(gtkmoduledir)

libatk_bridge_la_LIBADD = $(DBUS_GLIB_LIBS) \
		          $(ATK_LIBS)       \
		          $(DBIND_LIBS)	    \
		          $(DROUTE_LIBS)    \
			  $(X_LIBS)         \
		          $(top_builddir)/droute/libdroute.la \
		          $(top_builddir)/dbind/libdbind.la \
		          $(top_builddir)/common/libspicommon.la

libatk_bridge_la_SOURCES =	\
	accessible-adaptor.c	\
	accessible-marshaller.c	\
	accessible-marshaller.h	\
	accessible-register.c	\
	accessible-register.h	\
	action-adaptor.c	\
	adaptors.h		\
	application-adaptor.c	\
	bridge.c		\
	bridge.h		\
	collection-adaptor.c	\
	component-adaptor.c	\
	document-adaptor.c	\
	editabletext-adaptor.c	\
	event.c			\
	event.h			\
	hyperlink-adaptor.c	\
	hypertext-adaptor.c	\
	image-adaptor.c		\
	selection-adaptor.c	\
	table-adaptor.c		\
	text-adaptor.c		\
	tree-adaptor.c		\
	tree-adaptor.h		\
	value-adaptor.c

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
