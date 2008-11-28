
AC_INIT([at-spi], [1.9.0], [accessibility-atspi@lists.linux-foundation.org])
AC_CONFIG_AUX_DIR(config)

AT_SPI_MAJOR_VERSION=1
AT_SPI_MINOR_VERSION=23
AT_SPI_MICRO_VERSION=3
AT_SPI_INTERFACE_AGE=0
AT_SPI_BINARY_AGE=0
AT_SPI_VERSION="$AT_SPI_MAJOR_VERSION.$AT_SPI_MINOR_VERSION.$AT_SPI_MICRO_VERSION"
AC_SUBST(AT_SPI_MAJOR_VERSION)
AC_SUBST(AT_SPI_MINOR_VERSION)
AC_SUBST(AT_SPI_MICRO_VERSION)
AC_SUBST(AT_SPI_INTERFACE_AGE)
AC_SUBST(AT_SPI_BINARY_AGE)

# libtool versioning
LT_RELEASE=$AT_SPI_MAJOR_VERSION.$AT_SPI_MINOR_VERSION
LT_CURRENT=10
LT_REVISION=11
LT_AGE=10
LT_VERSION_INFO='-version-info ${LT_CURRENT}:${LT_REVISION}:${LT_AGE}'
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AM_INIT_AUTOMAKE([-Wall foreign])

dnl translation of any at-spi strings: used only for .server
dnl IT_PROG_INTLTOOL([0.35.0])

AC_PROG_CC
AM_PATH_PYTHON(2.4)
AM_DISABLE_STATIC
AM_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

GETTEXT_PACKAGE="${PACKAGE}"
AC_SUBST(GETTEXT_PACKAGE)
dnl internationalization support; uncomment if translatable strings are reintroduced
AM_GLIB_GNU_GETTEXT

# AM_GLIB_GNU_GETTEXT above subst. $DATADIRNAME
# this is the directory where the *.{mo,gmo} files are installed
localedir='${prefix}/${DATADIRNAME}/locale'
AC_SUBST(localedir)

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

PKG_CHECK_MODULES(LIB_XML, [libxml-2.0 >= 2.0.0])
AC_SUBST(LIB_XML_LIBS)
AC_SUBST(LIB_XML_CFLAGS)

PKG_CHECK_MODULES(GOBJ, [gobject-2.0 >= 2.0.0])
AC_SUBST(GOBJ_LIBS)
AC_SUBST(GOBJ_CFLAGS)

PKG_CHECK_MODULES(GMODULE, [gmodule-2.0 >= 2.0.0])	
AC_SUBST(GMODULE_LIBS)
AC_SUBST(GMODULE_CFLAGS)

PKG_CHECK_MODULES(ATK, [atk >= 1.17.0])
AC_SUBST(ATK_LIBS)
AC_SUBST(ATK_CFLAGS)

PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.10.0])
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

PKG_CHECK_MODULES(GDK, [gdk-2.0 >= 2.0.0])
AC_SUBST(GDK_LIBS)
AC_SUBST(GDK_CFLAGS)

PKG_CHECK_MODULES(GAIL, [gail >= 1.9.0])	
AC_SUBST(GAIL_LIBS)
AC_SUBST(GAIL_CFLAGS)

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
	  DEFAULT_ATSPI_INTROSPECTION_PATH=dbus
fi

AC_ARG_VAR([GTK_MODULE_DIR],
	   [Where to place the atk-adaptor gtk module])
if test -z "$GTK_MODULE_DIR"; then
	  GTK_MODULE_DIR=gtk-2.0/modules
fi

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

#libtool option to strip symbols starting with cspi
#
LIBTOOL_EXPORT_OPTIONS='-export-symbols-regex "^[[^cspi]].*"'
AC_SUBST(LIBTOOL_EXPORT_OPTIONS)

AC_CONFIG_FILES([Makefile
		 libspi-1.0.pc
		 libspi-1.0-uninstalled.pc
		 libloginhelper-1.0.pc
		 cspi-1.0.pc
		 cspi-1.0-uninstalled.pc
		 xml/Makefile
		 tests/Makefile
		 tools/Makefile
		 droute/Makefile
		 spi-common/Makefile
		 registryd/Makefile
		 atk-adaptor/Makefile
		 login-helper/Makefile
		 tests/dummyatk/Makefile
		 tests/data/Makefile
		 tests/pyatspi/Makefile
		 tests/pyatspi/pasytest/Makefile
		 tests/apps/Makefile
		 tests/cspi/Makefile
		 dbind/Makefile
		 cspi/Makefile
		 pyatspi/Makefile
		 dbind-config.h])

AC_OUTPUT
