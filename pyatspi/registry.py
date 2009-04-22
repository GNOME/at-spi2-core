#Copyright (C) 2008 Codethink Ltd
#copyright: Copyright (c) 2005, 2007 IBM Corporation

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

#Portions of this code originally licensed and copyright (c) 2005, 2007
#IBM Corporation under the BSD license, available at
#U{http://www.opensource.org/licenses/bsd-license.php}

#authors: Peter Parente, Mark Doffman

import os as _os
import dbus as _dbus
import gobject as _gobject

from desktop import Desktop as _Desktop

from event import EventType as _EventType
from event import event_type_to_signal_reciever as _event_type_to_signal_reciever

from applicationcache import TestApplicationCache, ApplicationCache

from dbus.mainloop.glib import DBusGMainLoop as _DBusGMainLoop

from Queue import Queue
from deviceevent import *
from deviceevent import _TestDeviceEventController

_DBusGMainLoop(set_as_default=True)

#------------------------------------------------------------------------------

class _Registry(object):
        """
        Wraps the Accessibility.Registry to provide more Pythonic registration for
        events.

        This object should be treated as a singleton, but such treatment is not
        enforced. You can construct another instance of this object and give it a
        reference to the Accessibility.Registry singleton. Doing so is harmless and
        has no point.

        @ivar async: Should event dispatch to local listeners be decoupled from event
                receiving from the registry?
        @type async: boolean
        @ivar reg: Reference to the real, wrapped registry object
        @type reg: Accessibility.Registry
        @ivar dev: Reference to the device controller
        @type dev: Accessibility.DeviceEventController
        @ivar queue: Queue of events awaiting local dispatch
        @type queue: Queue.Queue
        @ivar clients: Map of event names to client listeners
        @type clients: dictionary
        @ivar observers: Map of event names to AT-SPI L{_Observer} objects
        @type observers: dictionary
        """

        def __init__(self):
                """
                Stores a reference to the AT-SPI registry. Gets and stores a reference
                to the DeviceEventController.

                @param reg: Reference to the AT-SPI registry daemon
                @type reg: Accessibility.Registry
                """
                self._bus = _dbus.SessionBus()

                app_name = None
                if "ATSPI_TEST_APP_NAME" in _os.environ.keys():
                        app_name = _os.environ["ATSPI_TEST_APP_NAME"]

                if app_name:
                        self._app_cache = TestApplicationCache(self, self._bus, app_name)
                        self.dev = _TestDeviceEventController()
                else:
                        self._app_cache = ApplicationCache(self, self._bus)
                        self.dev = DeviceEventController(self._bus)

                self._event_listeners = {}

                # All of this special casing is for the 'faked'
                # events caused by cache updates.

                self._name_type = _EventType("object:property-change:name")
                self._name_listeners = {}
                self._description_type = _EventType("object:property-change:description")
                self._description_listeners = {}
                self._parent_type = _EventType("object:property-change:parent")
                self._parent_listeners = {}
                self._children_changed_type = _EventType("object:children-changed")
                self._children_changed_listeners = {}

                self.clients = {}
                self.deviceClients = {}

        def __call__(self):
                """
                @return: This instance of the registry
                @rtype: L{Registry}
                """
                return self

        @property
        def cache (self):
                """
                This is the accessible application cache through which
                all accessible objects are accessed.
                """
                return self._app_cache

        def start(self, async=False, gil=True):
                """
                Enter the main loop to start receiving and dispatching events.

                @param async: Should event dispatch be asynchronous (decoupled) from 
                        event receiving from the AT-SPI registry?
                @type async: boolean
                @param gil: Add an idle callback which releases the Python GIL for a few
                        milliseconds to allow other threads to run? Necessary if other threads
                        will be used in this process.
                        Note - No Longer used.
                @type gil: boolean
                """
                self._loop = _gobject.MainLoop()
                try:
                        self._loop.run()
                except KeyboardInterrupt:
                        pass

        def stop(self, *args):
                """Quits the main loop."""
                self._loop.quit()
                self.flushEvents()

        def getDesktopCount(self):
                """
                Gets the number of available desktops.

                @return: Number of desktops
                @rtype: integer
                """
                return 1

        def getDesktop(self, i):
                """
                Gets a reference to the i-th desktop.

                @param i: Which desktop to get
                @type i: integer
                @return: Desktop reference
                @rtype: Accessibility.Desktop
                """
                return _Desktop(self._app_cache)

        # -------------------------------------------------------------------------------

        def _callClients(self, register, event):
                for client in register.keys():
                        client(event)

        def _notifyNameChange(self, event):
                self._callClients(self._name_listeners, event)

        def _notifyDescriptionChange(self, event):
                self._callClients(self._description_listeners, event)

        def _notifyParentChange(self, event):
                self._callClients(self._parent_listeners, event)

        def _notifyChildrenChange(self, event):
                self._callClients(self._children_changed_listeners, event)

        def _registerFake(self, type, register, client, *names):
                """
                Registers a client from a register of clients
                for 'Fake' events emitted by the cache.
                """
                try:
                        registered = register[client]
                except KeyError:
                        registered = []
                        register[client] = registered

                for name in names:
                        new_type = _EventType(name)
                        if new_type.is_subtype(type):
                                registered.append(new_type.name)

                if registered == []:
                        del(register[client])

        def _deregisterFake(self, type, register, client, *names):
                """
                Deregisters a client from a register of clients
                for 'Fake' events emitted by the cache.
                """
                try:
                        registered = register[client]
                except KeyError:
                        return True

                for name in names:
                        remove_type = _EventType(name)

                        copy = registered[:]
                        for i in range(0, len(copy)):
                                type_name = copy[i]
                                registered_type = _EventType(type_name)

                                if remove_type.is_subtype(registered_type):
                                        del(registered[i])

                if registered == []:
                        del(register[client])

        # -------------------------------------------------------------------------------

        def registerEventListener(self, client, *names):
                """
                Registers a new client callback for the given event names. Supports 
                registration for all subevents if only partial event name is specified.
                Do not include a trailing colon.

                For example, 'object' will register for all object events, 
                'object:property-change' will register for all property change events,
                and 'object:property-change:accessible-parent' will register only for the
                parent property change event.

                Registered clients will not be automatically removed when the client dies.
                To ensure the client is properly garbage collected, call 
                L{deregisterEventListener}.

                @param client: Callable to be invoked when the event occurs
                @type client: callable
                @param names: List of full or partial event names
                @type names: list of string
                """
                try:
                        registered = self._event_listeners[client]
                except KeyError:
                        registered = []
                        self._event_listeners[client] = registered

                for name in names:
                        new_type = _EventType(name)
                        registered.append((new_type.name,
                                           _event_type_to_signal_reciever(self._bus, self._app_cache, client, new_type)))

                self._registerFake(self._name_type, self._name_listeners, client, *names)
                self._registerFake(self._description_type, self._description_listeners, client, *names)
                self._registerFake(self._parent_type, self._parent_listeners, client, *names)
                self._registerFake(self._children_changed_type, self._children_changed_listeners, client, *names)

        def deregisterEventListener(self, client, *names):
                """
                Unregisters an existing client callback for the given event names. Supports 
                unregistration for all subevents if only partial event name is specified.
                Do not include a trailing colon.

                This method must be called to ensure a client registered by
                L{registerEventListener} is properly garbage collected.

                @param client: Client callback to remove
                @type client: callable
                @param names: List of full or partial event names
                @type names: list of string
                @return: Were event names specified for which the given client was not
                        registered?
                @rtype: boolean
                """
                try:
                        registered = self._event_listeners[client]
                except KeyError:
                        # Presumably if were trying to deregister a client with
                        # no names then the return type is always true.
                        return True

                missing = False

                for name in names:
                        remove_type = _EventType(name)
                        copy = registered[:]
                        for i in range (0, len(copy)):
                                type_name, signal_match = copy[i]
                                registered_type = _EventType(type_name)

                                if remove_type.is_subtype(registered_type):
                                        signal_match.remove()
                                        del(registered[i])
                                else:
                                        missing = True

                if registered == []:
                        del(self._event_listeners[client])

                #TODO Do these account for missing also?
                self._deregisterFake(self._name_type, self._name_listeners, client, *names)
                self._deregisterFake(self._description_type, self._description_listeners, client, *names)
                self._deregisterFake(self._parent_type, self._parent_listeners, client, *names)
                self._deregisterFake(self._children_changed_type, self._children_changed_listeners, client, *names)

                return missing

        # -------------------------------------------------------------------------------

        def registerKeystrokeListener(self,
                                      client,
                                      key_set=[],
                                      mask=0,
                                      kind=(KEY_PRESSED_EVENT, KEY_RELEASED_EVENT),
                                      synchronous=True,
                                      preemptive=True,
                                      global_=False):
                """
                Registers a listener for key stroke events.

                @param client: Callable to be invoked when the event occurs
                @type client: callable
                @param key_set: Set of hardware key codes to stop monitoring. Leave empty
                        to indicate all keys.
                @type key_set: list of integer
                @param mask: When the mask is None, the codes in the key_set will be 
                        monitored only when no modifier is held. When the mask is an 
                        integer, keys in the key_set will be monitored only when the modifiers in
                        the mask are held. When the mask is an iterable over more than one 
                        integer, keys in the key_set will be monitored when any of the modifier
                        combinations in the set are held.
                @type mask: integer, iterable, None
                @param kind: Kind of events to watch, KEY_PRESSED_EVENT or 
                        KEY_RELEASED_EVENT.
                @type kind: list
                @param synchronous: Should the callback notification be synchronous, giving
                        the client the chance to consume the event?
                @type synchronous: boolean
                @param preemptive: Should the callback be allowed to preempt / consume the
                        event?
                @type preemptive: boolean
                @param global_: Should callback occur even if an application not supporting
                        AT-SPI is in the foreground? (requires xevie)
                @type global_: boolean
                """
                try:
                        # see if we already have an observer for this client
                        ob = self.deviceClients[client]
                except KeyError:
                        # create a new device observer for this client
                        ob = KeyboardDeviceEventListener(self, synchronous, preemptive, global_)
                        # store the observer to client mapping, and the inverse
                        self.deviceClients[ob] = client
                        self.deviceClients[client] = ob
                if mask is None:
                        # None means all modifier combinations
                        mask = utils.allModifiers()
                # register for new keystrokes on the observer
                ob.register(self.dev, key_set, mask, kind)

        def deregisterKeystrokeListener(self,
                                        client,
                                        key_set=[],
                                        mask=0,
                                        kind=(KEY_PRESSED_EVENT, KEY_RELEASED_EVENT)):
                """
                Deregisters a listener for key stroke events.

                @param client: Callable to be invoked when the event occurs
                @type client: callable
                @param key_set: Set of hardware key codes to stop monitoring. Leave empty
                        to indicate all keys.
                @type key_set: list of integer
                @param mask: When the mask is None, the codes in the key_set will be 
                        monitored only when no modifier is held. When the mask is an 
                        integer, keys in the key_set will be monitored only when the modifiers in
                        the mask are held. When the mask is an iterable over more than one 
                        integer, keys in the key_set will be monitored when any of the modifier
                        combinations in the set are held.
                @type mask: integer, iterable, None
                @param kind: Kind of events to stop watching, KEY_PRESSED_EVENT or 
                        KEY_RELEASED_EVENT.
                @type kind: list
                @raise KeyError: When the client isn't already registered for events
                """
                # see if we already have an observer for this client
                ob = self.deviceClients[client]
                if mask is None:
                        # None means all modifier combinations
                        mask = utils.allModifiers()
                # register for new keystrokes on the observer
                ob.unregister(self.dev, key_set, mask, kind)

        def handleDeviceEvent(self, event, ob):
                """
                Dispatches L{event.DeviceEvent}s to registered clients. Clients are called
                in the order they were registered for the given AT-SPI event. If any
                client returns True, callbacks cease for the event for clients of this registry 
                instance. Clients of other registry instances and clients in other processes may 
                be affected depending on the values of synchronous and preemptive used when invoking
                L{registerKeystrokeListener}. 

                @note: Asynchronous dispatch of device events is not supported.

                @param event: AT-SPI device event
                @type event: L{event.DeviceEvent}
                @param ob: Observer that received the event
                @type ob: L{KeyboardDeviceEventListener}

                @return: Should the event be consumed (True) or allowed to pass on to other
                        AT-SPI observers (False)?
                @rtype: boolean
                """
                try:
                        # try to get the client registered for this event type
                        client = self.deviceClients[ob]
                except KeyError:
                        # client may have unregistered recently, ignore event
                        return False
                # make the call to the client
                try:
                        return client(event) or event.consume
                except Exception:
                        # print the exception, but don't let it stop notification
                        traceback.print_exc()

        # -------------------------------------------------------------------------------

        def pumpQueuedEvents (self):
                """
                No Longer needed all application events are asyncronous.
                """
                pass

        def flushEvents (self):
                """
                No Longer needed all application events are asyncronous.
                """
                pass

        # -------------------------------------------------------------------------------

        def generateKeyboardEvent(self, keycode, keysym, kind):
                """
                Generates a keyboard event. One of the keycode or the keysym parameters
                should be specified and the other should be None. The kind parameter is 
                required and should be one of the KEY_PRESS, KEY_RELEASE, KEY_PRESSRELEASE,
                KEY_SYM, or KEY_STRING.

                @param keycode: Hardware keycode or None
                @type keycode: integer
                @param keysym: Symbolic key string or None
                @type keysym: string
                @param kind: Kind of event to synthesize
                @type kind: integer
                """
                if keysym is None:
                        self.dev.generateKeyboardEvent(keycode, '', kind)
                else:
                        self.dev.generateKeyboardEvent(None, keysym, kind)

        def generateMouseEvent(self, x, y, name):
                """
                Generates a mouse event at the given absolute x and y coordinate. The kind
                of event generated is specified by the name. For example, MOUSE_B1P 
                (button 1 press), MOUSE_REL (relative motion), MOUSE_B3D (butten 3 
                double-click).

                @param x: Horizontal coordinate, usually left-hand oriented
                @type x: integer
                @param y: Vertical coordinate, usually left-hand oriented
                @type y: integer
                @param name: Name of the event to generate
                @type name: string
                """
                self.dev.generateMouseEvent(x, y, name)
