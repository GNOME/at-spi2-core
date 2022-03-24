#!/bin/bash

set +x
set +e

srcdir=$( pwd )
builddir=$1

meson test -C ${builddir} \
        --print-errorlogs

# Store the exit code for the CI run, but always
# generate the reports
exit_code=$?

cd ${builddir}

$srcdir/.gitlab-ci/meson-junit-report.py \
        --project-name=atk \
        --job-id="${CI_JOB_NAME}" \
        --output=report.xml \
        meson-logs/testlog.json

exit $exit_code
