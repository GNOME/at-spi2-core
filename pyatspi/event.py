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
from factory import create_accessible, add_accessible_class
from accessible import BoundingBox

__all__ = [
		"Event",
		"EventType",
	  ]

#------------------------------------------------------------------------------

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
	format = ('klass', 'major', 'minor', 'detail')

	def __init__(self, name):
		"""		
		Parses the full AT-SPI event name into its components
		(klass:major:minor:detail). If the provided event name is an integer
		instead of a string, then the event is really a device event.
		
		@param name: Full AT-SPI event name
		@type name: string
		@raise AttributeError: When the given event name is not a valid string 
		"""
		# get rid of any leading and trailing '_' separators
		self.value = name.strip('_')
		self.name = self.value # Backward compatability
		self.klass = None
		self.major = None
		self.minor = None
		self.detail = None
		
		# split type according to delimiters
		split = self.value.split('_', 3)
		# loop over all the components
		for i in xrange(len(split)):
			# store values of attributes in this object
			setattr(self, self.format[i], split[i])

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
	def __init__(self, cache, source_path, source_application, name, event):
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

		self._detail = event[0]
		self.type = EventType(name + '_' + self._detail)
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
			application_root = self._cache[self._source_application]._get_root()
			return create_accessible(self._cache,
					 	 self._source_application,
						 application_root,
						 interfaces.ATSPI_APPLICATION,
				 	 	 connection=self._cache._connection)
		return self._application

	@property
	def source(self):
		if not self._source:
			self._source = create_accessible(self._cache,
		 	 		 		 self._source_application,
			 		 		 self._source_path,
			 		 		 interfaces.ATSPI_ACCESSIBLE,
			 		 		 connection=self._cache._connection)
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
