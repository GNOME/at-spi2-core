NULL=

noinst_PROGRAMS = test-simple at app simple-at stress-test keysynth-demo key-listener-test event-listener-test window-listener-test screen-review-test keypad-test keysynth-test

keysynth_test_SOURCES = keysynth-test.c

stress_test_SOURCES = stress-test.c

window_listener_test_SOURCES = window-listener-test.c

keypad_test_SOURCES = keypad-test.c

key_listener_test_SOURCES = key-listener-test.c

event_listener_test_SOURCES = event-listener-test.c

screen_review_test_SOURCES = screen-review-test.c

at_SOURCES = at.c 

app_SOURCES = app.c

simple_at_SOURCES = simple-at.c 

keysynth_demo_SOURCES = keysynth-demo.c 

test_simple_SOURCES = test-simple.c 

DEBUG_CFLAGS=-DSPI_DEBUG

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
           -I$(top_srcdir)/libspi    \
           -I$(top_builddir)/libspi  \
	   -I$(top_srcdir)/cspi	     \
	   -I$(top_builddir)/cspi    \
	   $(TESTS_CFLAGS)	     \
	   $(DEBUG_CFLAGS)

LDADD = ../libspi/libspi.la ../cspi/libcspi.la $(TESTS_LIBS) $(X_LIBS) $(XINPUT_LIBS)

TESTS = test-simple
