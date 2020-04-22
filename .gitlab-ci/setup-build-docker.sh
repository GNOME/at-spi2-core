#!/bin/bash

set +e
set +x

export PATH="${HOME}/.local/bin:${PATH}"

meson ${EXTRA_MESON_FLAGS:-} _build .
