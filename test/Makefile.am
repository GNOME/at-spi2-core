NULL=

noinst_PROGRAMS = test-simple at app simple-at stress-test keysynth-demo key-listener-test event-listener-test window-listener-test screen-review-test keypad-test keysynth-test login-helper-client-test login-helper-server-test

login_helper_client_test_SOURCES = login-helper-client-test.c

login_helper_server_test_SOURCES = login-helper-server-test.c

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
	   $(TESTS_CFLAGS)	     \
	   $(DEBUG_CFLAGS)

LDADD = ../libspi/libspi.la ../cspi/libcspi.la ../login-helper/libloginhelper.la $(TESTS_LIBS) $(X_LIBS) $(XINPUT_LIBS)

TESTS = test-simple

TESTS_ENVIRONMENT =					\
	LD_LIBRARY_PATH=$(top_builddir)/libspi/.libs/:$(top_builddir)/cspi/.libs/	\
	PATH=$(top_builddir)/registryd/.libs/:$(PATH)

