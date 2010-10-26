DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

SUBDIRS=registryd xml bus dbind atspi

ACLOCAL_AMFLAGS=-I m4
