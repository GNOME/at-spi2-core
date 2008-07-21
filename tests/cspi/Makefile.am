noinst_PROGRAMS = key-listener-test keysynth-test simple-at

key_listener_test_SOURCES = key-listener-test.c 
keysynth_test_SOURCES = keysynth-test.c 
simple_at_SOURCES = simple-at.c 

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
	   $(DBUS_CFLAGS)	     \
	   $(GLIB_CFLAGS)	     \
	   $(GTK_CFLAGS)	     \
	   $(ATK_CFLAGS)	     \
	   $(DEBUG_CFLAGS)

LDADD = $(top_builddir)/spi-common/libspicommon.la 			\
	$(top_builddir)/cspi/libcspi.la 				\
	$(TESTS_LIBS) $(X_LIBS) $(XINPUT_LIBS) $(ATK_LIBS) $(GTK_LIBS)
