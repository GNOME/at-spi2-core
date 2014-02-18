noinst_LTLIBRARIES = libatk-bridge-adaptors.la

libatk_bridge_adaptors_la_CFLAGS =\
			  $(DBUS_CFLAGS) \
		          $(ATK_CFLAGS)       \
		          $(ATSPI_CFLAGS)       \
		          -I$(top_srcdir)     \
		          -I$(top_srcdir)/atk-adaptor \
	$(P2P_CFLAGS)

libatk_bridge_adaptors_la_LIBADD =\
			  $(DBUS_LIBS) \
		          $(ATK_LIBS)       \
		          $(ATSPI_LIBS)

libatk_bridge_adaptors_la_SOURCES =\
	accessible-adaptor.c	\
	action-adaptor.c	\
	adaptors.h		\
	application-adaptor.c	\
	cache-adaptor.c		\
	collection-adaptor.c	\
	component-adaptor.c	\
	document-adaptor.c	\
	editabletext-adaptor.c	\
	hyperlink-adaptor.c	\
	hypertext-adaptor.c	\
	image-adaptor.c		\
	selection-adaptor.c	\
	socket-adaptor.c	\
	table-adaptor.c		\
	table-cell-adaptor.c		\
	text-adaptor.c		\
	value-adaptor.c
