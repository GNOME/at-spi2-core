AC_INIT([at-spi2-core], [0.3.90], [accessibility-atspi@lists.linux-foundation.org])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])

AT_SPI_CORE_MAJOR_VERSION=0
AT_SPI_CORE_MINOR_VERSION=1
AT_SPI_CORE_MICRO_VERSION=3
AT_SPI_CORE_VERSION="$AT_SPI_CORE_MAJOR_VERSION.$AT_SPI_CORE_MINOR_VERSION"
AC_SUBST(AT_SPI_CORE_VERSION)

LT_RELEASE=$AT_SPI_CORE_VERSION
LT_CURRENT=0
LT_REVISION=1
LT_AGE=0
LT_VERSION_INFO='-version-info ${LT_CURRENT}:${LT_REVISION}:${LT_AGE}'
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AM_INIT_AUTOMAKE([-Wall foreign])

AC_PROG_CC
AM_DISABLE_STATIC
AM_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

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

AC_CHECK_LIB(dl, dlopen)

AC_PATH_XTRA

if test x$no_x = xyes ; then
	AC_MSG_ERROR([X development libraries not found])
else
	X_LIBS="$X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS"
fi
AC_SUBST(X_LIBS)

#LIBS="$LIBS $X_LIBS"
AC_CHECK_LIB(Xtst, XTestFakeKeyEvent, XTST_LIBS=-lXtst,[
	AC_MSG_ERROR([Couldn't find the Xtst library. Check config.log])],
	-lX11 -lXext)
AC_SUBST(XTST_LIBS)

have_xkb=
AC_CHECK_LIB(X11, XkbQueryExtension, have_xkb="maybe")
if test "x$have_xkb" = "xmaybe"; then 
	AC_CHECK_HEADER(X11/XKBlib.h, have_xkb=yes)
fi
if test "x$have_xkb" = "xyes"; then
	AC_MSG_RESULT(yes)
	AC_DEFINE([HAVE_XKB], [], Xkb is present)
fi

have_xinput=
AC_CHECK_LIB(Xi, XOpenDevice, XINPUT_LIBS=-lXi,,-lXext)
if test "x$XINPUT_LIBS" = x; then
	AC_MSG_ERROR(Couldn't find the XInput library. Check config.log for details)
fi			       
AC_CHECK_HEADER(X11/extensions/XInput.h, have_xinput=yes)
AC_SUBST(XINPUT_LIBS)

AC_ARG_ENABLE(xevie, [--enable-xevie  Build with XEViE support [default=yes]], enable_xevie="$enableval", enable_xevie=yes)

if test x$enable_xevie = xyes ; then
	have_xevie=
	AC_CHECK_LIB(Xext, XevieStart, have_xevie="yes")

	if test "x$have_xevie" = "xyes"; then
		XEVIE_LIBS="-lXext"
		AC_DEFINE([HAVE_XEVIE], [], Xevie is present)
	else
	   AC_CHECK_LIB(Xevie, XevieQueryVersion, have_xevie="maybe",,-lXevie -lXext)
	   if test "x$have_xevie" = "xmaybe"; then
	      AC_CHECK_HEADER(X11/extensions/Xevie.h, have_xevie=yes, [], [#include <X11/Xlib.h>])
	      if test "x$have_xevie" = "xyes"; then
	      	      XEVIE_LIBS="-lXext -lXevie"	       
		      AC_DEFINE([HAVE_XEVIE], [], Xevie is present)
	      fi
	   fi	
	fi
	AC_SUBST(XEVIE_LIBS)
fi

AC_ARG_VAR([DEFAULT_ATSPI_INTROSPECTION_PATH],
	   [Set the default path for the install ofDBus introspection XML
	    relative to the pkgdatadir.])
if test -z "$DEFAULT_ATSPI_INTROSPECTION_PATH"; then
	  DEFAULT_ATSPI_INTROSPECTION_PATH=at-spi2/dbus
fi

AS_AC_EXPAND(EXPANDED_BINDIR, "$bindir")
AC_ARG_WITH(dbus_daemondir, AS_HELP_STRING([--with-dbus-daemondir=[dirname]],[Directory where the DBUS daemon is installed]))
if test -z "$with_dbus_daemondir" ; then
    DBUS_DAEMONDIR=$EXPANDED_BINDIR
else
    DBUS_DAEMONDIR=$with_dbus_daemondir
fi
AC_SUBST(DBUS_DAEMONDIR)
AC_DEFINE_UNQUOTED(DBUS_DAEMONDIR,"$DBUS_DAEMONDIR", [Directory for installing the DBUS daemon])

AC_ARG_WITH(dbus-services,
           [AC_HELP_STRING([--with-dbus-services=<dir>],
           [where D-BUS services directory is])])
if ! test -z "$with_dbus_services" ; then
           DBUS_SERVICES_DIR="$with_dbus_services"
else
           DBUS_SERVICES_DIR="$datadir/dbus-1/services"
fi
AC_SUBST(DBUS_SERVICES_DIR)

AC_SUBST(LIBTOOL_EXPORT_OPTIONS)

AC_ARG_ENABLE(relocate, [  --enable-relocate  Relocate to coexist with CORBA [default=no]], enable_relocate="$enableval", enable_relocate=no)
if test x$enable_relocate = xyes ; then
	AC_DEFINE(RELOCATE, , [Relocate to coexist with CORBA])
fi
AM_CONDITIONAL(RELOCATE, test x$enable_relocate = xyes)

AC_CONFIG_FILES([Makefile
		 xml/Makefile
		 registryd/Makefile])

AC_OUTPUT
