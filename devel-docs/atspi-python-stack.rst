Libatspi and the Python stack
=============================

Libatspi is a C library that holds an AT's view of an application's
tree of accessible objects.  For example, when Orca gets to know about
an application, it has to build a view of the app's accessible objects
so it can let the user navigate among them.  Libatspi provides that
view with a tree-ish data structure.

The Python API for libatspi
---------------------------

Orca is written in Python, so there is an intermediate layer between
Libatspi and Orca — that's pyatspi2.  While in principle it could just
be a language binding generated automatically from GObject
Introspection data on ``libatspi.so``, in practice it is a
hand-written wrapper around exactly that kind of
automatically-generated binding.  Orca predates GObject Introspection;
instead of changing its code to use the new automatically-generated
API, pyatspi2 was written/modified to accomodate Orca's use of the
"old API".

For the purposes of this chapter, we will call the GObject
Introspection API generated for libatspi the "GI API", and what Orca
expects and consumes from pyatspi2 the "old API".

Cleaning up the Python stack
----------------------------

The Python part of the accessibility stack has more or less these components:

- Orca, the screenreader.
- Pyatspi2, API glue and a little logic.
- Dogtail, a GUI testing tool built upon the accessibility APIs.
- Accerciser, a tool for developers.

Pyatspi2 has no CI, but it does have a test suite.  Its repository
could be merged into at-spi2-core, the same as was done for
atk/at-spi2-atk.  One detail is that pyatspi2 still uses autotools,
while at-spi2-core uses meson, but this is a good opportunity to
change pyatspi2 to meson as well.

Orca has no CI.  It has a substantial test suite whose runner launches
applications like Firefox or LibreOffice, and runs a bunch of custom
scripts on them.

I would like to gradually "disappear" pyatspi2 like this:

* Make Orca use the GI API directly, instead of pyatspi2's wrappers.
  I think a lot of the work to do this is just renaming functions; for
  example, in the way that pyatspi already does things like
  ``Atspi.Hyperlink.getObject = Atspi.Hyperlink.get_object`` just for
  Orca's benefit.  **Update:** This is in progress as of 2023/Jun/14,
  see `Orca issue #300
  <https://gitlab.gnome.org/GNOME/orca/-/issues/300>`_.

* If Orca wants to keep running on old versions of the stack, those
  compatibility wrappers should live in Orca itself, not in pyatspi2.
  I would prefer it if Orca were released in tandem with at-spi2-core,
  but I don't know all of Orca's goals for accomodating various systems.

* Some parts of pyatspi2 are not just wrappers, but they have some
  logic of their own or hold a bit of state on their own.  I'd like to
  move this logic down to libatspi, or up into Orca, if possible.
  Rationale: if it's actual logic, don't make it part of the binding;
  instead make it generally useful in the basic library.  If it's
  utility code for Orca, make it live in Orca.

You'll note that I have not mentioned Dogtail and Accerciser in the
points above.  The same work should be done in them, and I think it
would be useful to make a choice:

* Can we formally consider orca/dogtail/accerciser to be the only
  supported users of pyatspi2?  That is, can we agree to release them
  in tandem with at-spi2-core+pyatspi2 while we finish cleaning things
  up, but not worry about external consumers of the API?

* If there are external consumers of the Python API which we must
  absolutely keep running, can we let them know how to make the
  necessary changes?

(I'm trying to reduce the amount of code that the "GNOME accessibility
team" is expected to keep working at all times.)

Per the discussion in `at-spi2-core !136
<https://gitlab.gnome.org/GNOME/at-spi2-core/-/merge_requests/136>`,
these are the reverse dependencies of pyatspi2 in Debian:

* ``accerciser`` - It's in a creaky state; would be good to replace it
  with something that is not X11-centric.
* ``autokey-common`` - Only uses the `keyboard listener
  <https://github.com/autokey/autokey/blob/717f90321b0d4a8053bdb7490ac7ca4e8c086ce9/lib/autokey/interface.py#L1317-L1346>`_,
  but this is changing in atspi anyway.
* caribou - It is `archived/read-only
  <https://gitlab.gnome.org/Archive/caribou>`_ and the last release
  was `from 2016 <https://download.gnome.org/sources/caribou/0.4/>`_ -
  probably dead by now.
* gnome-photos-tests - Only uses dogtail.
* ibus-speech - Hypra, not open source?
* ibus-speech-applet - Hypra, not open source?
* orca - Under discussion in the present document.
* python3-dogtail - Under discussion in the present document.

At some point we may want to make pyatspi2 emit a deprecation warning,
when GNOME-owned projects are converted to use the
gobject-introspected Atspi directly.

Orca's compatibility goals
--------------------------

Orca has to work in GNOME, MATE, KDE, and basically every other desktop.

Some people update Orca by hand even when they stay on an old distro.
This is easy to do — Orca can be installed in one's home directory as
a Python program — but upgrading at-spi2-core and its integrated
libraries requires superuser privileges.

I'm not sure how to resolve that.  We cannot support everyone's custom
configuration forever, but maybe we can make it easy for them to get
distro-like packages with updates.  SUSE's Open Build Service can
build packages for many distros, or maybe we can find contacts who are
willing to maintain updated packages or Orca's stack for their old
distros.

Modern amenities for Orca
-------------------------

I think this would be a good chance to bring some modern Python 3 amenities to Orca:

* Ruff/Flake8 - various lints and style checks.
* Automatic code formatting around PEP 8, like we ensure the C style
  during CI for at-spi2-core.
* Code coverage reports.  These are especially useful to find obsolete
  compatibility code.
* Static and runtime type checks.

If these look like a subset of `Hypermodern Python Cookiecutter
<https://github.com/cjolowicz/cookiecutter-hypermodern-python>`_, it's
because they are!

Summary: automate as much as possible of the grunt work that
maintainers need to do, and have the robots do it for us.  Use tools
that help us keep a rowdy code base under control.  See if type
checking can help us catch bugs early.  Make the code easier to
approach for newcomer by having safety checks everywhere.

