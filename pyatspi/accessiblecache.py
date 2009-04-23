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

import dbus as _dbus

from event import Event as _Event

#------------------------------------------------------------------------------

class _CacheData(object):
        __slots__ = [
                        'path',
                        'parent',
                        'interfaces',
                        'children',
                        'role',
                        'name',
                        'description',
                        'state',
                    ]

        def __init__(self, data):
                self._update(data)

        def _update(self, data):
                (self.path,
                 self.parent,
                 self.children,
                 self.interfaces,
                 self.name,
                 self.role,
                 self.description,
                 self.state) = data

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
                itf = _dbus.Interface(obj, self._INTERFACE)

                self._objects = {}

                get_method = itf.get_dbus_method(self._GET_METHOD)
                self._update_objects(get_method())

                self._updateMatch = itf.connect_to_signal(self._UPDATE_SIGNAL, self._update_single)
                self._removeMatch = itf.connect_to_signal(self._REMOVE_SIGNAL, self._remove_object)

                obj = connection.get_object(self._bus_name, self._PATH, introspect=False)
                itf = _dbus.Interface(obj, self._INTERFACE)

                self._root = itf.getRoot()

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

        def _remove_object(self, paths):
                # TODO I'm squashing a possible error here
                # I've seen things appear to be deleted twice
                # which needs investigation
                try:
                        del(self._objects[path])
                except KeyError:
                        pass

        def _get_root(self):
                return self._root

        root = property(fget=_get_root)

#END---------------------------------------------------------------------------
