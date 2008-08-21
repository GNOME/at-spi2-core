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

from dbus.mainloop.glib import DBusGMainLoop
DBusGMainLoop(set_as_default=True)

from test import TestApplicationCache
from desktop import Desktop

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
