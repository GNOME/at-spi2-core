gtkmoduledir = $(libdir)/$(GTK_MODULE_DIR)
gtkmodule_LTLIBRARIES = libspiatk.la

libspiatk_la_CFLAGS = $(DBUS_GLIB_CFLAGS)\
		      $(ATK_CFLAGS)\
		      -I$(top_srcdir)\
		      -I$(top_srcdir)/atk-adaptor/adaptors\
		      -DATSPI_INTROSPECTION_PATH=\"$(pkgdatadir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)\"

libspiatk_la_LDFLAGS = -no-undefined       \
		       -module             \
		       -avoid-version      \
		       -rpath $(gtkmoduledir)

libspiatk_la_LIBADD = $(DBUS_GLIB_LIBS)			\
		      $(ATK_LIBS)			\
		      $(top_builddir)/droute/libdroute.la\
		      $(top_builddir)/spi-common/libspicommon.la

libspiatk_la_SOURCES =		\
	accessible-adaptor.c	\
	accessible-marshaller.c	\
	accessible-marshaller.h	\
	accessible-register.c	\
	accessible-register.h	\
	action-adaptor.c	\
	adaptor.h		\
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
