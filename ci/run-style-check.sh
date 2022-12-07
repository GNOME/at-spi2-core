#!/bin/bash

set -e

# We need to add a new remote for the upstream main, since this script could
# be running in a personal fork of the repository which has out of date branches.
if [ "${CI_PROJECT_NAMESPACE}" != "GNOME" ]; then
    echo "Retrieving the current upstream repository from ${CI_PROJECT_NAMESPACE}/${CI_PROJECT_NAME}..."
    git remote add upstream https://gitlab.gnome.org/GNOME/at-spi2-core.git
    git fetch upstream
    ORIGIN="upstream"
else
    echo "Reusing the existing repository on ${CI_PROJECT_NAMESPACE}/${CI_PROJECT_NAME}"
    ORIGIN="origin"
fi

# Wrap everything in a subshell so we can propagate the exit status.
(

# Work out the newest common ancestor between the detached HEAD that this CI job
# has checked out, and the upstream target branch (which will typically be
# `upstream/main` or `upstream/at-spi2-core-3-24`).
#
# `${CI_MERGE_REQUEST_TARGET_BRANCH_NAME}` is only defined if we’re running in
# a merge request pipeline; fall back to `${CI_DEFAULT_BRANCH}` otherwise.
newest_common_ancestor_sha=$(diff --old-line-format='' --new-line-format='' <(git rev-list --first-parent "${ORIGIN}/${CI_MERGE_REQUEST_TARGET_BRANCH_NAME:-${CI_DEFAULT_BRANCH}}") <(git rev-list --first-parent HEAD) | head -1)
git diff -U0 --no-color "${newest_common_ancestor_sha}" | clang-format-diff -p1 > format-diff.log
)
exit_status=$?

[ ${exit_status} == 0 ] || exit ${exit_status}

format_diff="$(<format-diff.log)"

if [ -n "${format_diff}" ]; then
    cat format-diff.log
    echo
    echo "PLEASE FIX THE FORMATTING OF THE SOURCE CODE ABOVE"
    exit 1
fi
