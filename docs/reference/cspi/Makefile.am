## Process this file with automake to produce Makefile.in

# The name of the module.
DOC_MODULE = at-spi-cspi

# The top-level SGML file.
DOC_MAIN_SGML_FILE = at-spi-cspi-docs.sgml

# The directory containing the source code (if it contains documentation).
DOC_SOURCE_DIR = $(top_srcdir)/cspi

# Used for dependencies
HFILE_GLOB = $(top_srcdir)/cspi/*.h
CFILE_GLOB = $(top_srcdir)/cspi/*.c

# Header files to ignore when scanning
IGNORE_HFILES =	\
	spi-impl.h

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
INCLUDES = 			\
	-I$(top_srcdir)		\
	-I$(top_builddir)	\
	$(X_CFLAGS)		\
	$(LIBCSPI_CFLAGS)

GTKDOC_LIBS = $(top_builddir)/cspi/libspi.la

# gtkdoc-mkdb related varaibles
MKDB_OPTIONS =
HTML_IMAGES =

# If you need to override some of the declarations, place them in the
# $(DOC_MODULE)-overrides.txt file and uncomment the second line here.
DOC_OVERRIDES = $(DOC_MODULE)-overrides.txt

include $(top_srcdir)/gtk-doc.make
