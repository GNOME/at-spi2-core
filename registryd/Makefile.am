bin_PROGRAMS = registryd

INCLUDES = -I$(top_srcdir)          \
           -I$(top_builddir)        \
           -I$(top_builddir)/libspi \
           -I$(top_srcdir)/libspi   \
           $(REGISTRYD_CFLAGS)

at_registryd_SOURCES = registryd.c

LDADD = ../libspi/libspi.la $(REGISTRYD_LIBS)

oafinfodir = $(datadir)/oaf
oafinfo_DATA = Accessibility_Registry.oaf

@XML_I18N_MERGE_OAF_RULE@

EXTRA_DIST = Accessibility_Registry.oaf.in $(oafinfo_DATA)
