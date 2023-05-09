#!/bin/sh

set -eu

echo "About to run the tests.  First we'll launch a gnome-session DBus mock."

python3 -m dbusmock --session org.gnome.SessionManager /org/gnome/SessionManager org.gnome.SessionManager &

gdbus wait --session --timeout 10 org.gnome.SessionManager

gdbus call --session \
      --dest org.gnome.SessionManager \
      --object-path /org/gnome/SessionManager \
      --method org.freedesktop.DBus.Mock.AddTemplate 'tests/dbusmock/mock-gnome-session.py' '{}'

echo "Launching the accessibility bus by calling GetAddress:"

gdbus call --session --dest org.a11y.Bus --object-path /org/a11y/bus --method org.a11y.Bus.GetAddress

ps auxwww

echo "Setting the mock session to the running state"

gdbus call --session \
      --dest org.gnome.SessionManager \
      --object-path /org/gnome/SessionManager \
      --method org.freedesktop.DBus.Mock.SetSessionRunning true

echo "Now running the tests:"

meson test -C _build --print-errorlogs

echo "Telling the mock session to logout so the a11y daemons will exit"

gdbus call --session \
      --dest org.gnome.SessionManager \
      --object-path /org/gnome/SessionManager \
      --method org.gnome.SessionManager.Logout 0

ps auxwww
