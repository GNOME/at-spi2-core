#!/bin/sh
# gnome-autogen.sh runs configure, so do likewise.
autoreconf -vif && ./configure "$@"
