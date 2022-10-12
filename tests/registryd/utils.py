import pytest
import dbus

PROPERTIES_IFACE = 'org.freedesktop.DBus.Properties'

def get_property(proxy, iface_name, prop_name):
    return proxy.Get(iface_name, prop_name, dbus_interface=PROPERTIES_IFACE)

def check_unknown_property_yields_error(obj, iface):
    with pytest.raises(dbus.exceptions.DBusException):
        get_property(obj, iface, 'NonexistentProperty')
