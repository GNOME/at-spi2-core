#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="at-spi"

USE_GNOME2_MACROS=1 . gnome-autogen.sh
