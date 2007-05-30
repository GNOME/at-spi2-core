SUBDIRS = docs idl libspi login-helper registryd atk-bridge cspi pyatspi test po

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libspi-1.0.pc cspi-1.0.pc libloginhelper-1.0.pc

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

EXTRA_DIST = \
	MAINTAINERS \
	intltool-extract.in \
	intltool-merge.in   \
	intltool-update.in  \
	cspi-1.0.pc.in \
	cspi-1.0-uninstalled.pc.in \
	libloginhelper-1.0.pc.in \
	libspi-1.0.pc.in \
	libspi-1.0-uninstalled.pc.in

DISTCLEANFILES = intltool-extract \
	intltool-merge	\
	intltool-update
