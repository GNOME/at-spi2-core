gtkmoduledir = $(libdir)/$(GTK_MODULE_DIR)
gtkmodule_LTLIBRARIES = libspiatk.la

libspiatk_la_CFLAGS = $(DBUS_GLIB_CFLAGS) $(ATK_CFLAGS) -I$(top_srcdir)

libspiatk_la_LDFLAGS = -no-undefined       \
		       -module             \
		       -avoid-version      \
		       -rpath $(gtkmoduledir)

libspiatk_la_LIBADD = $(DBUS_GLIB_LIBS)			\
		      $(ATK_LIBS)			\
		      $(top_builddir)/droute/libdroute.la\
		      $(top_builddir)/spi-common/libspicommon.la

libspiatk_la_SOURCES =		\
	accessible.h		\
	accessible.c		\
	action.c                \
	application.c		\
	bridge.c		\
	bridge.h		\
	collection.c 		\
	component.c		\
	document.c		\
	editabletext.c		\
	event.c			\
	hyperlink.c		\
	hypertext.c		\
	atk-dbus.c		\
	atk-dbus.h		\
	image.c			\
	selection.c		\
	spi-private.h		\
	table.c			\
	text.c			\
	tree.c                  \
	util.c                  \
	value.c                 \
	introspectable.c 
