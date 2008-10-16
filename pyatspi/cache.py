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
                        'parent',
                        'interfaces',
                        'children',
                        'role',
                        'name',
                        'description',
                    ]

        def __init__(self, data):
                self._update(data)

        def _update(self, data):
                #Don't cache the path here, used as lookup in cache object dict.
                (path,
                self.parent,
                self.children,
                self.interfaces,
                self.name,
                self.role,
                self.description) = data

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
        _UPDATE_SIGNAL = 'updateTree'

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

                self._signalMatch = itf.connect_to_signal(self._UPDATE_SIGNAL, self._update_handler)

                obj = connection.get_object(self._bus_name, self._PATH, introspect=False)
                itf = _dbus.Interface(obj, self._INTERFACE)

                self._root = itf.getRoot()

        def __getitem__(self, key):
                return self._objects[key]

        def __contains__(self, key):
                return key in self._objects

        def _update_cache_dispatch_events(self, cachedata, data):
                (path,
                 parent,
                 children,
                 interfaces,
                 name,
                 role,
                 description) = data

                # TODO The 'self._registry._cache' statement makes me think
                # I have serious modularization FAIL here. 

                if name != cachedata.name:
                        event = _Event(self._registry._cache,
                                       path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "property-change",
                                       ("name", 0, 0, name))
                        self._registry._notifyNameChange(event)

                if description != cachedata.description:
                        event = _Event(self._registry._cache,
                                       path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "property-change",
                                       ("description", 0, 0, description))
                        self._registry._notifyDescriptionChange(event)

                if parent != cachedata.parent:
                        event = _Event(self._registry._cache,
                                       path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "property-change",
                                       ("parent", 0, 0, ""))
                        self._registry._notifyParentChange(event)

                if children != cachedata.children:
                        event = _Event(self._registry._cache,
                                       path,
                                       self._bus_name,
                                       "org.freedesktop.atspi.Event.Object",
                                       "children-changed",
                                       ("", 0, 0, ""))
                        self._registry._notifyChildrenChange(event)

                cachedata._update(data)

        def _update_handler(self, update, remove):
                self._remove_objects(remove)
                self._update_objects(update)

        def _update_objects(self, objects):
                for data in objects:
                        #First element is the object path.
                        path = data[0]
                        if path in self._objects:
                                cachedata = self._objects[path]
                                self._update_cache_dispatch_events(cachedata, data)
                        else:
                                self._objects[path] = _CacheData(data)

        def _remove_objects(self, paths):
                for path in paths:
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
