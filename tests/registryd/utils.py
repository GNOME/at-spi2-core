PROPERTIES_IFACE = 'org.freedesktop.DBus.Properties'

def get_property(proxy, iface_name, prop_name):
    return proxy.Get(iface_name, prop_name, dbus_interface=PROPERTIES_IFACE)
