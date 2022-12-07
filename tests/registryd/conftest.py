# This file contains common test fixtures for registryd's tests.
# Using pytest's terminology for a test's steps:
#
# 1. Arrange: create a test fixture; this usually involves a running registry and an accessible app stub.
#
# 2. Act: manipulate the app or the registry.
#
# 3. Assert: ensure that the thing being manipulated is in the expected state.
#
# 4. Cleanup: Terminate the registry and the app.  It is important that the registry exits cleanly,
#    so it can write out its code coverage information.
#
# This module exports the following fixtures:
#
# * main_loop - a GLib.MainLoop integrated with the DBusGMainLoop.
#
# * session_manager - A mock gnome-session to control the lifetime of daemons.  In
#   reality, the fixture assumes that there is a gnome-session mock running (see
#   ci/run-registryd-tests.sh) and just tells that mock to Logout at fixture teardown
#   time, so that all daemons that monitor the session's lifetime will exit at teardown.
#
# * registry - A dbus.proxies.ProxyObject for the registry's root object.  This automatically
#   depends on a session_manager fixture to control its lifetime.

import pytest
import dbus

@pytest.fixture
def main_loop():
    from dbus.mainloop.glib import DBusGMainLoop
    from gi.repository import GLib

    DBusGMainLoop(set_as_default=True)
    loop = GLib.MainLoop()
    return loop

def get_accesssibility_bus_address():
    bus = dbus.SessionBus()
    bus_launcher = bus.get_object('org.a11y.Bus', '/org/a11y/bus')
    return str(bus_launcher.GetAddress(dbus_interface='org.a11y.Bus'))

@pytest.fixture
def session_manager():
    # This assumes that pytest is running in this environment:
    #
    # * A session dbus daemon is running
    #
    # * There is a gnome-session mock running
    #
    # See the ci/run-registryd-tests.sh script to see how that environment is set up.

    import time

    bus = dbus.SessionBus()
    mock_session = bus.get_object('org.gnome.SessionManager', '/org/gnome/SessionManager')

    mock_session.SetSessionRunning(True, dbus_interface='org.freedesktop.DBus.Mock')

    yield mock_session

    # Tell all session clients to terminate
    mock_session.Logout(0, dbus_interface='org.gnome.SessionManager')

    # Wait a bit for the a11y bus launcher to really die
    proxy = bus.get_object('org.freedesktop.DBus', '/org/freedesktop/DBus')
    while True:
        names = proxy.ListNames(dbus_interface='org.freedesktop.DBus')
        if 'org.a11y.Bus' in names:
            time.sleep(1)
        else:
            break

    # Reset mock session back to its starting state
    mock_session.Reset(dbus_interface='org.freedesktop.DBus.Mock')

@pytest.fixture
def registry_root(main_loop, session_manager):
    a11y_address = get_accesssibility_bus_address()
    a11y_bus = dbus.bus.BusConnection(a11y_address)

    return a11y_bus.get_object('org.a11y.atspi.Registry', '/org/a11y/atspi/accessible/root')

@pytest.fixture
def registry_registry(main_loop, session_manager):
    a11y_address = get_accesssibility_bus_address()
    a11y_bus = dbus.bus.BusConnection(a11y_address)

    return a11y_bus.get_object('org.a11y.atspi.Registry', '/org/a11y/atspi/registry')
