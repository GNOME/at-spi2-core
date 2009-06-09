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

from accessiblecache import AccessibleCache
from desktop import Desktop, DESKTOP_PATH
from factory import accessible_factory
from event import Event as _Event
from base import AccessibleObjectNotAvailable

from interfaces import *

__all__ = [
           "ApplicationCache",
           "TestApplicationCache",
          ]

#------------------------------------------------------------------------------

ROOT_PATH    = '/org/freedesktop/atspi/accessible/root'

#------------------------------------------------------------------------------

class TestApplicationCache(object):

        """
        Test application store, accesses a single application.

        The store object acts as a central class for creating accessible objects.
        It interfaces with the ATSPI registry to keep account of all accessible
        applications. It contains the accessible cache objects from each application.

        @registry:   Each accessible cache object must have a reference to the registry
                     object to send update events.

        @connection: D-Bus connection used to access applications.

        @bus_name:   The test store only accesses one accessible application, this is its
                     D-Bus path.
        """

        def __init__(self, registry, connection, bus_name):
                self._connection = connection

                self.application_list = [bus_name]
                self.application_cache = {bus_name:AccessibleCache(registry, connection, bus_name)}

        def get_cache_data(self, app_name, acc_path):
                """
                Returns the cache tuple for the given application and accessible
                object path. Throws an IndexError if the cache data is not found.
                """
                return self.application_cache[app_name][acc_path]

        def create_application(self, app_name):
                """
                Creates an accessible object for the root of the application
                available at the given D-Bus name.
                """
                cls = accessible_factory.get_accessible_class(ATSPI_APPLICATION)
                try:
                        return cls(app_name, self.application_cache[app_name].root, self, ATSPI_APPLICATION)
                except KeyError:
                        raise AccessibleObjectNotAvailable ()

        def create_accessible(self, app_name, acc_path, interface, dbus_object=None):
                """
                Creates an accessible object.

                @app_name: D-Bus name of the application where the accessible object resides.

                @acc_path: D-Bus path of the object within the application.

                @interface: D-Bus interface of the requested object. A different accessible object
                            class will be created depending on this. Making the function much like 
                            an accessible object factory.

                @dbus_object: If a D-Bus object already exists for the accessible object it can be
                              provided here so that another one is not created.
                """
                # An acc_path of '/' implies the desktop object, whatever the app_name.
                if acc_path == DESKTOP_PATH:
                        return Desktop(self)
                if acc_path == ROOT_PATH:
                        return None
                else:
                        cls = accessible_factory.get_accessible_class(interface)
                        try:
                                return cls(app_name, acc_path, self, interface, dbus_object=dbus_object)
                        except KeyError:
                                raise AccessibleObjectNotAvailable ()

        @property
        def connection(self):
                """
                D-Bus connection used by the store.
                """
                return self._connection

#------------------------------------------------------------------------------

def _list_items_added_removed (l1, l2):
        """
        Returns a tuple (boolean, boolean).
        The first value indicates if, when
        moving from l1 to l2, any items have been added.
        The second value indicates whether any items have
        been removed.
        """
        l1notl2 = [item for item in l1 if item not in l2]
        l2notl1 = [item for item in l2 if item not in l1]
        return ((len(l1notl2) > 0), (len(l2notl1) > 0))

#------------------------------------------------------------------------------

