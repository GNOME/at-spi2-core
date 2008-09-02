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

import dbus
import gobject

from dbus.mainloop.glib import DBusGMainLoop
DBusGMainLoop(set_as_default=True)

from test import TestApplicationCache
from desktop import Desktop
from event import EventType, event_type_to_signal_reciever

class Registry(object):
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

	_REGISTRY_NAME = 'org.freedesktop.atspi.Registry'

	def __init__(self, app_name=None):
		"""
		Stores a reference to the AT-SPI registry. Gets and stores a reference
		to the DeviceEventController.
		
		@param reg: Reference to the AT-SPI registry daemon
		@type reg: Accessibility.Registry
		"""
		self._bus = dbus.SessionBus()
		if app_name:
			self._app_name = app_name
			self._cache = TestApplicationCache(self._bus, app_name)

		self._event_listeners = {}
		
	def __call__(self):
		"""
		@return: This instance of the registry
		@rtype: L{Registry}
		"""
		return self
	
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
		self._loop = gobject.MainLoop()
		self._loop.run()
		try:
			loop.run()
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
		return Desktop(self._cache)

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
			new_type = EventType(name)
			registered.append((new_type.name,
					   event_type_to_signal_reciever(self._bus, self._cache, client, new_type)))

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
		
		for name in names:
			remove_type = EventType(name)

			for i in range(0, len(registered)):
				(type_name, signal_match) = registered[i]
				registered_type = EventType(type_name)
				if remove_type.is_subtype(registered_type):
					signal_match.remove()
					del(registered[i])

		if registered == []:
			del(self._event_listeners[client])
