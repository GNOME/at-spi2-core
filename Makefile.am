# Process this file with automake to create Makefile.in.
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = atk tests docs po win32

EXTRA_DIST = 			\
	MAINTAINERS		\
	atk.pc.in		\
	atk-zip.sh.in   	\
	m4/introspection.m4	\
	config.h.win32

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atk.pc

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-introspection

DISTCLEANFILES =		\
	atk.pc			\
	atk-zip.sh
