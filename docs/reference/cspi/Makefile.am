## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.6

# Files whose declarations are to be ignored (private)
IGNORE_HFILES = spi-impl.h

# The name of the module.
DOC_MODULE=at-spi-cspi

# The top-level SGML file.
DOC_MAIN_SGML_FILE=at-spi-cspi-docs.sgml

# The directory containing the source code (if it contains documentation).
DOC_SOURCE_DIR=../../../cspi 

# Used for dependencies
HFILE_GLOB = $(top_srcdir)/cspi/*.h
CFILE_GLOB = $(top_srcdir)/cspi/*.c

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
INCLUDES =                      \
        -I$(top_srcdir)         \
        -I$(top_builddir)       \
        $(DEP_CFLAGS)

#GTKDOC_LIBS = $(top_builddir)/cspilibcspi.la

# gtkdoc-mkdb related varaibles
MKDB_OPTIONS =
content_files =

HTML_IMAGES =

include $(top_srcdir)/gtk-doc.make
