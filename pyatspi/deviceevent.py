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
import dbus.service as _service
import interfaces

from base import Enum as _Enum

#------------------------------------------------------------------------------

class PressedEventType(_Enum):
        _enum_lookup = {
                0:'KEY_PRESSED_EVENT',
                1:'KEY_RELEASED_EVENT',
                2:'BUTTON_PRESSED_EVENT',
                3:'BUTTON_RELEASED_EVENT',
        }

KEY_PRESSED_EVENT = PressedEventType(0)
KEY_RELEASED_EVENT = PressedEventType(1)
BUTTON_PRESSED_EVENT = PressedEventType(2)
BUTTON_RELEASED_EVENT = PressedEventType(3)

#------------------------------------------------------------------------------

class ControllerEventMask(_Enum):
        _enum_lookup = {
                1:'KEY_PRESSED_EVENT_MASK',
                2:'KEY_RELEASED_EVENT_MASK',
                4:'BUTTON_PRESSED_EVENT_MASK',
                8:'BUTTON_RELEASED_EVENT_MASK',
        }

KEY_PRESSED_EVENT_MASK = ControllerEventMask(1)
KEY_RELEASED_EVENT_MASK = ControllerEventMask(2)
BUTTON_PRESSED_EVENT_MASK = ControllerEventMask(4)
BUTTON_RELEASED_EVENT_MASK = ControllerEventMask(8)

#------------------------------------------------------------------------------

class KeyEventType(_Enum):
        _enum_lookup = {
                0:'KEY_PRESSED',
                1:'KEY_RELEASED',
        }
KEY_PRESSED = KeyEventType(0)
KEY_RELEASED = KeyEventType(1)

#------------------------------------------------------------------------------

class KeySynthType(_Enum):
        _enum_lookup = {
                0:'KEY_PRESS',
                1:'KEY_RELEASE',
                2:'KEY_PRESSRELEASE',
                3:'KEY_SYM',
                4:'KEY_STRING',
        }

KEY_PRESS = KeySynthType(0)
KEY_PRESSRELEASE = KeySynthType(2)
KEY_RELEASE = KeySynthType(1)
KEY_STRING = KeySynthType(4)
KEY_SYM = KeySynthType(3)

#------------------------------------------------------------------------------

class ModifierType(_Enum):
        _enum_lookup = {
                0:'MODIFIER_SHIFT',
                1:'MODIFIER_SHIFTLOCK',
                2:'MODIFIER_CONTROL',
                3:'MODIFIER_ALT',
                4:'MODIFIER_META',
                5:'MODIFIER_META2',
                6:'MODIFIER_META3',
                7:'MODIFIER_NUMLOCK',
        }

MODIFIER_ALT = ModifierType(3)
MODIFIER_CONTROL = ModifierType(2)
MODIFIER_META = ModifierType(4)
MODIFIER_META2 = ModifierType(5)
MODIFIER_META3 = ModifierType(6)
MODIFIER_NUMLOCK = ModifierType(7)
MODIFIER_SHIFT = ModifierType(0)
MODIFIER_SHIFTLOCK = ModifierType(1)

#------------------------------------------------------------------------------

