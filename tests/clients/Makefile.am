EXTRA_DIST = \
	__init__.py		\
	accessibleobject.py	\
	component.py		\
	testutil.py

CLEANFILES = *.pyc

noinst_PROGRAMS = simple-at

simple_at_SOURCES = simple-at.c 

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
	   $(DBUS_CFLAGS)	     \
	   $(GLIB_CFLAGS)	     \
	   $(ATK_CFLAGS)	     \
	   $(DEBUG_CFLAGS)

#LDADD = ../spi-common/libspicommon.la ../cspi/libcspi.la ../login-helper/libloginhelper.la $(TESTS_LIBS) $(X_LIBS) $(XINPUT_LIBS)
LDADD = ../../spi-common/libspicommon.la ../../cspi/libcspi.la $(TESTS_LIBS) $(X_LIBS) $(XINPUT_LIBS) $(ATK_LIBS)
