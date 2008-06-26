EXTRA_DIST = \
	__init__.py		\
	accessibleobject.py	\
	component.py		\
	testutil.py

CLEANFILES = *.pyc

noinst_PROGRAMS = keysynth-test simple-at

keysynth_test_SOURCES = keysynth-test.c 
simple_at_SOURCES = simple-at.c 

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
	   $(DBUS_CFLAGS)	     \
	   $(GLIB_CFLAGS)	     \
	   $(GTK_CFLAGS)	     \
	   $(ATK_CFLAGS)	     \
	   $(DEBUG_CFLAGS)

#LDADD = ../spi-common/libspicommon.la ../cspi/libcspi.la ../login-helper/libloginhelper.la $(TESTS_LIBS) $(X_LIBS) $(XINPUT_LIBS)
LDADD = ../../spi-common/libspicommon.la ../../cspi/libcspi.la $(TESTS_LIBS) $(X_LIBS) $(XINPUT_LIBS) $(ATK_LIBS) $(GTK_LIBS)
