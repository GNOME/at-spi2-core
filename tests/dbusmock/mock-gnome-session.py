'''gnome-session mock template

This module implements mock versions of a few of the DBus interfaces from
org.gnome.SessionManager and org.gnome.SessionManager.ClientPrivate.  They are used by
at-spi2-core's test suite, to simulate communication with a gnome-session manager.
'''

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.  See http://www.gnu.org/copyleft/lgpl.html for the full text
# of the license.

__author__ = 'Federico Mena Quintero'
__copyright__ = '(c) 2022 Federico Mena Quintero'

import dbus

from dbusmock import MOCK_IFACE
import dbusmock

SYSTEM_BUS = False
BUS_NAME = 'org.gnome.SessionManager'
MAIN_OBJ = '/org/gnome/SessionManager'
MAIN_IFACE = 'org.gnome.SessionManager'
CLIENT_PRIVATE_IFACE = 'org.gnome.SessionManager.ClientPrivate'

def load(mock, parameters):
    mock.is_running = False
    mock.client_serial_number = 0

# FIXME: at-spi2-registryd uses these, but are not implemented here yet:
#   org.gnome.SessionManager iface, SessionOver method
#   org.gnome.SessionManager.ClientPrivate iface, QueryEndSession signal
#   org.gnome.SessionManager.ClientPrivate iface, Stop signala


# org.gnome.SessionManager interface, IsSessionRunning method
@dbus.service.method(MAIN_IFACE, in_signature='', out_signature='b')
def IsSessionRunning(self):
    return self.is_running

# org.gnome.SessionManager interface, RegisterClient method
@dbus.service.method(MAIN_IFACE, in_signature='ss', out_signature='o')
def RegisterClient(self, app_id, client_startup_id):
    client_name = f"client_{self.client_serial_number}"
    self.client_serial_number += 1

    path = '/org/gnome/SessionManager/MockClientPrivate/' + client_name
    self.AddObject(path, CLIENT_PRIVATE_IFACE, {},
                   [
                       ('EndSessionResponse', 'bs', '', ''),
                   ])
    return path

# org.gnome.SessionManager interface, Logout method
@dbus.service.method(MAIN_IFACE, in_signature='u', out_signature='')
def Logout(self, logout_type):
    # only Normal logout is supported in this mock
    assert logout_type == 0

    for path in dbusmock.get_objects():
        if 'MockClientPrivate/' in path:
            obj = dbusmock.get_object(path)
            obj.EmitSignal(CLIENT_PRIVATE_IFACE, 'EndSession', 'u', [ 0 ])
            obj.EmitSignal(CLIENT_PRIVATE_IFACE, 'Stop', '', [])

# org.freedesktop.DBus.Mock interface, SetSessionRunning method
#
# at-spi2-registryd needs to know the difference between the session
# being in the "running" state or not, so this method is a way to be able to change that from
# the test code.
#
# This will change the internal state and emit the SessionRunning signal
# on the main interface, since at-spi2-registryd handles that signal.
@dbus.service.method(MOCK_IFACE, in_signature='b', out_signature='')
def SetSessionRunning(self, is_running):
    self.is_running = is_running
    self.EmitSignal(MAIN_IFACE, "SessionRunning", "", ())
