source ./ci/env.sh

set -eu

REPOSITORY=https://github.com/federicomenaquintero/grcov
BRANCH=accessible-html

git clone --depth 1 --branch $BRANCH $REPOSITORY
cd grcov
cargo build --release

cp target/release/grcov /usr/local/bin/
