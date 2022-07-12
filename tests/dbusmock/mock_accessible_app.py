import dbus
import dbus.service

PROPERTIES_IFACE = 'org.freedesktop.DBus.Properties'
ACCESSIBLE_IFACE = 'org.a11y.atspi.Accessible'
APPLICATION_IFACE = 'org.a11y.atspi.Application'
SOCKET_IFACE = 'org.a11y.atspi.Socket'

ROOT_PATH = '/org/a11y/atspi/accessible/root'

def accessible_get_property(prop_name):
    if prop_name == 'Name':
        pass # s
    elif prop_name == 'Description':
        pass # s
    elif prop_name == 'Parent':
        pass # (so)
    elif prop_name == 'ChildCount':
        pass # i
    elif prop_name == 'Locale':
        pass # s
    elif prop_name == 'AccessibleId':
        pass # s
    else:
        raise ValueError(f'unknown property name {prop_name}')

def application_get_property(prop_name):
    return None

@dbus.service.method(PROPERTIES_IFACE, in_signature='ss', out_signature='v')
def Get(self, iface_name, prop_name):
    if iface_name == ACCESSIBLE_IFACE:
        return accessible_get_property(prop_name)
    elif iface_name == PROPERTY_IFACE:
        return application_get_property(prop_name)

@dbus.service.method(PROPERTIES_IFACE, in_signature='ssv', out_signature='')
def Set(self, iface_name, prop_name, value):
    if iface_name == APPLICATION_IFACE:
        pass # FIXME

def get_accesssibility_bus_address():
    bus = dbus.SessionBus()
    bus_launcher = bus.get_object('org.a11y.Bus', '/org/a11y/bus')
    return str(bus_launcher.GetAddress(dbus_interface='org.a11y.Bus'))

def get_registry_root(a11y_bus):
    return a11y_bus.get_object('org.a11y.atspi.Registry', ROOT_PATH)

class MyObject(dbus.service.Object):
    def __init__(self, a11y_bus, path):
        dbus.service.Object.__init__(self, a11y_bus, path)

if __name__ == '__main__':
    from dbus.mainloop.glib import DBusGMainLoop
    from gi.repository import GLib

    DBusGMainLoop(set_as_default=True)
    main_loop = GLib.MainLoop()

    a11y_address = get_accesssibility_bus_address()
    a11y_bus = dbus.bus.BusConnection(a11y_address)

    registry = get_registry_root(a11y_bus)
    assert registry is not None

    my_unique_name = a11y_bus.get_unique_name()
    my_root = MyObject(a11y_bus, ROOT_PATH)
    
    (registry_bus_name, registry_root_path) = registry.Embed((my_unique_name, my_root), dbus_interface=SOCKET_IFACE)

    print("registry bus name:", registry_bus_name)
    print("registry_root_path:", registry_root_path)

    # main_loop.run()
