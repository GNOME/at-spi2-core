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

import dbus
from dbus.proxies import Interface
from dbus.exceptions import *

import interfaces

__all__ = [
           "AccessibleObjectNoLongerExists",
           "AccessibleObjectNotAvailable",
           "Enum",
           "BaseProxy",
           "_repack_tuple",
          ]

class AccessibleObjectNoLongerExists(Exception):
        pass

class AccessibleObjectNotAvailable(Exception):
        pass

#------------------------------------------------------------------------------

def _repack_tuple (tup):
        """
        Re-packs a tuple moving the last element to the beginning.
        """
        return (tup[-1] ,) + tup[:-1]

#------------------------------------------------------------------------------

class Enum(dbus.UInt32):
        def __str__(self):
                return self._enum_lookup[int(self)]

#------------------------------------------------------------------------------


class BaseProxyMeta(type):
        def __new__(meta, *args, **kwargs):
                cls = type.__new__(meta, *args, **kwargs)

                queryable_interfaces = { 
                        'Accessible':interfaces.ATSPI_ACCESSIBLE,
                        'Action':interfaces.ATSPI_ACTION,
                        'Application':interfaces.ATSPI_APPLICATION,
                        'Collection':interfaces.ATSPI_COLLECTION,
                        'Component':interfaces.ATSPI_COMPONENT,
                        'Desktop':interfaces.ATSPI_DESKTOP,
                        'Document':interfaces.ATSPI_DOCUMENT,
                        'EditableText':interfaces.ATSPI_EDITABLE_TEXT,
                        'Hypertext':interfaces.ATSPI_HYPERTEXT,
                        'Hyperlink':interfaces.ATSPI_HYPERLINK,
                        'Image':interfaces.ATSPI_IMAGE,
                        'Selection':interfaces.ATSPI_SELECTION,
                        'StreamableContent':interfaces.ATSPI_STREAMABLE_CONTENT,
                        'Table':interfaces.ATSPI_TABLE,
                        'Text':interfaces.ATSPI_TEXT,
                        'Value':interfaces.ATSPI_VALUE,
                }

                def return_query(interface):
                        def new_query(self):
                                return self.queryInterface(interface)
                        return new_query

                for interface in queryable_interfaces.keys():
                        name = 'query%s' % interface
                        setattr(cls, name, return_query(queryable_interfaces[interface])) 

                return cls

#------------------------------------------------------------------------------

class BaseProxy(object):
        """
        The base D-Bus proxy for a remote object that implements one or more
        of the AT-SPI interfaces.
        """

        __metaclass__ = BaseProxyMeta

        def __init__(self, app_name, acc_path, cache, interface, dbus_object=None):
                """
                Create a D-Bus Proxy for an ATSPI interface.

                cache - ApplicationCache, where the cached data for the accessible can be obtained.
                app_name - D-Bus bus name of the application this accessible belongs to.
                acc_path - D-Bus object path of the server side accessible object.
                parent - Parent accessible.
                dbus_object(kwarg) - The D-Bus proxy object used by the accessible for D-Bus method calls.
                """
                self._cache = cache
                self._app_name = app_name
                self._acc_path = acc_path
                self._dbus_interface = interface

                if not dbus_object:
                        dbus_object = cache.connection.get_object(self._app_name,
                                                                  self._acc_path,
                                                                  introspect=False)
                self._dbus_object = dbus_object

                self._pgetter = self.get_dbus_method("Get",
                                                     dbus_interface="org.freedesktop.DBus.Properties")
                self._psetter = self.get_dbus_method("Set",
                                                     dbus_interface="org.freedesktop.DBus.Properties")

        def __str__(self):
                    try:
                              return '[%s | %s]' % (self.getRoleName(), self.name)
                    except Exception:
                              return '[DEAD]'

        def __eq__(self, other):
                if other is None:
                        return False
                try:
                        if self._app_name == other._app_name and \
                           self._acc_path == other._acc_path:
                                return True
                        else:
                                return False
                except AttributeError:
                        return False

        def __ne__(self, other):
                return not self.__eq__(other)

        def __hash__(self):
                return hash(self._app_name + self._acc_path)

        def get_dbus_method(self, *args, **kwargs):
                method =  self._dbus_object.get_dbus_method(*args, **kwargs)

                def dbus_method_func(*iargs, **ikwargs):
                        # TODO Need to throw an AccessibleObjectNoLongerExists exception
                        # on D-Bus error of the same type.
                        try:
                                return method(*iargs, **ikwargs)
                        except UnknownMethodException, e:
                                raise NotImplementedError(e)
                        except DBusException, e:
                                raise LookupError(e)

                return dbus_method_func

        @property
        def cached_data(self):
                try:
                        return self._cache.get_cache_data(self._app_name, self._acc_path)
                except KeyError:
                        raise AccessibleObjectNoLongerExists, \
                                'Cache data cannot be found for path %s in app %s' % (self._acc_path, self._app_name)

        @property
        def interfaces(self):
                return self.cached_data.interfaces

        def queryInterface(self, interface):
                """
                Gets a different accessible interface for this object
                or raises a NotImplemented error if the given interface
                is not supported.
                """
                if interface in self.interfaces:
                        return self._cache.create_accessible(self._app_name,
                                                             self._acc_path,
                                                             interface,
                                                             dbus_object=self._dbus_object)
                else:
                        raise NotImplementedError(
                                "%s not supported by accessible object at path %s"
                                % (interface, self._acc_path))

#END----------------------------------------------------------------------------