class DeviceEvent(list):
        """
        Wraps an AT-SPI device event with a more Pythonic interface. Primarily adds
        a consume attribute which can be used to cease propagation of a device event.

        @ivar consume: Should this event be consumed and not allowed to pass on to
                observers further down the dispatch chain in this process or possibly
                system wide?
        @type consume: boolean
        @ivar type: Kind of event, KEY_PRESSED_EVENT or KEY_RELEASED_EVENT
        @type type: Accessibility.EventType
        @ivar id: Serial identifier for this key event
        @type id: integer
        @ivar hw_code: Hardware scan code for the key
        @type hw_code: integer
        @ivar modifiers: Modifiers held at the time of the key event
        @type modifiers: integer
        @ivar timestamp: Time at which the event occurred relative to some platform
                dependent starting point (e.g. XWindows start time)
        @type timestamp: integer
        @ivar event_string: String describing the key pressed (e.g. keysym)
        @type event_string: string
        @ivar is_text: Is the event representative of text to be inserted (True), or 
                of a control key (False)?
        @type is_text: boolean
        """
        def __new__(cls, type, id, hw_code, modifiers, timestamp, event_string, is_text):
                return list.__new__(cls, (type, id, hw_code, modifiers, timestamp, event_string, is_text))
        def __init__(self, type, id, hw_code, modifiers, timestamp, event_string, is_text):
                list.__init__(self, (type, id, hw_code, modifiers, timestamp, event_string, is_text))
                self.consume = False
        def _get_type(self):
                return self[0]
        def _set_type(self, val):
                self[0] = val
        type = property(fget=_get_type, fset=_set_type)
        def _get_id(self):
                return self[1]
        def _set_id(self, val):
                self[1] = val
        id = property(fget=_get_id, fset=_set_id)
        def _get_hw_code(self):
                return self[2]
        def _set_hw_code(self, val):
                self[2] = val
        hw_code = property(fget=_get_hw_code, fset=_set_hw_code)
        def _get_modifiers(self):
                return self[3]
        def _set_modifiers(self, val):
                self[3] = val
        modifiers = property(fget=_get_modifiers, fset=_set_modifiers)
        def _get_timestamp(self):
                return self[4]
        def _set_timestamp(self, val):
                self[4] = val
        timestamp = property(fget=_get_timestamp, fset=_set_timestamp)
        def _get_event_string(self):
                return self[5]
        def _set_event_string(self, val):
                self[5] = val
        event_string = property(fget=_get_event_string, fset=_set_event_string)
        def _get_is_text(self):
                return self[6]
        def _set_is_text(self, val):
                self[6] = val
        is_text = property(fget=_get_is_text, fset=_set_is_text)

        def __str__(self):
                """
                Builds a human readable representation of the event.

                @return: Event description
                @rtype: string
                """
                if self.type == constants.KEY_PRESSED_EVENT:
                        kind = 'pressed'
                elif self.type == constants.KEY_RELEASED_EVENT:
                        kind = 'released'
                return """\
%s
\thw_code: %d
\tevent_string: %s
\tmodifiers: %d
\tid: %d
\ttimestamp: %d
\tis_text: %s""" % (kind, self.hw_code, self.event_string, self.modifiers,
                self.id, self.timestamp, self.is_text)

#------------------------------------------------------------------------------

class EventListenerMode(list):
        def __new__(cls, synchronous, preemptive, global_):
                return list.__new__(cls, (synchronous, preemptive, global_))
        def __init__(self, synchronous, preemptive, global_):
                list.__init__(self, (synchronous, preemptive, global_))
        def _get_synchronous(self):
                return self[0]
        def _set_synchronous(self, val):
                self[0] = val
        synchronous = property(fget=_get_synchronous, fset=_set_synchronous)
        def _get_preemptive(self):
                return self[1]
        def _set_preemptive(self, val):
                self[1] = val
        preemptive = property(fget=_get_preemptive, fset=_set_preemptive)
        def _get_global_(self):
                return self[2]
        def _set_global_(self, val):
                self[2] = val
        global_ = property(fget=_get_global_, fset=_set_global_)

#------------------------------------------------------------------------------

class KeyDefinition(list):
        def __new__(cls, keycode, keysym, keystring, unused):
                return list.__new__(cls, (keycode, keysym, keystring, unused))
        def __init__(self, keycode, keysym, keystring, unused):
                list.__init__(self, (keycode, keysym, keystring, unused))
        def _get_keycode(self):
                return self[0]
        def _set_keycode(self, val):
                self[0] = val
        keycode = property(fget=_get_keycode, fset=_set_keycode)
        def _get_keysym(self):
                return self[1]
        def _set_keysym(self, val):
                self[1] = val
        keysym = property(fget=_get_keysym, fset=_set_keysym)
        def _get_keystring(self):
                return self[2]
        def _set_keystring(self, val):
                self[2] = val
        keystring = property(fget=_get_keystring, fset=_set_keystring)
        def _get_unused(self):
                return self[3]
        def _set_unused(self, val):
                self[3] = val
        unused = property(fget=_get_unused, fset=_set_unused)

#------------------------------------------------------------------------------

