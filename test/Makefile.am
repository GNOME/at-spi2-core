NULL=

noinst_PROGRAMS = at app simple-at keysynth-demo accessx-gui

at_SOURCES = at.c 
 
app_SOURCES = app.c

simple_at_SOURCES = simple-at.c 

keysynth_demo_SOURCES = keysynth-demo.c 

accessx_gui_SOURCES = accessx-gui.c 

INCLUDES = -I$(top_srcdir)           \
           -I$(top_builddir)         \
           -I$(top_srcdir)/libspi    \
           -I$(top_builddir)/libspi  \
	   -I$(top_srcdir)/cspi	     \
	   -I$(top_builddir)/cspi    \
	   $(TESTS_CFLAGS)

DEBUG_CFLAGS=-DSPI_DEBUG

CFLAGS += $(TESTS_CFLAGS) $(DEBUG_CFLAGS)

LDADD = ../util/libat-util.la ../libspi/libspi.la ../cspi/libcspi.la $(TESTS_LIBS)



