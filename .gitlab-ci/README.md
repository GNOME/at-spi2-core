# Continuous Integration scripts for at-spi2-core

Please see the general [documentation for at-spi2-core's Gitlab CI][ci-docs].

This directory contains scripts which get called during a run of a CI
pipeline, and utilities to maintain the CI infrastructure.

## Scripts used during a run of a CI pipeline:

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

## Utilities to maintain the CI infrastructure:

To make pipelines fast, and avoid a lot of repeated downloads,
at-spi2-core uses pre-built container images for CI pipelines, instead
of using a stock image like opensuse/tumbleweed and then installing
all the dependencies on top of it every time.

The prebuilt images are stored here:
https://gitlab.gnome.org/GNOME/at-spi2-core/container_registry

Instead of maintaining those images by hand with `docker` or `podman`
commands, here is a little script (stolen from [glib][glib-ci]) to
maintain them, which you can start exploring with `./run-docker.sh help`.

This script knows how to build and upload images from Dockerfiles
called `foo.Dockerfile`.  The image configurations we have:

* `opensuse.Dockerfile` - starts with an opensuse/tumbleweed image and
  installs the package dependencies for building at-spi2-core.

If you are one of at-spi2-core's maintainers, you'll want to update
the CI images periodically.  First, install `podman` and
`podman-docker`.  Then, run this:

```sh
# "opensuse" in these commands indicates to use the opensuse.Dockerfile configuration

./run-docker.sh build --base=opensuse      # builds the image, takes a while

./run-docker.sh run --base=opensuse        # launch the container; poke around; see that it works

./run-docker.sh push --base=opensuse       # push the image to registry.gitlab.gnome.org
```

The `build` subcommand creates an image named
`registry.gitlab.gnome.org/gnome/at-spi2-core/opensuse/tumbleweed:latest`
**that is only stored in your localhost**.

The `run` subcommand launches a container with that image and gives
you a shell prompt.  This is equivalent to `podman run`.

The `push` subcommand takes that built image and uploads it to
`registry.gitlab.gnome.org`.  It will then be visible from
https://gitlab.gnome.org/GNOME/at-spi2-core/container_registry - the
CI configuration in [`.gitlab-ci.yml`](../.gitlab-ci.yml) uses this
image for the pipeline.

[ci-docs]: ../devel-docs/gitlab-ci.md
[container-registry-docs]: https://gitlab.gnome.org/help/user/packages/container_registry/index
[glib-ci]: https://gitlab.gnome.org/GNOME/glib/-/tree/main/.gitlab-ci
