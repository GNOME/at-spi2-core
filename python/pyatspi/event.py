'''
Wrapper classes for AT-SPI events and device events.

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2005, 2007 IBM Corporation
@license: LGPL

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Portions of this code originally licensed and copyright (c) 2005, 2007
IBM Corporation under the BSD license, available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''
import constants

class DeviceEvent(object):
  '''
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
  '''
  def __init__(self, event):
    '''
    Attaches event data to this object.
    
    @param event: Event object
    @type event: Accessibility.DeviceEvent
    '''
    self.consume = False
    self.type = event.type
    self.id = event.id
    self.hw_code = event.hw_code
    self.modifiers = event.modifiers
    self.timestamp = event.timestamp
    self.event_string = event.event_string
    self.is_text = event.is_text
    
  def __str__(self):
    '''
    Builds a human readable representation of the event.

    @return: Event description
    @rtype: string
    '''
    if self.type == constants.KEY_PRESSED_EVENT:
      kind = 'pressed'
    elif self.type == constants.KEY_RELEASED_EVENT:
      kind = 'released'
    return '''\
%s
\thw_code: %d
\tevent_string: %s
\tmodifiers: %d
\tid: %d
\ttimestamp: %d
\tis_text: %s''' % (kind, self.hw_code, self.event_string, self.modifiers,
                    self.id, self.timestamp, self.is_text)

class Event(object):
  '''
  Wraps an AT-SPI event with a more Pythonic interface managing exceptions,
  the differences in any_data across versions, and the reference counting of
  accessibles provided with the event.
  
  @note: All unmarked attributes of this class should be considered public
    readable and writable as the class is acting as a record object.
    
  @ivar consume: Should this event be consumed and not allowed to pass on to
    observers further down the dispatch chain in this process?
  @type consume: boolean
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
  '''
  def __init__(self, event):
    '''
    Extracts information from the provided event. If the event is a "normal" 
    event, pulls the detail1, detail2, any_data, and source values out of the
    given object and stores it in this object. If the event is a device event,
    key ID is stored in detail1, scan code is stored in detail2, key name, 
    key modifiers (e.g. ALT, CTRL, etc.), is text flag, and timestamp are 
    stored as a 4-tuple in any_data, and source is None (since key events are
    global).

    @param event: Event from an AT-SPI callback
    @type event: Accessibility.Event or Accessibility.DeviceEvent
    '''
    # always start out assuming no consume
    self.consume = False
    self.type = EventType(event.type)
    self.detail1 = event.detail1
    self.detail2 = event.detail2
    # store the event source and increase the reference count since event 
    # sources are borrowed references; the AccessibleMixin automatically
    # decrements it later
    try:
      event.source.ref()
    except AttributeError:
      pass
    self.source = event.source

    # process any_data in a at-spi version independent manner
    details = event.any_data.value()
    try:
      # see if we have a "new" any_data object which is an EventDetails struct
      self.any_data = details.any_data.value()
    except Exception:
      # any kind of error means we have an "old" any_data object and None of
      # the extra data so set them to None
      self.any_data = details
      self.host_application = None
      self.source_name = None
      self.source_role = None
    else:
      # the rest of the data should be here, so retrieve it
      self.host_application = details.host_application
      self.source_name = details.source_name
      self.source_role = details.source_role
    try:
      # if we received an accessible, be sure to increment the ref count
      self.any_data.ref()
    except AttributeError:
      pass
    try:
      # if we received a host application, be sure to increment the ref count
      self.host_application.ref()
    except AttributeError:
      pass

  def __str__(self):
    '''
    Builds a human readable representation of the event including event type,
    parameters, and source info.

    @return: Event description
    @rtype: string
    '''
    return '%s(%s, %s, %s)\n\tsource: %s\n\thost_application: %s' % \
           (self.type, self.detail1, self.detail2, self.any_data,
            self.source, self.host_application)
  
class EventType(str):
  '''
  Wraps the AT-SPI event type string so its components can be accessed 
  individually as klass (can't use the keyword class), major, minor, and detail 
  (klass:major:minor:detail).
  
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
  '''
  format = ('klass', 'major', 'minor', 'detail')

  def __init__(self, name):
    '''    
    Parses the full AT-SPI event name into its components
    (klass:major:minor:detail). If the provided event name is an integer
    instead of a string, then the event is really a device event.
    
    @param name: Full AT-SPI event name
    @type name: string
    @raise AttributeError: When the given event name is not a valid string 
    '''
    # get rid of any leading and trailing ':' separators
    self.value = name.strip(':')
    self.name = self.value # Backward compatability
    self.klass = None
    self.major = None
    self.minor = None
    self.detail = None
    
    # split type according to delimiters
    split = self.value.split(':', 3)
    # loop over all the components
    for i in xrange(len(split)):
      # store values of attributes in this object
      setattr(self, self.format[i], split[i])
      