class AccessibleCache(object):
        """
        There is one accessible cache per application.
        For each application the accessible cache stores
        data on every accessible object within the app.

        It also acts as the factory for creating client
        side proxies for these accessible objects.

        connection - DBus connection.
        busName    - Name of DBus connection where cache interface resides.
        """

        _PATH = '/org/freedesktop/atspi/tree'
        _INTERFACE = 'org.freedesktop.atspi.Tree'
        _GET_METHOD = 'getTree'
        _UPDATE_SIGNAL = 'updateAccessible'
        _REMOVE_SIGNAL = 'removeAccessible'

        def __init__(self, registry, connection, bus_name):
                """
                Creates a cache.

                connection - DBus connection.
                busName    - Name of DBus connection where cache interface resides.
                """
                self._registry = registry
                self._connection = connection
                self._bus_name = bus_name

                obj = connection.get_object(bus_name, self._PATH, introspect=False)
                self._tree_itf = _dbus.Interface(obj, self._INTERFACE)

                self._objects = {}

                get_method = self._tree_itf.get_dbus_method(self._GET_METHOD)
                self._update_objects(get_method())

                self._updateMatch = self._tree_itf.connect_to_signal(self._UPDATE_SIGNAL, self._update_single)
                self._removeMatch = self._tree_itf.connect_to_signal(self._REMOVE_SIGNAL, self._remove_object)

                self._root = self._tree_itf.getRoot()

        def __getitem__(self, key):
                return self._objects[key]

        def __contains__(self, key):
                return key in self._objects

        def _dispatch_event(self, olddata, newdata):
                if olddata.name != newdata.name:
                        event = _Event(self._registry.cache,
                                       newdata.path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "property-change",
                                       ("accessible-name", 0, 0, newdata.name))
                        self._registry._notifyNameChange(event)

                if olddata.description != newdata.description:
                        event = _Event(self._registry.cache,
                                       newdata.path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "property-change",
                                       ("accessible-description", 0, 0, newdata.description))
                        self._registry._notifyDescriptionChange(event)

                if olddata.parent != newdata.parent:
                        event = _Event(self._registry.cache,
                                       newdata.path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "property-change",
                                       ("accessible-parent", 0, 0, ""))
                        self._registry._notifyParentChange(event)

                removed, added = _list_items_added_removed (olddata.children, newdata.children)

                if added:
                        event = _Event(self._registry.cache,
                                       newdata.path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "children-changed",
                                       ("add", 0, 0, ""))
                        self._registry._notifyChildrenChange(event)

                if removed:
                        event = _Event(self._registry.cache,
                                       newdata.path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "children-changed",
                                       ("remove", 0, 0, ""))
                        self._registry._notifyChildrenChange(event)

        # TODO This should be the other way around. Single is more common than many.
        def _update_single(self, object):
                self._update_objects ([object])

        def _update_objects(self, objects):
                cache_update_objects = []
                for data in objects:
                        #First element is the object path.
                        path = data[0]
                        if path in self._objects:
                                olddata = self._objects[path]
                                newdata = _CacheData(data)
                                cache_update_objects.append((olddata, newdata))
                                self._objects[path] = newdata
                        else:
                                self._objects[path] = _CacheData(data)
                for old, new in cache_update_objects:
                        self._dispatch_event(old, new)

        def _remove_object(self, path):
                # TODO I'm squashing a possible error here
                # I've seen things appear to be deleted twice
                # which needs investigation
                try:
                        del(self._objects[path])
                except KeyError:
                        pass

        def _get_root(self):
                return self._root

        def _refresh(self):
                get_method = self._tree_itf.get_dbus_method(self._GET_METHOD)
                self._update_objects(get_method())

        root = property(fget=_get_root)

#END---------------------------------------------------------------------------

