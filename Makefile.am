SUBDIRS = docs idl libspi registryd atk-bridge cspi test

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libspi-1.0.pc cspi-1.0.pc

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

EXTRA_DIST = \
	intltool-extract.in \
	intltool-merge.in   \
	intltool-update.in  \
	cspi-1.0.pc.in \
	cspi-1.0-uninstalled.pc.in \
	libspi-1.0.pc.in \
	libspi-1.0-uninstalled.pc.in
