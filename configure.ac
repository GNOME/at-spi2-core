AC_INIT([at-spi2-atk], [0.3.90], [accessibility-atspi@lists.linux-foundation.org])
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

AM_INIT_AUTOMAKE([-Wall foreign])

dnl translation of any at-spi strings: used only for .server
IT_PROG_INTLTOOL([0.40.0])

AC_PROG_CC
AM_DISABLE_STATIC
AM_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

GETTEXT_PACKAGE="${PACKAGE}"
AC_SUBST(GETTEXT_PACKAGE)

AC_CONFIG_HEADERS([config.h])

PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.0])
AC_SUBST(DBUS_LIBS)
AC_SUBST(DBUS_CFLAGS)

PKG_CHECK_MODULES(GLIB, [glib-2.0])
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

PKG_CHECK_MODULES(DBUS_GLIB, [dbus-glib-1 >= 0.7.0])
AC_SUBST(DBUS_GLIB_LIBS)
AC_SUBST(DBUS_GLIB_CFLAGS)

PKG_CHECK_MODULES(GOBJ, [gobject-2.0 >= 2.0.0])
AC_SUBST(GOBJ_LIBS)
AC_SUBST(GOBJ_CFLAGS)

PKG_CHECK_MODULES(GMODULE, [gmodule-2.0 >= 2.0.0])	
AC_SUBST(GMODULE_LIBS)
AC_SUBST(GMODULE_CFLAGS)

PKG_CHECK_MODULES(ATK, [atk >= 1.29.3])
AC_SUBST(ATK_LIBS)
AC_SUBST(ATK_CFLAGS)

dnl GConf checks
AC_PATH_PROG(GCONFTOOL, gconftool-2)
AM_GCONF_SOURCE_2

AC_PATH_XTRA

if test x$no_x = xyes ; then
	AC_MSG_ERROR([X development libraries not found])
else
	X_LIBS="$X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS"
fi
AC_SUBST(X_LIBS)

AC_ARG_ENABLE(relocate, [  --enable-relocate  Relocate to coexist with CORBA [default=yes]], enable_relocate="$enableval", enable_relocate=yes)
if test x$enable_relocate = xyes ; then
	AC_DEFINE(RELOCATE, , [Relocate to coexist with CORBA])
fi
AM_CONDITIONAL(RELOCATE, test x$enable_relocate = xyes)

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
DBIND_CHECK_ALIGNOF(char)
DBIND_CHECK_ALIGNOF(dbus_bool_t)
DBIND_CHECK_ALIGNOF(dbus_int16_t)
DBIND_CHECK_ALIGNOF(dbus_int32_t)
DBIND_CHECK_ALIGNOF(dbus_int64_t)
DBIND_CHECK_ALIGNOF(double)
DBIND_CHECK_ALIGNOF(dbind_pointer)
DBIND_CHECK_ALIGNOF(dbind_struct)
CPPFLAGS=$orig_CPPFLAGS 

AC_CONFIG_FILES([Makefile
		 dbind/dbind-config.h
		 dbind/Makefile
		 droute/Makefile
		 common/Makefile
		 atk-adaptor/Makefile
		 atk-adaptor/adaptors/Makefile
		 atk-adaptor/gtk-2.0/Makefile
		 atk-adaptor/gtk-3.0/Makefile
		 po/Makefile.in
		])

AC_OUTPUT
