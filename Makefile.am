# Process this file with automake to create Makefile.in.
ACLOCAL_AMFLAGS = -I m4

SUBDIRS=atk tests docs po

EXTRA_DIST = 			\
	MAINTAINERS		\
	atk-uninstalled.pc.in	\
	atk.pc.in		\
	atk.spec.in		\
	atk.spec		\
	atk-zip.sh.in   \
    m4/introspection.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atk.pc

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-introspection

DISTCLEANFILES =		\
	atk-uninstalled.pc	\
	atk.pc			\
	atk.pc.spec		\
	atk-zip.sh
