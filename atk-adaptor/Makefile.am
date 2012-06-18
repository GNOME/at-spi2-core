SUBDIRS= adaptors . gtk-2.0

lib_LTLIBRARIES = libatk-bridge-2.0.la

libatk_bridge_2_0_la_CFLAGS = \
	$(DBUS_CFLAGS)    \
	$(ATK_CFLAGS)     \
	$(ATSPI_CFLAGS)   \
	-I$(top_srcdir)   \
	-I$(top_srcdir)/atk-adaptor/adaptors \
	$(P2P_CFLAGS)

atkbridgeincludedir = $(pkgincludedir)/2.0/
atkbridgeinclude_HEADERS = atk-bridge.h

libatk_bridge_2_0_la_SOURCES =  \
        accessible-leasing.c    \
        accessible-leasing.h    \
        accessible-cache.c      \
        accessible-cache.h      \
	accessible-register.c   \
	accessible-register.h   \
	accessible-stateset.c   \
	accessible-stateset.h   \
	bitarray.h              \
	introspection.c         \
	introspection.h         \
	bridge.c                \
	bridge.h                \
	object.c                \
	object.h                \
	event.c                 \
	event.h                 \
	spi-dbus.c              \
	spi-dbus.h		\
	atk-bridge.h

libatk_bridge_2_0_la_LIBADD = \
	$(DBUS_LIBS)          \
	$(GMODULE_LIBS)       \
	$(ATK_LIBS)           \
	$(ATSPI_LIBS)         \
	$(top_builddir)/droute/libdroute.la \
	$(top_builddir)/atk-adaptor/adaptors/libatk-bridge-adaptors.la

libatk_bridge_2_0_la_LDFLAGS = \
	$(LT_VERSION_INFO)                          \
	-export-symbols $(srcdir)/atkbridge.symbols \
	-no-undefined                               \
	$(AM_LDFLAGS)

EXTRA_DIST = Makefile.include \
	atkbridge.symbols
