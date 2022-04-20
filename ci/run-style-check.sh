#!/bin/bash

set -e

ancestor_horizon=28  # days (4 weeks)

# Wrap everything in a subshell so we can propagate the exit status.
(

source ci/search-common-ancestor.sh

git diff -U0 --no-color "${newest_common_ancestor_sha}" atspi/*.c bus/*.c dbind/*.c registryd/*.c test/*.c | clang-format-diff -p1 > format-diff.log

)
exit_status=$?

[ ${exit_status} == 0 ] || exit ${exit_status}

format_diff="$(<format-diff.log)"

if [ -n "${format_diff}" ]; then
  echo 'body=```diff' > format.log
  cat format-diff.log >> format.log
  echo '```' >> format.log
  [ -n "$CI_MERGE_REQUEST_IID" ] && curl \
    --request POST \
    --header "Private-Token: $STYLE_CHECK_TOKEN" \
    --data-urlencode "$(<format.log)" \
    https://gitlab.gnome.org/api/v4/projects/$CI_PROJECT_ID/merge_requests/$CI_MERGE_REQUEST_IID/notes \
    --insecure
  unlink format.log
  exit 1
fi
