Gitlab Continuous Integration (CI) for at-spi2-core
===================================================

Summary: make the robots set up an environment for running the test
suite, run it, and report back to us.  Have them also run lints and
formatters, and get interesting reports for our perusal.

If you have questions about the CI, mail federico@gnome.org, or `file an
issue <https://gitlab.gnome.org/GNOME/at-spi2-core/-/issues>`__ and
mention ``@federico`` in it.

Quick overview
--------------

By having a `.gitlab-ci.yml
<https://gitlab.gnome.org/GNOME/at-spi2-core/-/blob/main/.gitlab-ci.yml>`_
file in the toplevel directory of a project, Gitlab knows that it must
run a continuous integration pipeline when certain events occur, for
example, when someone creates a merge request, or pushes to a branch.

What’s a pipeline? It is an automated version of the following. Running
the test suite for at-spi2-core involves some repetitive steps:

-  Create a reproducible environment for testing, without all
   the random gunk from one’s development system. Gitlab CI uses Linux
   containers, with pre-built operating system images in the `Open
   Container Initiative <https://opencontainers.org/>`__ format — this
   is what Docker, Podman, etc. all use.

-  Install the build-time dependencies (gcc, meson, libfoo-devel, etc.),
   the test-time dependencies (dbus-daemon, etc.) in that pristine
   environment, as well as special tools (lcov, libasan, clang-tools).

-  Run the build and install it, and run the test suite.

-  Run variations of the build and test suite with other tools — for
   example, using static analysis during compilation, or with address
   sanitizer (asan), or with a code coverage tool. Gitlab can collect
   the analysis results of each of these tools and present them as part
   of the merge request that is being evaluated. It also lets developers
   obtain those useful results without dealing with a lot of fiddly
   tools on their own computers.

Additionally, on each pipeline run we’d like to do extra repetitive work
like building the reference documentation, and publishing it on a web
page.

The ``.gitlab-ci.yml`` file defines the CI pipeline, the jobs it will
run (build/test, coverage, asan, static-scan, etc.), and the locations
where each job’s artifacts will be stored.

What’s an artifact or a job? Read on!

A little glossary
-----------------

**Pipeline** - A collection of **jobs**, which can be run in parallel or
sequentially. For example, a pair of “build” and “test” jobs would need
to run sequentially, but maybe a “render documentation” job can run in
parallel with them. Similarly, “build” jobs for various distributions or
configurations could be run in parallel.

**Job** - Think of it as running a shell script within a container. It
can have input from other previous jobs: if you have separate “build”
and “test” jobs, then the “build” job will want to keep around its
compiled artifacts so that the “test” job can use them. It can provide
output artifacts that can be stored for human perusal, or for use by
other jobs.

**Artifact** - Something produced from a job. If your job compiles
binaries, those binaries could be artifacts if you decide to keep them
around for use later. A documentation job can produce HTML artifacts
from the rendered documentation. A code coverage job will produce a
coverage report artifact.

**Runner** - An operating system setup for running jobs.
Gitlab.gnome.org provides runners for Linux, BSD, Windows, and MacOS.
For example. the Linux runners let you use any OCI image, so you can
test on openSUSE, Fedora, a custom distro, etc. You don’t normally need
to be concerned with runners; Gitlab assigns the shared runners
automatically to your pipeline.

**Container** - You can think of it as a chroot with extra isolation, or
a lightweight virtual machine. Basically, the Linux kernel can isolate
groups of processes in control groups (cgroups). Each cgroup can have a
different view of the file system, as if you had a different chroot for
each cgroup. Cgroups can be isolated to be in their own PID namespace,
so running “ps” in the container will not show all the processes in the
system, but only those inside the container’s cgroup. File system
overlays allow you to have read-only images for the operating system
(the OCI images we talked about above) plus a read-write overlay that is
kept around only during the lifetime of a container, or persistently if
one wants. For Gitlab CI one does not need to deal with containers
directly, but keep in mind that your jobs will run inside a container,
which is more limited than e.g. a shell session on a graphical,
development machine.

The CI pipeline for at-spi2-core
--------------------------------

The ``.gitlab-ci.yml`` file is a more-or-less declarative description
the CI pipeline, with some ``script`` sections which are imperative
commands to actually *do stuff*.

Jobs are run in ``stages``, and the names of the stages are declared
near the beginning of the YAML file. The stage names are arbitrary; the
ones here follow some informal GNOME conventions.

Jobs are declared at the toplevel of the YAML file, and they are
distinguished from other declarations by having a container ``image``
declared for them, as well as a ``script`` to execute.

Many jobs need exactly the same kind of setup (same container images,
mostly same package dependencies), so they use ``extends:`` to use a
declared template with all that stuff instead of redeclaring it each
time.

The ``container-build`` stage builds container images with the
reproducible environments described above.  See the :ref:`ci_templates`
below for details.

The ``style-check`` stage runs a code formatter to ensure that new
code preserves the coding style.  It does not modify your code;
instead the job will fail if your code does not match the rest of the
coding style, so you can fix it.  See :ref:`code_formatting` for details.

The ``build`` stage has these jobs:

-  ``opensuse-x86_64`` - Extends the ``.only-default`` rule,
   builds/installs the code, and runs the tests. Generally this is the
   job that one cares about during regular development.

