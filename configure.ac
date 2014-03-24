AC_INIT([at-spi2-atk], [2.12.0], [accessibility-atspi@lists.linux-foundation.org])
AC_CONFIG_AUX_DIR(config)

AT_SPI_ATK_MAJOR_VERSION=0
AT_SPI_ATK_MINOR_VERSION=3
AT_SPI_ATK_INTERFACE_AGE=0
AT_SPI_ATK_BINARY_AGE=0
AT_SPI_ATK_VERSION="$AT_SPI_MAJOR_VERSION.$AT_SPI_MINOR_VERSION"
AC_SUBST(AT_SPI_ATK_VERSION)

# libtool versioning
LT_RELEASE=$AT_SPI_MAJOR_VERSION.$AT_SPI_MINOR_VERSION
LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
LT_VERSION_INFO='-version-info ${LT_CURRENT}:${LT_REVISION}:${LT_AGE}'
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AM_INIT_AUTOMAKE([-Wall foreign no-dist-gzip dist-xz])

# Enable silent build when available (Automake 1.11)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_DISABLE_STATIC
AM_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

GETTEXT_PACKAGE="${PACKAGE}"
AC_SUBST(GETTEXT_PACKAGE)

AC_CONFIG_HEADERS([config.h])

PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.5])
AC_SUBST(DBUS_LIBS)
AC_SUBST(DBUS_CFLAGS)

PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.32.0])
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

PKG_CHECK_MODULES(GOBJ, [gobject-2.0 >= 2.0.0])
AC_SUBST(GOBJ_LIBS)
AC_SUBST(GOBJ_CFLAGS)

PKG_CHECK_MODULES(GMODULE, [gmodule-2.0 >= 2.0.0])	
AC_SUBST(GMODULE_LIBS)
AC_SUBST(GMODULE_CFLAGS)

PKG_CHECK_MODULES(ATK, [atk >= 2.11.90])
AC_SUBST(ATK_LIBS)
AC_SUBST(ATK_CFLAGS)

PKG_CHECK_MODULES(ATSPI, [atspi-2 >= 2.11.2])
AC_SUBST(ATSPI_LIBS)
AC_SUBST(ATSPI_CFLAGS)

GLIB_GSETTINGS

AC_ARG_ENABLE(p2p, [  --enable-p2p  Allow peer-to-peer DBus connections [default=yes]], enable_p2p="$enableval", enable_p2p=yes)

#libtool option to strip symbols starting with cspi
LIBTOOL_EXPORT_OPTIONS='-export-symbols-regex "^[[^cspi]].*"'
AC_SUBST(LIBTOOL_EXPORT_OPTIONS)

# Extra libraries for sockets added by Willie Walker
# based upon how SunStudio libraries work.
#
if test "$GCC" = yes; then
    EXTRA_SOCKET_LIBS=""
else
    EXTRA_SOCKET_LIBS="-lsocket -lnsl"
fi
AC_SUBST(EXTRA_SOCKET_LIBS)

dnl find sizes & alignments
orig_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $DBUS_CFLAGS"
CPPFLAGS=$orig_CPPFLAGS 

if test "x$GCC" = xyes; then
	CFLAGS="$CFLAGS -Werror-implicit-function-declaration"
fi

if test "x$enable_p2p" = "xno"; then
	P2P_CFLAGS=-DDISABLE_P2P
fi

AC_SUBST(P2P_CFLAGS)

AC_CONFIG_FILES([Makefile
		 atk-bridge-2.0.pc
		 droute/Makefile
		 atk-adaptor/Makefile
		 atk-adaptor/adaptors/Makefile
		 atk-adaptor/gtk-2.0/Makefile
		])

AC_OUTPUT
