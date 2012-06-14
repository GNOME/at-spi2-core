#!/bin/sh

# run gtkdocize
gtkdocize || exit 1

intltoolize --force --copy --automake || exit 1

# gnome-autogen.sh runs configure, so do likewise.
autoreconf -vif
test -n "$NOCONFIGURE" || ./configure "$@"