-  ``fedora-x86_64`` - Same as the previous job, but runs on Fedora
   instead of openSUSE. The intention is to run the build configuration
   for ``dbus-broker`` here instead of ``dbus-daemon``.

The ``analysis`` stage has these jobs:

-  ``static-scan`` - Runs static analysis during compilation, which
   performs interprocedural analysis to detect things like double
   ``free()`` or uninitialized struct fields across functions.

-  ``asan-build`` - Builds and runs with Address Sanitizer (libasan).

-  ``coverage`` - Instruments the build to get code coverage
   information, and runs the test suite to see how much of the code it
   manages to exercise. This is to see which code paths may be untested
   automatically, and to decide which ones would require manual testing,
   or refactoring to allow automated testing.

Finally, the ``docs`` stage builds documentation:

- ``reference`` - Public API reference.

- ``devel-docs`` - Development guide for the accessibility internals,
   including the document you are reading right now.


.. _ci_templates:

CI templates
------------

The task of setting up a container image to do CI for a particular
distro or build configuration is rather repetitive. One has to start
with a “bare” distro image, then install the build-time dependencies
that your project requires, which is slow; then you want to test
another distro, then you want to make those container images easily
available to your project’s forks, and then you start pulling your
hair.

`Fredesktop CI
Templates <https://gitlab.freedesktop.org/freedesktop/ci-templates/>`__
(`documentation <https://freedesktop.pages.freedesktop.org/ci-templates/>`__)
are a solution to this. They can automatically build container images
for various distros, make them available to forks of your project, and
have some nice amenities to reduce the maintenance burden.

At-spi2-core uses CI templates to test its various build configurations,
since it actually works differently depending on a distro’s choice of
``dbus-daemon`` versus ``dbus-broker``.

The prebuilt container images are stored here:
https://gitlab.gnome.org/GNOME/at-spi2-core/container_registry

They get updated automatically thanks to the CI Templates machinery.

.. _code_formatting:

Code formatting
---------------

The C coding style is enforced via `clang-format
<https://clang.llvm.org/docs/ClangFormat.html>`_ and a 
`.clang-format
<https://gitlab.gnome.org/GNOME/at-spi2-core/-/blob/main/.clang-format>`_
configuration file (`docs on configuration
<https://clang.llvm.org/docs/ClangFormatStyleOptions.html>`_).

The ``style-check-diff`` job in CI will fail if you put in new code
that does not match what ``clang-format`` would do for it.  You must
fix your code by hand; it is not re-indented automatically to give you
a chance to selectively opt-out of formatting some chunks of code.

To format a whole file, run ``clang-format -i some_file.c``.  The
``-i`` option means "in place"; with it the file will be overwritten,
otherwise clang-format will write to standard output.

You can prevent a chunk of code from being changed with comments like
the following, for example, for a struct initializer:

.. code:: c

   /* clang-format off */

   static MyStruct some_array[] = {
     { "a",            42, "b"        },
     { "long string",   0, "blahblah" },
     { "etc etc",     -42, ""         },
   };

   /* clang-format on */

Note that **clang-format likes to re-order includes alphabetically**
within chunks separated by blank lines:

.. code:: c

   #include <dbus/dbus.h>
   
   #include "de-marshaller.h"
   #include "de-types.h"
   #include "keymasks.h"
   #include "paths.h"
   
   #include "deviceeventcontroller.h"
   #include "introspection.h"
   #include "reentrant-list.h"

Here, each of the three groups of includes will be sorted
independently.  You can fix your header files so that the order of
inclusion doesn't matter, or separate them out with blank lines to
enforce ordering.

General advice and future work
------------------------------

A failed run of a CI pipeline should trouble you; it either means that
some test broke, or that something is not completely deterministic. Fix
it at once.

Try not to accept merge requests that fail the CI, as this will make
``git bisect`` hard in the future. There are tools like Marge-bot to
enforce this; ask federico@gnome.org about it. Read `“The Not Rocket
Science Rule Of Software
Engineering” <https://graydon.livejournal.com/186550.html>`__, which can
be summarized as “automatically maintain a repository of code that
always passes all the tests” for inspiration. Marge-bot is an
implementation of that, and can be used with Gitlab.

If your software can be configured to build with substantial changes,
the CI pipeline should have jobs that test each of those configurations.
For example, at-spi-bus-launcher operates differently depending on
whether ``dbus-daemon`` or ``dbus-broker`` are being used. As of
2022/Apr/19 the CI only tests ``dbus-daemon``; there should be a test
for dbus-broker, too, in the ``fedora-x86_64`` job since that is one of
the distros that uses ``dbus-broker``.

Although the YAML syntax for ``.gitlab-ci.yml`` is a bit magic, the
scripts and configuration are quite amenable to refactoring. Do it
often!

Minimizing the amount of time that CI takes to run is a good goal. It
reduces energy consumption in the build farm, and allows you to have a
faster feedback loop. Instead of installing package dependencies on each
job, we can move to prebuilt container images.

References
----------

Full documentation for Gitlab CI: https://docs.gitlab.com/ee/ci/

Introduction to Gitlab CI:
https://docs.gitlab.com/ee/ci/quick_start/index.html

Freedesktop CI templates:
https://gitlab.freedesktop.org/freedesktop/ci-templates/