class ApplicationCache(object):
        """
        Test application store, accesses a single application.

        The store object acts as a central class for creating accessible objects.
        It interfaces with the ATSPI registry to keep account of all accessible
        applications. It contains the accessible cache objects from each application.

        @registry:   Each accessible cache object must have a reference to the registry
                     object to send update events.

        @connection: D-Bus connection used to access applications.

        @bus_name:   The test store only accesses one accessible application, this is its
                     D-Bus path.
        """

        _APPLICATIONS_ADD = 1
        _APPLICATIONS_REMOVE = 0

        def __init__(self, registry, connection):
                self._connection = connection
                self._registry = registry

                self.application_list = []
                self.application_cache = {}

                self._regsig = connection.add_signal_receiver(self.update_handler,
                                                              dbus_interface=ATSPI_REGISTRY_INTERFACE,
                                                              signal_name="updateApplications")

                obj = connection.get_object(ATSPI_REGISTRY_NAME,
                                            ATSPI_REGISTRY_PATH,
                                            introspect=False)
                self._app_register = dbus.Interface(obj, ATSPI_REGISTRY_INTERFACE)

                self.application_list.extend(self._app_register.getApplications())
                for bus_name in self.application_list:
                        self.application_cache[bus_name] = AccessibleCache(self._registry, self._connection, bus_name)

        def update_handler (self, update_type, bus_name):
                if update_type == ApplicationCache._APPLICATIONS_ADD:
                        #TODO Check that app does not already exist
                        #TODO Excuding this app is a hack, need to have re-entrant method calls.
                        if bus_name != self._connection.get_unique_name ():
                                self.application_list.append(bus_name)
                                self.application_cache[bus_name] = AccessibleCache(self._registry,
                                                                                   self._connection,
                                                                                   bus_name)
                                event = _Event(self,
                                               DESKTOP_PATH,
                                               ATSPI_REGISTRY_NAME,
                                               "org.freedesktop.atspi.Event.Object",
                                               "children-changed",
                                               ("add", 0, 0, ""))
                elif update_type == ApplicationCache._APPLICATIONS_REMOVE:
                        #TODO Fail safely if app does not exist
                        self.application_list.remove(bus_name)
                        del(self.application_cache[bus_name])
                        event = _Event(self,
                                       DESKTOP_PATH,
                                       ATSPI_REGISTRY_NAME,
                                       "org.freedesktop.atspi.Event.Object",
                                       "children-changed",
                                       ("remove", 0, 0, ""))

                self._registry._notifyChildrenChange(event)

        def get_cache_data(self, app_name, acc_path):
                """
                Returns the cache tuple for the given application and accessible
                object path. Throws an IndexError if the cache data is not found.
                """
                return self.application_cache[app_name][acc_path]

        def create_application(self, app_name):
                """
                Creates an accessible object for the root of the application
                available at the given D-Bus name.
                """
                if app_name == ATSPI_REGISTRY_NAME:
                        return Desktop(self)
                else:
                        cls = accessible_factory.get_accessible_class(ATSPI_APPLICATION)
                        try:
                                return cls(app_name, self.application_cache[app_name].root, self, ATSPI_APPLICATION)
                        except KeyError:
                                raise AccessibleObjectNotAvailable ()

        def create_accessible(self, app_name, acc_path, interface, dbus_object=None):
                """
                Creates an accessible object.

                @app_name: D-Bus name of the application where the accessible object resides.

                @acc_path: D-Bus path of the object within the application.

                @interface: D-Bus interface of the requested object. A different accessible object
                            class will be created depending on this. Making the function much like 
                            an accessible object factory.

                @dbus_object: If a D-Bus object already exists for the accessible object it can be
                              provided here so that another one is not created.
                """
                if acc_path == DESKTOP_PATH:
                        return Desktop(self)
                if acc_path == ROOT_PATH:
                        return None
                else:
                        cls = accessible_factory.get_accessible_class(interface)
                        try:
                                return cls(app_name, acc_path, self, interface, dbus_object=dbus_object)
                        except KeyError:
                                raise AccessibleObjectNotAvailable ()

        @property
        def connection(self):
                """
                D-Bus connection used by the store.
                """
                return self._connection

        def _refresh(self):
                app_addresses = self._app_register.getApplications()
                added, removed = _list_items_added_removed (self.application_list, app_addresses)
                for item in added:
                        self.update_handler (ApplicationsCache._APPLICATIONS_ADD, item):
                for item in removed:
                        self.update_handler (ApplicationsCache._APPLICATIONS_REMOVE, item):

#END----------------------------------------------------------------------------
