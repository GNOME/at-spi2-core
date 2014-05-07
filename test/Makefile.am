LDADD = $(top_builddir)/atspi/libatspi.la
noinst_PROGRAMS = memory
memory_SOURCES = memory.c
memory_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_builddir)/atspi
memory_CFLAGS = $(GLIB_CFLAGS) 	$(GOBJ_LIBS) $(DBUS_CFLAGS)
memory_LDFLAGS = 

-include $(top_srcdir)/git.mk
