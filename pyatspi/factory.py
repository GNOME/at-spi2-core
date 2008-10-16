#Copyright (C) 2008 Codethink Ltd

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License version 2 as published by the Free Software Foundation.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU Lesser General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#------------------------------------------------------------------------------

class AccessibleFactory(object):
        __accessible_interfaces = {}

        def create_accessible(self, cache, app_name, acc_path, interface, dbus_object=None, connection=None, parent=None):
                class_ = self.__accessible_interfaces[interface]
                return class_(cache,
                              app_name,
                              acc_path,
                              interface,
                              dbus_object=dbus_object,
                              connection=connection,
                              parent=parent)

        def add_accessible_class(self, name, cls):
                self.__accessible_interfaces[name] = cls

_factory = AccessibleFactory()

def create_accessible(cache, app_name, acc_path, interface, dbus_object=None, connection=None, parent=None):
        """
        Used to create different python classes for each of the accessible interfaces.

        The decision on which class to create is based on the name of the
        accessible interface.

        cache - ApplicationCache, where the cached data for the accessible can be obtained.
        app_name - D-Bus bus name of the application this accessible belongs to.
        acc_path - D-Bus object path of the server side accessible object.
        interface - D-Bus interface of the object. Used to decide which accessible class to instanciate.
        dbus_object(kwarg) - The D-Bus proxy object used by the accessible for D-Bus method calls.
        connection(kwarg) - Client side D-Bus connection, provided if no D-Bus proxy is available.
        """
        return _factory.create_accessible(cache,
                                          app_name,
                                          acc_path,
                                          interface,
                                          dbus_object=dbus_object,
                                          connection=connection,
                                          parent=parent)

def add_accessible_class(name, cls):
        _factory.add_accessible_class(name, cls)

#END----------------------------------------------------------------------------
