DISTCHECK_CONFIGURE_FLAGS = --enable-introspection --enable-gtk-doc

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

SUBDIRS=po registryd xml bus dbind atspi doc

ACLOCAL_AMFLAGS=-I m4
