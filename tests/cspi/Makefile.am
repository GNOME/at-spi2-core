noinst_PROGRAMS = key-listener-test keysynth-test simple-at test-simple

key_listener_test_SOURCES = key-listener-test.c 
keysynth_test_SOURCES = keysynth-test.c 
simple_at_SOURCES = simple-at.c 
test_simple_SOURCES = test-simple.c 

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
	   $(DBUS_CFLAGS)	     \
	   $(GLIB_CFLAGS)	     \
	   $(GTK_CFLAGS)	     \
	   $(ATK_CFLAGS)	     \
           $(DBIND_CFLAGS)           \
	   $(DEBUG_CFLAGS)

LDADD = $(top_builddir)/common/libspicommon.la 			\
	$(top_builddir)/cspi/libcspi.la 			\
	$(TESTS_LIBS) $(XINPUT_LIBS) $(ATK_LIBS) $(GTK_LIBS) $(DBIND_LIBS) @EXTRA_SOCKET_LIBS@
