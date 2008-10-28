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

import interfaces
from accessible import BoundingBox

__all__ = [
                "Event",
                "EventType",
                "event_type_to_signal_reciever",
          ]

#------------------------------------------------------------------------------

_interface_to_klass = {
                "org.freedesktop.atspi.Event.Object":"object",
                "org.freedesktop.atspi.Event.Window":"window",
                "org.freedesktop.atspi.Event.Mouse":"mouse",
                "org.freedesktop.atspi.Event.Terminal":"terminal",
                "org.freedesktop.atspi.Event.Document":"document",
                "org.freedesktop.atspi.Event.Focus":"focus",
                }

_klass_to_interface = {
                "object":"org.freedesktop.atspi.Event.Object",
                "window":"org.freedesktop.atspi.Event.Window",
                "mouse":"org.freedesktop.atspi.Event.Mouse",
                "terminal":"org.freedesktop.atspi.Event.Terminal",
                "document":"org.freedesktop.atspi.Event.Document",
                "focus":"org.freedesktop.atspi.Event.Focus",
                }

#------------------------------------------------------------------------------

class _ELessList(list):
        def __getitem__(self, index):
                try:
                        return list.__getitem__(self, index)
                except IndexError:
                        return None

class EventType(str):
        """
        Wraps the AT-SPI event type string so its components can be accessed 
        individually as klass (can't use the keyword class), major, minor, and detail 
        (klass_major_minor_detail).

        @note: All attributes of an instance of this class should be considered 
                public readable as it is acting a a struct.
        @ivar klass: Most general event type identifier (object, window, mouse, etc.)
        @type klass: string
        @ivar major: Second level event type description
        @type major: string
        @ivar minor: Third level event type description
        @type minor: string
        @ivar detail: Lowest level event type description
        @type detail: string
        @ivar name: Full, unparsed event name as received from AT-SPI
        @type name: string
        @cvar format: Names of the event string components
        @type format: 4-tuple of string
        """

        _SEPARATOR = ':'

        def __init__(self, name):
                """
                Parses the full AT-SPI event name into its components
                (klass:major:minor:detail). If the provided event name is an integer
                instead of a string, then the event is really a device event.

                @param name: Full AT-SPI event name
                @type name: string
                @raise AttributeError: When the given event name is not a valid string 
                """
                stripped = name.strip(self._SEPARATOR)
                separated = stripped.split(self._SEPARATOR, 3) 
                self._separated = _ELessList(separated)

                self.klass = self._separated[0]
                self.major = self._separated[1]
                self.minor = self._separated[2]
                self.detail = self._separated[3]

                self._name = ":".join(separated)

        def is_subtype(self, event_type):
                """
                Determines if the passed event type is a subtype
                of this event.
                """
                if event_type.klass and event_type.klass !=  self.klass:
                        return False
                else:
                        if event_type.major and event_type.major != self.major:
                                return False
                        else:
                                if event_type.minor and event_type.minor != self.minor:
                                        return False
                return True

        @property
        def name(self):
                return self._name

        @property
        def value(self):
                return self._name

#------------------------------------------------------------------------------

def event_type_to_signal_reciever(bus, cache, event_handler, event_type):
        kwargs = {
                        'sender_keyword':'sender',
                        'interface_keyword':'interface',
                        'member_keyword':'member',
                        'path_keyword':'path',
                 }
        if event_type.major:
                major = event_type.major.replace('-', '_')
        if event_type.klass:
                kwargs['dbus_interface'] = _klass_to_interface[event_type.klass]
        if event_type.major:
                kwargs['signal_name'] = major
        if event_type.minor:
                kwargs['arg0'] = event_type.minor

        def handler_wrapper(minor, detail1, detail2, any_data, 
                            sender=None, interface=None, member=None, path=None):
                event = Event(cache, path, sender, interface, member, (minor, detail1, detail2, any_data))
                return event_handler(event)

        return bus.add_signal_receiver(handler_wrapper, **kwargs) 

#------------------------------------------------------------------------------

class Event(object):
        """
        Wraps an AT-SPI event with a more Pythonic interface managing exceptions,
        the differences in any_data across versions, and the reference counting of
        accessibles provided with the event.

        @note: All unmarked attributes of this class should be considered public
                readable and writable as the class is acting as a record object.

        @ivar type: The type of the AT-SPI event
        @type type: L{EventType}
        @ivar detail1: First AT-SPI event parameter
        @type detail1: integer
        @ivar detail2: Second AT-SPI event parameter
        @type detail2: integer
        @ivar any_data: Extra AT-SPI data payload
        @type any_data: object
        @ivar host_application: Application owning the event source
        @type host_application: Accessibility.Application
        @ivar source_name: Name of the event source at the time of event dispatch
        @type source_name: string
        @ivar source_role: Role of the event source at the time of event dispatch
        @type source_role: Accessibility.Role
        @ivar source: Source of the event
        @type source: Accessibility.Accessible
        """
        def __init__(self, cache, source_path, source_application, interface, name, event):
                """
                Extracts information from the provided event. If the event is a "normal" 
                event, pulls the detail1, detail2, any_data, and source values out of the
                given object and stores it in this object. If the event is a device event,
                key ID is stored in detail1, scan code is stored in detail2, key name, 
                key modifiers (e.g. ALT, CTRL, etc.), is text flag, and timestamp are 
                stored as a 4-tuple in any_data, and source is None (since key events are
                global).

                @param event: Event from an AT-SPI callback
                @type event: Accessibility.Event or Accessibility.DeviceEvent
                """
                self._cache = cache
                self._source_path = source_path
                self._source_application = source_application

                self._source = None
                self._application = None

                self._klass = _interface_to_klass[interface]
                # The replace is neccessary as '-' not legal as signal name
                # so translated on the server side.
                self._major = name.replace('_', '-')
                self._minor = event[0]
                self.type = EventType(':'.join([self._klass, self._major, self._minor]))
                self.detail1 = event[1]
                self.detail2 = event[2]

                data = event[3]
                if name == "object_bounds_changed":
                        self.any_data = BoundingBox(*data)
                else:
                        self.any_data = data

        @property
        def host_application(self):
                if not self._application:
                        return self._cache.create_application(self._source_application)
                return self._application

        @property
        def source(self):
                if not self._source:
                        self._source = self._cache.create_accessible(self._source_application,
                                                                     self._source_path,
                                                                     interfaces.ATSPI_ACCESSIBLE)
                return self._source

        @property
        def source_name(self):
                return source.name

        @property
        def source_role(self):
                return source.getRole()

        def __str__(self):
                """
                Builds a human readable representation of the event including event type,
                parameters, and source info.

                @return: Event description
                @rtype: string
                """
                return '%s(%s, %s, %s)\n\tsource: %s\n\thost_application: %s' % \
                                         (self.type, self.detail1, self.detail2, self.any_data,
                                                self.source, self.host_application)

#END----------------------------------------------------------------------------
