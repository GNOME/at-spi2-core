#!/bin/bash

set -e

for path in _coverage/*.lcov; do
        lcov --config-file ci/lcovrc -r "${path}" '*/_build/*' -o "$(pwd)/${path}"
        lcov --config-file ci/lcovrc -e "${path}" "$(pwd)/*" -o "$(pwd)/${path}"
done

genhtml \
        --ignore-errors=source \
        --config-file ci/lcovrc \
        _coverage/*.lcov \
        -o _coverage/coverage

cd _coverage
rm -f ./*.lcov
