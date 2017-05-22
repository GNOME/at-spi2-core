# Process this file with automake to create Makefile.in.
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = atk tests docs po win32

meson_dist_files = \
	meson_options.txt \
	meson.build \
	atk/meson.build \
	docs/meson.build \
	docs/xml/meson.build \
	docs/xml/gtkdocentities.ent.in \
	po/meson.build \
	tests/meson.build \
	$()

EXTRA_DIST = 			\
	MAINTAINERS		\
	atk.pc.in		\
	config.h.win32		\
	$(meson_dist_files)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atk.pc

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-introspection

DISTCLEANFILES = atk.pc
