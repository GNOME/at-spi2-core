#!/bin/sh

echo "About to run the tests.  First we'll launch the accessibility bus by calling GetAddress:"

dbus-send --print-reply --session --dest=org.a11y.Bus /org/a11y/bus org.a11y.Bus.GetAddress

ps auxwww

echo "Now running the tests:"

meson test -C _build

echo "After the tests - calling GetAddress again:"

dbus-send --print-reply --session --dest=org.a11y.Bus /org/a11y/bus org.a11y.Bus.GetAddress

