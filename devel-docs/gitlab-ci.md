# Gitlab Continuous Integration (CI) for at-spi2-core

Summary: make the robots set up an environment for running the test
suite, run it, and report back to us.

If you have questions about the CI, mail federico@gnome.org, or [file
an issue](https://gitlab.gnome.org/GNOME/at-spi2-core/-/issues) and
mention `@federico` in it.

Table of contents:

[[_TOC_]]

# Quick overview

By having a [`.gitlab-ci.yml`](../.gitlab-ci.yml) file in the toplevel
directory of a project, Gitlab knows that it must run a continuous
integration pipeline when certain events occur, for example, when
someone creates a merge request, or pushes to a branch.

What's a pipeline?  It is an automated version of the following.
Running the test suite for at-spi2-core involves some repetitive
steps:

* Create a pristine and minimal environment for testing, without all the random
  gunk from one's development system.  Gitlab CI uses Linux containers,
  with pre-built operating system images in the [Open Container
  Initiative][OCI] format — this is what Docker, Podman, etc. all use.

* Install the build-time dependencies (gcc, meson, libfoo-devel,
  etc.), the test-time dependencies (dbus-daemon, etc.) in that
  pristine environment, as well as special tools (lcov, libasan,
  clang-tools).

* Run the build and install it, and run the test suite.

* Run variations of the build and test suite with other tools — for
  example, using static analysis during compilation, or with address
  sanitizer (asan), or with a code coverage tool.  Gitlab can collect
  the analysis results of each of these tools and present them as part
  of the merge request that is being evaluated.  It also lets
  developers obtain those useful results without dealing with a lot of
  fiddly tools on their own computers.
  
Additionally, on each pipeline run we'd like to do extra repetitive
work like building the reference documentation, and publishing it on a
web page.

The `.gitlab-ci.yml` file defines the CI pipeline, the jobs it will
run (build/test, coverage, asan, static-scan, etc.), and the locations
where each job's artifacts will be stored.

What's an artifact or a job?  Read on!

# A little glossary

**Pipeline** - A collection of **jobs**, which can be run in parallel
or sequentially.  For example, a pair of "build" and "test" jobs would
need to run sequentially, but maybe a "render documentation" job can
run in parallel with them.  Similarly, "build" jobs for various
distributions or configurations could be run in parallel.

**Job** - Think of it as running a shell script within a container.
It can have input from other previous jobs: if you have separate
"build" and "test" jobs, then the "build" job will want to keep around
its compiled artifacts so that the "test" job can use them.  It can
provide output artifacts that can be stored for human perusal, or for
use by other jobs.

**Artifact** - Something produced from a job.  If your job compiles
binaries, those binaries could be artifacts if you decide to keep them
around for use later.  A documentation job can produce HTML artifacts
from the rendered documentation.  A code coverage job will produce a
coverage report artifact.

**Runner** - An operating system setup for running jobs.
Gitlab.gnome.org provides runners for Linux, BSD, Windows, and MacOS.
For example. the Linux runners let you use any OCI image, so you can
test on openSUSE, Fedora, a custom distro, etc.  You don't normally
need to be concerned with runners; Gitlab assigns the shared runners
automatically to your pipeline.

**Container** - You can think of it as a chroot with extra isolation,
or a lightweight virtual machine.  Basically, the Linux kernel can
isolate groups of processes in control groups (cgroups).  Each cgroup
can have a different view of the file system, as if you had a
different chroot for each cgroup.  Cgroups can be isolated to be in
their own PID namespace, so running "ps" in the container will not
show all the processes in the system, but only those inside the
container's cgroup.  File system overlays allow you to have read-only
images for the operating system (the OCI images we talked about above)
plus a read-write overlay that is kept around only during the lifetime
of a container, or persistently if one wants.  For Gitlab CI one does
not need to deal with containers directly, but keep in mind that your
jobs will run inside a container, which is more limited than e.g. a
shell session on a graphical, development machine.

# The CI pipeline for at-spi2-core

The `.gitlab-ci.yml` file is a more-or-less declarative description
the CI pipeline, with some `script` sections which are imperative
commands to actually *do stuff*.

Jobs are run in `stages`, and the names of the stages are declared
near the beginning of the YAML file.  The stage names are arbitrary;
the ones here follow some informal GNOME conventions.

Jobs are declared at the toplevel of the YAML file, and they are
distinguished from other declarations by having a container `image`
declared for them, as well as a `script` to execute.

Many jobs need exactly the same kind of setup (same container images,
mostly same package dependencies), so they use `extends:` to use a
declared template with all that stuff instead of redeclaring it each
time.  In our configuration, the `.only-default` template has the
`PROJECT_DEPS`, with the dependencies that most jobs need.  The
`.build-setup` template is for the analysis jobs, and it lets them
declare `EXTRA_DEPS` as an environment variable with the names of
extra dependencies: for example, the coverage job puts `lcov` in
`EXTRA_DEPS`.  The commands in `before_script` blocks use these
environment variables to install the package dependencies, for example
`zypper install -y ${PROJECT_DEPS}` for an openSUSE job.

The `build` stage has these jobs:

* `opensuse-x86_64` - Extends the `.build-default` rule,
  builds/installs the code, and runs the tests.  Generally this is the
  job that one cares about during regular development.

The `analysis` stage has these jobs:

* `static-scan` - Runs static analysis during compilation, which
  performs interprocedural analysis to detect things like double
  `free()` or uninitialized struct fields across functions.
  
* `asan-build` - Builds and runs with Address Sanitizer (libasan).

* `coverage` - Instruments the build to get code coverage information,
  and runs the test suite to see how much of the code it manages to
  exercise.  This is to see which code paths may be untested
  automatically, and to decide which ones would require manual
  testing, or refactoring to allow automated testing.

As of 2021/Dec/15 there are some commented-out jobs to generate
documentation and publish it; this needs to be made to work.

# References

Full documentation for Gitlab CI: https://docs.gitlab.com/ee/ci/

Introduction to Gitlab CI: https://docs.gitlab.com/ee/ci/quick_start/index.html

at-spi2-core's CI pipeline is mostly [cut-and-pasted from
libgweather](https://gitlab.gnome.org/GNOME/libgweather/-/blob/main/.gitlab-ci.yml).
Thanks to Emmanuele Bassi for his advice on how to use it.

[OCI]: https://opencontainers.org/
