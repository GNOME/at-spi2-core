bin_PROGRAMS = registryd

INCLUDES = -I$(top_srcdir)          \
           -I$(top_builddir)        \
           -I$(top_builddir)/libspi \
           -I$(top_srcdir)/libspi   \
           $(AT_COMMON_CFLAGS)

LDADD = ../libspi/libspi.la $(AT_COMMON_LIBS)

registryd_SOURCES = registryd.c

oafinfodir = $(datadir)/oaf
oafinfo_DATA = Accessibility_Registry.oaf

@XML_I18N_MERGE_OAF_RULE@

EXTRA_DIST = Accessibility_Registry.oaf.in $(oafinfo_DATA)
