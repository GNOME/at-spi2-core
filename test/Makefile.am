NULL=

noinst_PROGRAMS = test-simple at app simple-at stress-test keysynth-demo accessx-gui key-listener-test event-listener-test screen-review-test visual-bell

key_listener_test_SOURCES = key-listener-test.c

stress_test_SOURCES = stress-test.c

visual_bell_SOURCES = visual-bell.c

event_listener_test_SOURCES = event-listener-test.c

screen_review_test_SOURCES = screen-review-test.c

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

LDADD = ../libspi/libspi.la ../cspi/libcspi.la $(TESTS_LIBS) $(X_LIBS)

TESTS = test-simple
