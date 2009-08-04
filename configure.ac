AC_INIT([at-spi2-core], [0.1.0], [accessibility-atspi@lists.linux-foundation.org])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])

AT_SPI_CORE_MAJOR_VERSION=0
AT_SPI_CORE_MINOR_VERSION=1
AT_SPI_CORE_VERSION="$AT_SPI_CORE_MAJOR_VERSION.$AT_SPI_CORE_MINOR_VERSION"
AC_SUBST(AT_SPI_CORE_VERSION)

DBIND_MAJOR_VERSION=0
DBIND_MINOR_VERSION=1
DBIND_VERSION="$DBIND_MAJOR_VERSION.$DBIND_MINOR_VERSION"
AC_SUBST(DBIND_VERSION)

DROUTE_MAJOR_VERSION=0
DROUTE_MINOR_VERSION=1
DROUTE_VERSION="$DROUTE_MAJOR_VERSION.$DROUTE_MINOR_VERSION"
AC_SUBST(DROUTE_VERSION) 

LT_RELEASE=$AT_SPI_CORE_VERSION
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

PKG_CHECK_MODULES(GMODULE, [gmodule-2.0 >= 2.0.0])	
AC_SUBST(GMODULE_LIBS)
AC_SUBST(GMODULE_CFLAGS)

PKG_CHECK_MODULES(GDK, [gdk-2.0 >= 2.0.0])
AC_SUBST(GDK_LIBS)
AC_SUBST(GDK_CFLAGS)

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

dnl Allow disabling SMlib
AC_ARG_ENABLE(sm, [  --enable-sm  Enable session management support [default=yes]], enable_sm="$enableval", enable_sm=yes)

if test x$enable_sm = xyes ; then
	have_sm=
	AC_CHECK_LIB(SM, SmcSaveYourselfDone, have_sm="yes")
	if test x$have_sm = xyes ; then
		SM_LIBS="-lSM -lICE"
		AC_DEFINE(HAVE_SM, , [Building with SM support])
	fi
	AC_SUBST(SM_LIBS)
fi


AC_ARG_VAR([DEFAULT_ATSPI_INTROSPECTION_PATH],
	   [Set the default path for the install ofDBus introspection XML
	    relative to the pkgdatadir.])
if test -z "$DEFAULT_ATSPI_INTROSPECTION_PATH"; then
	  DEFAULT_ATSPI_INTROSPECTION_PATH=dbus
fi

AC_ARG_WITH(dbus-services,
           [AC_HELP_STRING([--with-dbus-services=<dir>],
           [where D-BUS services directory is])])
if ! test -z "$with_dbus_services" ; then
           DBUS_SERVICES_DIR="$with_dbus_services"
else
           DBUS_SERVICES_DIR="$datadir/dbus-1/services"
fi
AC_SUBST(DBUS_SERVICES_DIR)

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

AC_SUBST(LIBTOOL_EXPORT_OPTIONS)

AC_CONFIG_FILES([Makefile
                 pkgconfig/dbind-0.1.pc
                 pkgconfig/dbind-0.1-uninstalled.pc
                 pkgconfig/droute-0.1.pc
                 pkgconfig/droute-0.1-uninstalled.pc
                 pkgconfig/Makefile
		 xml/Makefile
		 tools/Makefile
		 droute/Makefile
		 registryd/Makefile
		 registryd/at-spi-registryd.desktop.in
		 dbind/Makefile
		 dbind/dbind-config.h])

AC_OUTPUT
