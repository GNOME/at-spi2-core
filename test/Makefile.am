NULL=

noinst_PROGRAMS = test-simple at app simple-at keysynth-demo accessx-gui wm_test

wm_test_SOURCES = wm_mode_test.c

at_SOURCES = at.c 

app_SOURCES = app.c

simple_at_SOURCES = simple-at.c 

keysynth_demo_SOURCES = keysynth-demo.c 

accessx_gui_SOURCES = accessx-gui.c 

test_simple_SOURCES = test-simple.c 

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
           -I$(top_srcdir)/libspi    \
           -I$(top_builddir)/libspi  \
	   -I$(top_srcdir)/cspi	     \
	   -I$(top_builddir)/cspi    \
	   $(TESTS_CFLAGS)

DEBUG_CFLAGS=-DSPI_DEBUG

CFLAGS += $(TESTS_CFLAGS) $(DEBUG_CFLAGS)

LDADD = ../util/libat-util.la ../libspi/libspi.la ../cspi/libcspi.la $(TESTS_LIBS) $(X_LIBS)

TESTS = test-simple