class DeviceEventController(object):
        """
        The interface via which clients request notification of device
        events, and through which device events may be simulated.
        """

        def __init__ (self, connection):
                dec_object = connection.get_object(interfaces.ATSPI_REGISTRY_NAME,
                                                   interfaces.ATSPI_DEVICE_EVENT_CONTROLLER_PATH,
                                                   introspect=True)
                self._dec = _dbus.Interface(dec_object, interfaces.ATSPI_DEVICE_EVENT_CONTROLLER_INTERFACE)

        def registerKeystrokeListener(self,
                                      event_listener,
                                      keys,
                                      event_mask,
                                      key_event_types,
                                      event_listener_mode):
                """
                Register to intercept keyboard events, and either pass them on
                or consume them.
                @param : listener
                A DeviceEventListener which will intercept key events. 
                @param : keys
                A list of KeyDefinition indicating which keys to intercept, or KEYSET_ALL_KEYS.
                @param : mask
                A ControllerEventMask bitmask for filtering the intercepted key events.
                @param : type
                A list of KeyEventType
                @param : mode
                An EventListenerMode indicating whether the listener should receive
                the events synchronously, potentially consuming them, or just
                be notified asynchronously of those events that have been generated.

                @return True if the DeviceEventListener was successfully registered
                for the requested KeySet, ControllerEventMask, event types, and
                EventListenerMode; otherwise returns False.
                """
                func = self._dec.get_dbus_method("registerKeystrokeListener")
                return func(event_listener,
                            keys,
                            event_mask,
                            key_event_types,
                            event_listener_mode)

        def deregisterKeystrokeListener(self,
                                        event_listener,
                                        keys,
                                        event_mask,
                                        key_event_types):
                """
                De-register a previously registered keyboard eventlistener.
                @param : listener
                A DeviceEventListener which will intercept key events.
                @param : keys
                A list of KeyDefinition indicating which keys to intercept, or KEYSET_ALL_KEYS.
                @param : mask
                A ControllerEventMask filtering the intercepted key events.
                @param : type
                A list of KeyEventType
                """
                func = self._dec.get_dbus_method("deregisterKeystrokeListener")
                return func(event_listener,
                            keys,
                            event_mask,
                            key_event_types)

        def registerDeviceEventListener(self,
                                        event_listener,
                                        event_types):
                """
                Register to intercept events, and either pass them on or consume
                them. To listen to keyboard events use registerKeystrokeListener
                instead. 
                @param : listener
                A DeviceEventListener which will intercept events.
                @param : typeseq
                A list of EventType indicating which event types to listen for.
                @return True if successful, False if not
                """
                func = self._dec.get_dbus_method("registerDeviceEventListener")
                return func(event_listener, event_types)

        def deregisterDeviceEventListener(self,
                                          event_listener,
                                          event_types):
                """
                De-register a previously registered keyboard eventlistener.
                @param : listener
                A DeviceEventListener which will intercept events.
                @param : typeseq
                A List of EventType indicating which event types to stop listening
                for.
                """
                func = self._dec.get_dbus_method("deregisterDeviceEventListener")
                return func(event_listener, event_types)

        def notifyListenersSync(self, event):
                """
                Notify the Registry instance that a device event has taken place,
                and allow pre-emptive listeners the opportunity to 'consume'
                the event and thus prevent its further issuance/forwarding. This
                is the method used by accessibility bridges to forward "toolkit
                dependent" device events to the Registry from the application's
                process space.
                @return True if the event was consumed by a (pre-emptive) listener,
                False if not (in which case the device event will be forwarded
                as normal to any application which would normally receive it,
                e.g. the currently active application in the case of mouse or
                keyboard events).
                """
                func = self._dec.get_dbus_method("notifyListenersSync")
                return func(event)

        def notifyListenersAsync(self, event):
                """
                Notify the Registry instance that a device event has taken place
                in an asynchronous manner. This is the method used by accessibility
                bridges to forward "toolkit dependent" device events to the Registry
                from the application's process space. If the event in question
                is potentially pre-emptible. notifyListenersSync should be used
                instead.
                """
                func = self._dec.get_dbus_method("notifyListenersAsync")
                return func(event)

        def generateKeyboardEvent(self, keycode, keystring, type):
                """
                Synthesize a keyboard event.
                @param : keycode
                A long integer indicating the keycode of the keypress to be synthesized.
                @param : keystring
                an optional UTF-8 string indicating a complex keyboard input
                event.
                @param : type
                A KeySynthType indicating the type of event(s) to be synthesized:
                a key press, release, press-release pair, or a complex input
                string (for instance from an internationalized or complex text
                input method, or a composed character).
                """
                func = self._dec.get_dbus_method("generateKeyboardEvent")
                return func(keycode, keystring, type)

        def generateMouseEvent(self, x, y, name):
                """
                Synthesize a mouse event.
                @param : x
                A long integer indicating the screen x coord for the mouse event.
                @param : y
                A long integer indicating the screen y coord for the mouse event.
                @param : name
                A string indicating the type of mouse event, e.g. "button1up"
                """
                func = self._dec.get_dbus_method("generateMouseEvent")
                return func(x, y, name)

#------------------------------------------------------------------------------

