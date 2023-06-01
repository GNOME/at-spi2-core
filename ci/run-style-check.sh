#!/bin/bash

set -e

# We might need to add a new remote for the upstream main, since this script could
# be running in a personal fork of the repository which has out of date branches.
UPSTREAM_URL=https://gitlab.gnome.org/GNOME/at-spi2-core.git
UPSTREAM_REMOTE=$(git remote -v | awk -v url="$UPSTREAM_URL" '$2 == url { print $1; exit }')
if [ -n "$UPSTREAM_REMOTE" ]; then
    echo "Reusing the existing repository on ${CI_PROJECT_NAMESPACE}/${CI_PROJECT_NAME}, it already has a remote, ${UPSTREAM_REMOTE}"
    ORIGIN="$UPSTREAM_REMOTE"
else
    echo "Retrieving the current upstream repository from ${CI_PROJECT_NAMESPACE}/${CI_PROJECT_NAME}..."
    git remote add upstream https://gitlab.gnome.org/GNOME/at-spi2-core.git
    git fetch upstream
    ORIGIN="upstream"
fi

# Wrap everything in a subshell so we can propagate the exit status.
(

# Work out the newest common ancestor between the detached HEAD that this CI job
# has checked out, and the upstream target branch (which will typically be
# `upstream/main` or `upstream/at-spi2-core-3-24`).
#
# `${CI_MERGE_REQUEST_TARGET_BRANCH_NAME}` is only defined if we’re running in
# a merge request pipeline; fall back to `${CI_DEFAULT_BRANCH}` otherwise.
newest_common_ancestor_sha=$(diff --old-line-format='' --new-line-format='' <(git rev-list --first-parent "${ORIGIN}/${CI_MERGE_REQUEST_TARGET_BRANCH_NAME:-${CI_DEFAULT_BRANCH:-main}}") <(git rev-list --first-parent HEAD) | head -1)
git diff -U0 --no-color "${newest_common_ancestor_sha}" | clang-format-diff -p1 > format-diff.log
)
exit_status=$?

[ ${exit_status} == 0 ] || exit ${exit_status}

format_diff="$(<format-diff.log)"

if [ -n "${format_diff}" ]; then
    cat format-diff.log
    echo
    echo "PLEASE FIX THE FORMATTING OF THE SOURCE CODE ABOVE"
    echo
    echo "See https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/gitlab-ci.html#code-formatting"
    echo "for details."

    exit 1
fi
