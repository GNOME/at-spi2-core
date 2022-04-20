# Continuous Integration scripts for at-spi2-core

Please see the general [documentation for at-spi2-core's Gitlab CI][ci-docs].

This directory contains scripts which get called during a run of a CI
pipeline, and utilities to maintain the CI infrastructure.

## Scripts used during a run of a CI pipeline:

* `container-builds.yml` - Gets included from the toplevel
  `.gitlab-ci.yml`; has the declarations to build each container image
  with the [Freedesktop CI Templates][ci-templates] machinery.

* `run-tests.sh` - Runs the test suite and prints other diagnostics.

* `gen-coverage.sh` - After the test suite is run, merges the various
  code coverage reports from `lcov`, and generates an HTML report.
  
* `lcovrc` - Configuration file for `lcov`, used by `gen-coverage.sh`.
  Among other things, this tells `lcov` to exclude branch coverage for
  the unreachable branches of `g_return_if_fail()` and friends.
  
* `run-style-check.sh` - Runs `clang-format-diff` to test for source
  files with inconsistent formatting, and uploads the resulting report
  to gitlab so it can be viewed as part of a merge request's analysis.
  
* `search-common-ancestor.sh` - Utility used from
  `run-style-check.sh`; finds a git branch point from the current
  commit.

[ci-docs]: ../devel-docs/gitlab-ci.md
[ci-templates]: https://gitlab.freedesktop.org/freedesktop/ci-templates/