class _TestDeviceEventController(object):
        """
        Used for testing when no Registry daemon is present.
        """

        def registerKeystrokeListener(self, event_listener, keys, event_mask, key_event_types, event_listener_mode):
                return True

        def deregisterKeystrokeListener(self, event_listener, keys, event_mask, key_event_types):
                pass

        def registerDeviceEventListener(self, event_listener, event_types):
                return True

        def deregisterDeviceEventListener(self, event_listener, event_types):
                pass

        def notifyListenersSync(self, event):
                return False

        def notifyListenersAsync(self, event):
                pass

        def generateKeyboardEvent(self, keycode, keystring, type):
                pass

        def generateMouseEvent(self, x, y, name):
                pass

#------------------------------------------------------------------------------

class KeyboardDeviceEventListener(_service.Object):
        """
        Observes keyboard press and release events.

        @ivar registry: The L{Registry} that created this observer
        @type registry: L{Registry}
        @ivar key_set: Set of keys to monitor
        @type key_set: list of integer
        @ivar mask: Watch for key events while these modifiers are held
        @type mask: integer
        @ivar kind: Kind of events to monitor
        @type kind: integer
        @ivar mode: Keyboard event mode
        @type mode: Accessibility.EventListenerMode
        """

        _next_listener_id = 0

        def _get_unique_path (self):
                KeyboardDeviceEventListener._next_listener_id += 1
                return "/org/freedesktop/atspi/keyeventlistener/%d" % (KeyboardDeviceEventListener._next_listener_id,)

        def __init__(self, registry, synchronous, preemptive, global_):
                """
                Creates a mode object that defines when key events will be received from 
                the system. Stores all other information for later registration.

                @param registry: The L{Registry} that created this observer
                @type registry: L{Registry}
                @param synchronous: Handle the key event synchronously?
                @type synchronous: boolean
                @param preemptive: Allow event to be consumed?
                @type preemptive: boolean
                @param global_: Watch for events on inaccessible applications too?
                @type global_: boolean
                """
                self._upath = self._get_unique_path()
                _service.Object.__init__(self, registry._bus, self._upath)
                self.mode = EventListenerMode(synchronous, preemptive, global_)

        def register(self, dc, key_set, mask, kind):
                """
                Starts keyboard event monitoring.

                @param dc: Reference to a device controller
                @type dc: Accessibility.DeviceEventController
                @param key_set: Set of keys to monitor
                @type key_set: list of integer
                @param mask: Integer modifier mask or an iterable over multiple masks to
                        unapply all at once
                @type mask: integer, iterable, or None
                @param kind: Kind of events to monitor
                @type kind: integer
                """
                try:
                        # check if the mask is iterable
                        iter(mask)
                except TypeError:
                        # register a single integer if not
                        dc.registerKeystrokeListener(self._upath, key_set, mask, kind, self.mode)
                else:
                        for m in mask:
                                dc.registerKeystrokeListener(self._upath, key_set, m, kind, self.mode)

        def unregister(self, dc, key_set, mask, kind):
                """
                Stops keyboard event monitoring.

                @param dc: Reference to a device controller
                @type dc: Accessibility.DeviceEventController
                @param key_set: Set of keys to monitor
                @type key_set: list of integer
                @param mask: Integer modifier mask or an iterable over multiple masks to
                        unapply all at once
                @type mask: integer, iterable, or None
                @param kind: Kind of events to monitor
                @type kind: integer
                """
                try:
                        # check if the mask is iterable
                        iter(mask)
                except TypeError:
                        # unregister a single integer if not
                        dc.deregisterKeystrokeListener(self._upath, key_set, mask, kind)
                else:
                        for m in mask:
                                dc.deregisterKeystrokeListener(self._upath, key_set, m, kind)

        @_service.method(dbus_interface=interfaces.ATSPI_DEVICE_EVENT_LISTENER_INTERFACE,
                         in_signature="(uinnisb)",
                         out_signature="b")
        def notifyEvent(self, ev):
                """
                Notifies the L{Registry} that an event has occurred. Wraps the raw event 
                object in our L{Event} class to support automatic ref and unref calls. An
                observer can return True to indicate this event should not be allowed to pass 
                to other AT-SPI observers or the underlying application.

                @param ev: Keyboard event
                @type ev: Accessibility.DeviceEvent
                @return: Should the event be consumed (True) or allowed to pass on to other
                        AT-SPI observers (False)?
                @rtype: boolean
                """
                # wrap the device event
                ev = event.DeviceEvent(ev)
                return self.registry.handleDeviceEvent(ev, self)

#END---------------------------------------------------------------------------
