#!/bin/sh

set -eu

GRCOV_VERSION=$1
GRCOV_ARCH=$2

GRCOV_URL=https://github.com/mozilla/grcov/releases/download/$GRCOV_VERSION/grcov-$GRCOV_ARCH.tar.bz2

wget $GRCOV_URL
tar xvf grcov-x86_64-unknown-linux-gnu.tar.bz2
chmod 755 grcov

mv grcov /usr/local/bin
