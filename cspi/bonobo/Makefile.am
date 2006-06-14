INCLUDES = -I $(top_srcdir)           \
           -I $(top_builddir)         \
           $(X_CFLAGS)                \
           $(WARN_CFLAGS)                \
           $(LIBCSPI_CFLAGS)

noinst_LTLIBRARIES = libcspi-bonobo.la

libcspi_bonobo_la_SOURCES =	\
	cspi-bonobo.c		\
	cspi-bonobo-listener.c	\
	cspi-bonobo-listener.h

libcspi_bonobo_la_LIBADD = $(LIBCSPI_LIBS) $(X_LIBS)
