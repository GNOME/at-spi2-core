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

#import signal
#import time
#import weakref
#import Queue
#import traceback
#import gobject
#import utils
#import constants
#import event

ATSPI_DEVICE_EVENT_CONTROLLER = 'org.freedesktop.atspi.DeviceEventController'
ATSPI_DEVICE_EVENT_LISTENER = 'org.freedesktop.atspi.DeviceEventListener'

import constants

class _Observer(object):
	"""
	Parent class for all event observers. Dispatches all received events to the 
	L{Registry} that created this L{_Observer}. Provides basic reference counting
	functionality needed by L{Registry} to determine when an L{_Observer} can be
	released for garbage collection. 
	
	The reference counting provided by this class is independent of the reference
	counting used by CORBA. Keeping the counts separate makes it easier for the
	L{Registry} to detect when an L{_Observer} can be freed in the 
	L{Registry._unregisterObserver} method.
	
	@ivar registry: Reference to the L{Registry} that created this L{_Observer}
	@type registry: weakref.proxy to L{Registry}
	@ivar ref_count: Reference count on this L{_Observer}
	@type ref_count: integer
	"""
	def __init__(self, registry):
		"""
		Stores a reference to the creating L{Registry}. Intializes the reference
		count on this object to zero.
		
		@param registry: The L{Registry} that created this observer
		@type registry: weakref.proxy to L{Registry}
		"""
		self.registry = weakref.proxy(registry)
		self.ref_count = 0

	def clientRef(self):
		"""
		Increments the Python reference count on this L{_Observer} by one. This
		method is called when a new client is registered in L{Registry} to receive
		notification of an event type monitored by this L{_Observer}.
		"""
		self.ref_count += 1
		
	def clientUnref(self):
		"""		
		Decrements the pyatspi reference count on this L{_Observer} by one. This
		method is called when a client is unregistered in L{Registry} to stop
		receiving notifications of an event type monitored by this L{_Observer}.
		"""
		self.ref_count -= 1
		
	def getClientRefCount(self):
		"""
		@return: Current Python reference count on this L{_Observer}
		@rtype: integer
		"""
		return self.ref_count
	
	def ref(self): 
		"""Required by CORBA. Does nothing."""
		pass
		
	def unref(self): 
		"""Required by CORBA. Does nothing."""
		pass

class _DeviceObserver(_Observer, Accessibility__POA.DeviceEventListener):
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
		_Observer.__init__(self, registry)	 
		self.mode = Accessibility.EventListenerMode()
		self.mode.preemptive = preemptive
		self.mode.synchronous = synchronous
		self.mode._global = global_		
	 
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
			dc.registerKeystrokeListener(self._this(), key_set, mask, kind, 
																	 self.mode)
		else:
			for m in mask:
				dc.registerKeystrokeListener(self._this(), key_set, m, kind, self.mode)

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
			dc.deregisterKeystrokeListener(self._this(), key_set, mask, kind)
		else:
			for m in mask:
				dc.deregisterKeystrokeListener(self._this(), key_set, m, kind)
			
	def queryInterface(self, repo_id):
		"""
		Reports that this class only implements the AT-SPI DeviceEventListener 
		interface. Required by AT-SPI.
		
		@param repo_id: Request for an interface 
		@type repo_id: string
		@return: The underlying CORBA object for the device event listener
		@rtype: Accessibility.EventListener
		"""
		if repo_id == utils.getInterfaceIID(Accessibility.DeviceEventListener):
			return self._this()
		else:
			return None

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

class _EventObserver(_Observer, Accessibility__POA.EventListener):
	"""
	Observes all non-keyboard AT-SPI events. Can be reused across event types.
	"""
	def register(self, reg, name):
		"""
		Starts monitoring for the given event.
		
		@param name: Name of the event to start monitoring
		@type name: string
		@param reg: Reference to the raw registry object
		@type reg: Accessibility.Registry
		"""
		reg.registerGlobalEventListener(self._this(), name)
		
	def unregister(self, reg, name):
		"""
		Stops monitoring for the given event.
		
		@param name: Name of the event to stop monitoring
		@type name: string
		@param reg: Reference to the raw registry object
		@type reg: Accessibility.Registry
		"""
		reg.deregisterGlobalEventListener(self._this(), name)

	def queryInterface(self, repo_id):
		"""
		Reports that this class only implements the AT-SPI DeviceEventListener 
		interface. Required by AT-SPI.

		@param repo_id: Request for an interface 
		@type repo_id: string
		@return: The underlying CORBA object for the device event listener
		@rtype: Accessibility.EventListener
		"""
		if repo_id == utils.getInterfaceIID(Accessibility.EventListener):
			return self._this()
		else:
			return None

	def notifyEvent(self, ev):
		"""
		Notifies the L{Registry} that an event has occurred. Wraps the raw event 
		object in our L{Event} class to support automatic ref and unref calls.
		Aborts on any exception indicating the event could not be wrapped.
		
		@param ev: AT-SPI event signal (anything but keyboard)
		@type ev: Accessibility.Event
		"""
		# wrap raw event so ref counts are correct before queueing
		ev = event.Event(ev)
		self.registry.handleEvent(ev)

class DeviceEvent(object):
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
	def __init__(self, event):
		"""
		Attaches event data to this object.
		
		@param event: Event object
		@type event: Accessibility.DeviceEvent
		"""
		self.consume = False
		self.type = event.type
		self.id = event.id
		self.hw_code = event.hw_code
		self.modifiers = event.modifiers
		self.timestamp = event.timestamp
		self.event_string = event.event_string
		self.is_text = event.is_text
		
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

class Event(object):
	"""
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
	"""
	def __init__(self, event):
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
		"""
		Builds a human readable representation of the event including event type,
		parameters, and source info.

		@return: Event description
		@rtype: string
		"""
		return '%s(%s, %s, %s)\n\tsource: %s\n\thost_application: %s' % \
					 (self.type, self.detail1, self.detail2, self.any_data,
						self.source, self.host_application)
	
class EventType(str):
	"""
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
		if app_path:
			self._app_name = app_name
			self._cache = TestApplicationCache(self._bus, app_name)

		#self.async = None
		#self.reg = reg
		#self.dev = self.reg.getDeviceEventController()
		#self.queue = Queue.Queue()
		#self.clients = {}
		#self.observers = {}
		
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
		for name in names:
			# store the callback for each specific event name
			self._registerClients(client, name)

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
		missed = False
		for name in names:
			# remove the callback for each specific event name
			missed |= self._unregisterClients(client, name)
		return missed

	def registerKeystrokeListener(self, client, key_set=[], mask=0, 
																kind=(constants.KEY_PRESSED_EVENT, 
																			constants.KEY_RELEASED_EVENT),
																synchronous=True, preemptive=True, 
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
			ob = self.clients[client]
		except KeyError:
			# create a new device observer for this client
			ob = _DeviceObserver(self, synchronous, preemptive, global_)
			# store the observer to client mapping, and the inverse
			self.clients[ob] = client
			self.clients[client] = ob
		if mask is None:
			# None means all modifier combinations
			mask = utils.allModifiers()
		# register for new keystrokes on the observer
		ob.register(self.dev, key_set, mask, kind)

	def deregisterKeystrokeListener(self, client, key_set=[], mask=0, 
																	kind=(constants.KEY_PRESSED_EVENT, 
																				constants.KEY_RELEASED_EVENT)):
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
		ob = self.clients[client]
		if mask is None:
			# None means all modifier combinations
			mask = utils.allModifiers()
		# register for new keystrokes on the observer
		ob.unregister(self.dev, key_set, mask, kind)

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
		@type ob: L{_DeviceObserver}

		@return: Should the event be consumed (True) or allowed to pass on to other
			AT-SPI observers (False)?
		@rtype: boolean
		"""
		try:
			# try to get the client registered for this event type
			client = self.clients[ob]
		except KeyError:
			# client may have unregistered recently, ignore event
			return False
		# make the call to the client
		try:
			return client(event) or event.consume
		except Exception:
			# print the exception, but don't let it stop notification
			traceback.print_exc()
 
	def handleEvent(self, event):
		"""		
		Handles an AT-SPI event by either queuing it for later dispatch when the
		L{Registry.async} flag is set, or dispatching it immediately.

		@param event: AT-SPI event
		@type event: L{event.Event}
		"""
		if self.async:
			# queue for now
			self.queue.put_nowait(event)
		else:
			# dispatch immediately
			self._dispatchEvent(event)

	def _dispatchEvent(self, event):
		"""
		Dispatches L{event.Event}s to registered clients. Clients are called in
		the order they were registered for the given AT-SPI event. If any client
		returns True, callbacks cease for the event for clients of this registry 
		instance. Clients of other registry instances and clients in other processes 
		are unaffected.

		@param event: AT-SPI event
		@type event: L{event.Event}
		"""
		et = event.type
		try:
			# try to get the client registered for this event type
			clients = self.clients[et.name]
		except KeyError:
			try:
				# we may not have registered for the complete subtree of events
				# if our tree does not list all of a certain type (e.g.
				# object:state-changed:*); try again with klass and major only
				if et.detail is not None:
					# Strip the 'detail' field.
					clients = self.clients['%s:%s:%s' % (et.klass, et.major, et.minor)]
				elif et.minor is not None:
					# The event could possibly be object:state-changed:*.
					clients = self.clients['%s:%s' % (et.klass, et.major)]
			except KeyError:
				# client may have unregistered recently, ignore event
				return
		# make the call to each client
		consume = False
		for client in clients:
			try:
				consume = client(event) or False
			except Exception:
				# print the exception, but don't let it stop notification
				traceback.print_exc()
			if consume or event.consume:
				# don't allow further processing if a client returns True
				break

	def flushEvents(self):
		"""
		Flushes the event queue by destroying it and recreating it.
		"""
		self.queue = Queue.Queue()

	def pumpQueuedEvents(self, num=-1):
		"""
		Provides asynch processing of events in the queue by executeing them with 
		_dispatchEvent() (as is done immediately when synch processing). 
		This method would normally be called from a main loop or idle function.

		@param num: Number of events to pump. If number is negative it pumps
		the entire queue. Default is -1.
		@type num: integer
		@return: True if queue is not empty after events were pumped.
		@rtype: boolean
		"""
		if num < 0:
			# Dequeue as many events as currently in the queue.
			num = self.queue.qsize()
		for i in xrange(num):
			try:
				# get next waiting event
				event = self.queue.get_nowait()
			except Queue.Empty:
				break
			self._dispatchEvent(event)

		return not self.queue.empty()
 
	def _registerClients(self, client, name):
		"""
		Internal method that recursively associates a client with AT-SPI event 
		names. Allows a client to incompletely specify an event name in order to 
		register for subevents without specifying their full names manually.
		
		@param client: Client callback to receive event notifications
		@type client: callable
		@param name: Partial or full event name
		@type name: string
		"""
		try:
			# look for an event name in our event tree dictionary
			events = constants.EVENT_TREE[name]
		except KeyError:
			# if the event name doesn't exist, it's a leaf event meaning there are
			# no subtypes for that event
			# add this client to the list of clients already in the dictionary 
			# using the event name as the key; if there are no clients yet for this 
			# event, insert an empty list into the dictionary before appending 
			# the client
			et = event.EventType(name)
			clients = self.clients.setdefault(et.name, [])
			try:
				# if this succeeds, this client is already registered for the given
				# event type, so ignore the request
				clients.index(client)
			except ValueError:
				# else register the client
				clients.append(client)
				self._registerObserver(name)
		else:
				# if the event name does exist in the tree, there are subevents for
				# this event; loop through them calling this method again to get to
				# the leaf events
				for e in events:
					self._registerClients(client, e)
			
	def _unregisterClients(self, client, name):
		"""
		Internal method that recursively unassociates a client with AT-SPI event 
		names. Allows a client to incompletely specify an event name in order to 
		unregister for subevents without specifying their full names manually.
		
		@param client: Client callback to receive event notifications
		@type client: callable
		@param name: Partial or full event name
		@type name: string
		"""
		missed = False
		try:
			# look for an event name in our event tree dictionary
			events = constants.EVENT_TREE[name]
		except KeyError:
			try:
				# if the event name doesn't exist, it's a leaf event meaning there are
				# no subtypes for that event
				# get the list of registered clients and try to remove the one provided
				et = event.EventType(name)
				clients = self.clients[et.name]
				clients.remove(client)
				self._unregisterObserver(name)
			except (ValueError, KeyError):
				# ignore any exceptions indicating the client is not registered
				missed = True
			return missed
		# if the event name does exist in the tree, there are subevents for this 
		# event; loop through them calling this method again to get to the leaf
		# events
		for e in events:
			missed |= self._unregisterClients(client, e)
		return missed
	
	def _registerObserver(self, name):
		"""		
		Creates a new L{_Observer} to watch for events of the given type or
		returns the existing observer if one is already registered. One
		L{_Observer} is created for each leaf in the L{constants.EVENT_TREE} or
		any event name not found in the tree.
	 
		@param name: Raw name of the event to observe
		@type name: string
		@return: L{_Observer} object that is monitoring the event
		@rtype: L{_Observer}
		"""
		et = event.EventType(name)
		try:
			# see if an observer already exists for this event
			ob = self.observers[et.name]
		except KeyError:
			# build a new observer if one does not exist
			ob = _EventObserver(self)
			# we have to register for the raw name because it may be different from
			# the parsed name determined by EventType (e.g. trailing ':' might be 
			# missing)
			ob.register(self.reg, name)
			self.observers[et.name] = ob
		# increase our client ref count so we know someone new is watching for the 
		# event
		ob.clientRef()
		return ob
		
	def _unregisterObserver(self, name):
		"""		
		Destroys an existing L{_Observer} for the given event type only if no
		clients are registered for the events it is monitoring.
		
		@param name: Name of the event to observe
		@type name: string
		@raise KeyError: When an observer for the given event is not regist
		"""
		et = event.EventType(name)
		# see if an observer already exists for this event
		ob = self.observers[et.name]
		ob.clientUnref()
		if ob.getClientRefCount() == 0:
			ob.unregister(self.reg, name)
			del self.observers[et.name]

class DeviceEvent(list):
    def __new__(cls, type, id, hw_code, modifiers, timestamp, event_string, is_text):
        list.__new__(cls, (type, id, hw_code, modifiers, timestamp, event_string, is_text))
    def __init__(self, type, id, hw_code, modifiers, timestamp, event_string, is_text):
        list.__init__(self, (type, id, hw_code, modifiers, timestamp, event_string, is_text))
    
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

class DeviceEventController(_BaseProxy):
    """
    The interface via which clients request notification of device
    events, and through which device events may be simulated.
    """

    def deregisterDeviceEventListener(self, *args, **kwargs):
        """
        De-register a previously registered keyboard eventlistener. 
        @param : listener
        a DeviceEventListener which will intercept events. 
        @param : typeseq
        an EventTypeSeq indicating which event types to stop listening
        for.
        """
        func = self.get_dbus_method("deregisterDeviceEventListener")
        return func(*args, **kwargs)
    
    def deregisterKeystrokeListener(self, *args, **kwargs):
        """
        De-register a previously registered keyboard eventlistener. 
        @param : listener
        a DeviceEventListener which will intercept key events. 
        @param : keys
        a KeySet indicating which keys to intercept, or KEYSET_ALL_KEYS.
        @param : mask
        a ControllerEventMask filtering the intercepted key events. 
        @param : type
        an EventType mask that may created by ORing event types together.
        """
        func = self.get_dbus_method("deregisterKeystrokeListener")
        return func(*args, **kwargs)
    
    def generateKeyboardEvent(self, *args, **kwargs):
        """
        Synthesize a keyboard event. 
        @param : keycode
        a long integer indicating the keycode of the keypress to be synthesized.
        @param : keystring
        an optional UTF-8 string indicating a complex keyboard input
        event. 
        @param : type
        a KeySynthType indicating the type of event(s) to be synthesized:
        a key press, release, press-release pair, or a complex input
        string (for instance from an internationalized or complex text
        input method, or a composed character).
        """
        func = self.get_dbus_method("generateKeyboardEvent")
        return func(*args, **kwargs)
    
    def generateMouseEvent(self, *args, **kwargs):
        """
        Synthesize a mouse event. 
        @param : x
        a long integer indicating the screen x coord for the mouse event.
        @param : y
        a long integer indicating the screen y coord for the mouse event.
        @param : eventName
        a string indicating the type of mouse event, e.g. "button1up"
        """
        func = self.get_dbus_method("generateMouseEvent")
        return func(*args, **kwargs)
    
    def notifyListenersAsync(self, *args, **kwargs):
        """
        Notify the Registry instance that a device event has taken place
        in an asynchronous manner. This is the method used by accessibility
        bridges to forward "toolkit dependent" device events to the Registry
        from the application's process space. If the event in question
        is potentially pre-emptible. notifyListenersSync should be used
        instead.
        """
        func = self.get_dbus_method("notifyListenersAsync")
        return func(*args, **kwargs)
    
    def notifyListenersSync(self, *args, **kwargs):
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
        func = self.get_dbus_method("notifyListenersSync")
        return func(*args, **kwargs)
    
    def registerDeviceEventListener(self, *args, **kwargs):
        """
        Register to intercept events, and either pass them on or consume
        them. To listen to keyboard events use registerKeystrokeListener
        instead. 
        @param : listener
        a DeviceEventListener which will intercept events. 
        @param : typeseq
        an EventTypeSeq indicating which event types to listen for. 
        @return True if successful, False if not
        """
        func = self.get_dbus_method("registerDeviceEventListener")
        return func(*args, **kwargs)
    
    def registerKeystrokeListener(self, *args, **kwargs):
        """
        Register to intercept keyboard events, and either pass them on
        or consume them.
        @param : listener
        a DeviceEventListener which will intercept key events. 
        @param : keys
        a KeySet indicating which keys to intercept, or KEYSET_ALL_KEYS.
        @param : mask
        a ControllerEventMask filtering the intercepted key events. 
        @param : type
        a KeyEventTypeSeq that may created by ORing event types together.
        @param : mode
        an EventListenerMode indicating whether the listener should receive
        the events synchronously, potentially consuming them, or just
        be notified asynchronously of those events that have been generated.
        @return True if the DeviceEventListener was successfully registered
        for the requested KeySet, ControllerEventMask, event types, and
        EventListenerMode; otherwise returns False.
        """
        func = self.get_dbus_method("registerKeystrokeListener")
        return func(*args, **kwargs)
    
class DeviceEventListener(_BaseProxy):
    """
    This interface should be implemented by AT-SPI clients who wish
    to make use of the DeviceEventController to receive device event
    notifications. DeviceEvents include keyboard events and mouse
    button/motion events.
    """
    
    def notifyEvent(self, *args, **kwargs):
        """
        Notify an interested DeviceEventListener that a DeviceEvent has
        occurred. 
        @return True if the recipient/consumer wishes to consume the
        event, i.e. prevent it from being delivered to the desktop, False
        if the event should continue to be delivered as normal.
        """
        func = self.get_dbus_method("notifyEvent")
        return func(*args, **kwargs)
    

class Event(list):
    def __new__(cls, type, source, detail1, detail2, any_data):
        list.__new__(cls, (type, source, detail1, detail2, any_data))
    def __init__(self, type, source, detail1, detail2, any_data):
        list.__init__(self, (type, source, detail1, detail2, any_data))
    
    def _get_type(self):
        return self[0]
    def _set_type(self, val):
        self[0] = val
    type = property(fget=_get_type, fset=_set_type)
    def _get_source(self):
        return self[1]
    def _set_source(self, val):
        self[1] = val
    source = property(fget=_get_source, fset=_set_source)
    def _get_detail1(self):
        return self[2]
    def _set_detail1(self, val):
        self[2] = val
    detail1 = property(fget=_get_detail1, fset=_set_detail1)
    def _get_detail2(self):
        return self[3]
    def _set_detail2(self, val):
        self[3] = val
    detail2 = property(fget=_get_detail2, fset=_set_detail2)
    def _get_any_data(self):
        return self[4]
    def _set_any_data(self, val):
        self[4] = val
    any_data = property(fget=_get_any_data, fset=_set_any_data)


class EventDetails(list):
    def __new__(cls, host_application, source_role, source_name, any_data):
        list.__new__(cls, (host_application, source_role, source_name, any_data))
    def __init__(self, host_application, source_role, source_name, any_data):
        list.__init__(self, (host_application, source_role, source_name, any_data))
    
    def _get_host_application(self):
        return self[0]
    def _set_host_application(self, val):
        self[0] = val
    host_application = property(fget=_get_host_application, fset=_set_host_application)
    def _get_source_role(self):
        return self[1]
    def _set_source_role(self, val):
        self[1] = val
    source_role = property(fget=_get_source_role, fset=_set_source_role)
    def _get_source_name(self):
        return self[2]
    def _set_source_name(self, val):
        self[2] = val
    source_name = property(fget=_get_source_name, fset=_set_source_name)
    def _get_any_data(self):
        return self[3]
    def _set_any_data(self, val):
        self[3] = val
    any_data = property(fget=_get_any_data, fset=_set_any_data)

class EventListener(_BaseProxy):
    """
    A generic interface implemented by objects for the receipt of
    event notifications. EventListener is the interface from which
    Accessibility::Registry is derived, and via which clients of
    the Registry receive notification of changes to an application's
    user interface and content.
    """
    
    def notifyEvent(self, *args, **kwargs):
        """
        Synchronously notify an EventListener that an event has occurred,
        by passing it an Event struct. 
        @param : e
        The Event about which the listener is being notified.
        """
        func = self.get_dbus_method("notifyEvent")
        return func(*args, **kwargs)
    
    def unImplemented2_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2_")
        return func(*args, **kwargs)
    
    def unImplemented3_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3_")
        return func(*args, **kwargs)
    
    def unImplemented4_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4_")
        return func(*args, **kwargs)
    
    def unImplemented_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_")
        return func(*args, **kwargs)


class EventListenerMode(list):
    def __new__(cls, synchronous, preemptive, global_):
        list.__new__(cls, (synchronous, preemptive, global_))
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


class EventType(_Enum):
    _enum_lookup = {
        0:'KEY_PRESSED_EVENT',
        1:'KEY_RELEASED_EVENT',
        2:'BUTTON_PRESSED_EVENT',
        3:'BUTTON_RELEASED_EVENT',
    }



    


class KeyDefinition(list):
    def __new__(cls, keycode, keysym, keystring, unused):
        list.__new__(cls, (keycode, keysym, keystring, unused))
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

class KeyEventType(_Enum):
    _enum_lookup = {
        0:'KEY_PRESSED',
        1:'KEY_RELEASED',
    }

class KeySynthType(_Enum):
    _enum_lookup = {
        0:'KEY_PRESS',
        1:'KEY_RELEASE',
        2:'KEY_PRESSRELEASE',
        3:'KEY_SYM',
        4:'KEY_STRING',
    }



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

class Registry(EventListener):
    """
    The Registry is a service through which applications providing
    accessibility services (servers) can rendezvous with consumers
    of those services (Assistive Technologies). The Registry is the
    first "port of call" for accessible applications and for assistive
    technologies wishing to query and interact with those applications.
    The Registry service provides four basic functions to Assistive
    Technology (AT) clients: 
    it provides a list of the applications who have registered with
    the AT-SPI framework, thereby announcing their participation
    in the AT-SPI framework; 
    it allows AT clients to register for notification of changes
    in application state (at-spi Events); 
    it dispatches/relays said events from participating applications
    to the registered listeners; 
    it gives access to system device events via the associated DeviceEventController
    interface.
    From the point of view of accessible applications (i.e. AT-SPI
    service producers), the Registry is primarily a registration
    and event delivery service. Applications normally only call the
    registerApplication and deregisterApplication Registry methods,
    and its inherited EventListener::notifyEvent method.
    The Registry normally lives in its own process space; communication
    via Registry and both application services and AT clients takes
    place via IPC. A process space diagram illustrating the relationship
    between applications, Registry, and AT is shown below.
    """
    
    def deregisterApplication(self, *args, **kwargs):
        """
        De-register an application previously registered with the broker.
        deregisterApplication: 
        @param : app
        a reference to the Application to be deregistered.
        """
        func = self.get_dbus_method("deregisterApplication")
        return func(*args, **kwargs)
    
    def deregisterGlobalEventListener(self, *args, **kwargs):
        """
        deregisterGlobalEventListener: 
        @param : listener
        the requesting EventListener 
        @param : eventName
        a string indicating the type of events
        Request that a previously registered client stop receiving global
        notifications for events of a certain type.
        """
        func = self.get_dbus_method("deregisterGlobalEventListener")
        return func(*args, **kwargs)
    
    def deregisterGlobalEventListenerAll(self, *args, **kwargs):
        """
        deregisterGlobalEventListenerAll: 
        @param : listener
        the requesting EventListener
        Request that a previously registered client stop receiving global
        notifications for all events for which it was registered.
        """
        func = self.get_dbus_method("deregisterGlobalEventListenerAll")
        return func(*args, **kwargs)
    
    def getDesktop(self, *args, **kwargs):
        """
        getDesktop: 
        : the index of the requested Desktop.
        Get the nth accessible desktop.
        @return a reference to the requested Desktop.
        """
        func = self.get_dbus_method("getDesktop")
        return func(*args, **kwargs)
    
    def getDesktopCount(self, *args, **kwargs):
        """
        event types: "Window" "Desktop" "Window:Create" "Window:Destroy"
        "Window:Iconify" "Window:Restore" "Window:Fullscreen" "Window:Resize"
        "Desktop:Create" "Desktop:Destroy" "Desktop:Focus" "Desktop:Defocus"
        "Desktop:Reorder" "Focus" "GtkWidget:show" "GObject:notify:<propertyname>"
        ( not sure we should allow these last 2 forms, since they are
        toolkit-specific, but they're powerful ) getDesktopCount:
        Get the current number of desktops. 
        @return a short integer indicating the current number of Desktops.
        """
        func = self.get_dbus_method("getDesktopCount")
        return func(*args, **kwargs)
    
    def getDesktopList(self, *args, **kwargs):
        """
        Get a list of accessible desktops.
        @return : a sequence containing references to the Desktops.
        """
        func = self.get_dbus_method("getDesktopList")
        return func(*args, **kwargs)
    
    def getDeviceEventController(self, *args, **kwargs):
        """
        Obtain an object which can be used to request device event notifications.
        @return : an object implementing DeviceEventController
        """
        func = self.get_dbus_method("getDeviceEventController")
        return func(*args, **kwargs)
    
    def registerApplication(self, *args, **kwargs):
        """
        Register a new application with the accessibility broker. 
        @param : app
        a reference to the requesting Application
        """
        func = self.get_dbus_method("registerApplication")
        return func(*args, **kwargs)
    
    def registerGlobalEventListener(self, *args, **kwargs):
        """
        Register a client's interest in (all) application events of a
        certain type. 
        @param : listener
        a reference to the requesting EventListener. 
        @param : eventName
        a string which indicates the type of events about which the client
        desires notification.
        """
        func = self.get_dbus_method("registerGlobalEventListener")
        return func(*args, **kwargs)

    







class Table(_BaseProxy):
    """
    An interface used by containers whose contained data is arranged
    in a "tabular" (i.e. row-column) fashion. Tables may resemble
    a two-dimensional grid, as in a spreadsheet, or may feature objects
    which span multiple rows and/or columns, but whose bounds are
    aligned on a row/column matrix. Thus, the Table interface may
    be used to represent "spreadsheets" as well as "frames".
    Objects within tables are children of the Table instance, and
    they may be referenced either via a child index or via a row/column
    pair. Their role may be ROLE_TABLE_CELL, but table 'cells' may
    have other roles as well. These 'cells' may implement other interfaces,
    such as Text, Action, Image, and Component, and should do so
    as appropriate to their onscreen representation and/or behavior.
    """
    
    def addColumnSelection(self, *args, **kwargs):
        """
        Select the specified column, adding it to the current column
        selection, if the table's selection model permits it.
        @param : column
        @return True if the specified column was successfully selected,
        False if not.
        """
        func = self.get_dbus_method("addColumnSelection")
        return func(*args, **kwargs)
    
    def addRowSelection(self, *args, **kwargs):
        """
        Select the specified row, adding it to the current row selection,
        if the table's selection model permits it.
        @param : row
        @return True if the specified row was successfully selected,
        False if not.
        """
        func = self.get_dbus_method("addRowSelection")
        return func(*args, **kwargs)
    
    def getAccessibleAt(self, *args, **kwargs):
        """
        Get the table cell at the specified row and column indices. 
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return an Accessible object representing the specified table
        cell.
        """
        func = self.get_dbus_method("getAccessibleAt")
        return func(*args, **kwargs)
    
    def getColumnAtIndex(self, *args, **kwargs):
        """
        Get the table column index occupied by the child at a particular
        1-D child index.
        @param : index
        the specified child index, zero-indexed.
        @return a long integer indicating the first column spanned by
        the child of a table, at the specified 1-D (zero-offset) index.
        """
        func = self.get_dbus_method("getColumnAtIndex")
        return func(*args, **kwargs)
    
    def getColumnDescription(self, *args, **kwargs):
        """
        Get a text description of a particular table column. This differs
        from AccessibleTable_getColumnHeader, which returns an Accessible.
        @param : column
        the specified table column, zero-indexed.
        @return a UTF-8 string describing the specified table column,
        if available.
        """
        func = self.get_dbus_method("getColumnDescription")
        return func(*args, **kwargs)
    
    def getColumnExtentAt(self, *args, **kwargs):
        """
        Get the number of columns spanned by the table cell at the specific
        row and column. (some tables can have cells which span multiple
        rows and/or columns).
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer indicating the number of columns spanned
        by the specified cell.
        """
        func = self.get_dbus_method("getColumnExtentAt")
        return func(*args, **kwargs)
    
    def getColumnHeader(self, *args, **kwargs):
        """
        Get the header associated with a table column, if available,
        as an instance of Accessible. This differs from getColumnDescription,
        which returns a string.
        @param : column
        the specified table column, zero-indexed.
        @return an Accessible representatin of the specified table column,
        if available.
        """
        func = self.get_dbus_method("getColumnHeader")
        return func(*args, **kwargs)
    
    def getIndexAt(self, *args, **kwargs):
        """
        Get the 1-D child index corresponding to the specified 2-D row
        and column indices. 
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer which serves as the index of a specified
        cell in the table, in a form usable by Accessible::getChildAtIndex.
        """
        func = self.get_dbus_method("getIndexAt")
        return func(*args, **kwargs)
    
    def getRowAtIndex(self, *args, **kwargs):
        """
        Get the table row index occupied by the child at a particular
        1-D child index.
        @param : index
        the specified child index, zero-indexed.
        @return a long integer indicating the first row spanned by the
        child of a table, at the specified 1-D (zero-offset) index.
        """
        func = self.get_dbus_method("getRowAtIndex")
        return func(*args, **kwargs)
    
    def getRowColumnExtentsAtIndex(self, *args, **kwargs):
        """
        Given a child index, determine the row and column indices and
        extents, and whether the cell is currently selected. If the child
        at index is not a cell (for instance, if it is a summary, caption,
        etc.), False is returned.
        @param : index
        the index of the Table child whose row/column extents are requested.
        @param : row
        back-filled with the first table row associated with the cell
        with child index index. 
        @param : col
        back-filled with the first table column associated with the cell
        with child index index. 
        @param : row_extents
        back-filled with the number of table rows across which child
        i extends. 
        @param : col_extents
        back-filled with the number of table columns across which child
        i extends. 
        @param : is_selected
        a boolean which is back-filled with True if the child at index
        i corresponds to a selected table cell, False otherwise.
        Example: If the Table child at index '6' extends across columns
        5 and 6 of row 2 of a Table instance, and is currently selected,
        then retval=table::getRowColumnExtentsAtIndex(6,row,col,
        row_extents,
        col_extents,
        is_selected);
         will return True, and after the call row, col, row_extents,
        col_extents, and is_selected will contain 2, 5, 1, 2, and True,
        respectively.
        @return True if the index is associated with a valid table cell,
        False if the index does not correspond to a cell. If False is
        returned, the values of the out parameters are undefined.
        """
        func = self.get_dbus_method("getRowColumnExtentsAtIndex")
        return func(*args, **kwargs)
    
    def getRowDescription(self, *args, **kwargs):
        """
        Get a text description of a particular table row. This differs
        from AccessibleTable_getRowHeader, which returns an Accessible.
        @param : row
        the specified table row, zero-indexed.
        @return a UTF-8 string describing the specified table row, if
        available.
        """
        func = self.get_dbus_method("getRowDescription")
        return func(*args, **kwargs)
    
    def getRowExtentAt(self, *args, **kwargs):
        """
        Get the number of rows spanned by the table cell at the specific
        row and column. (some tables can have cells which span multiple
        rows and/or columns).
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer indicating the number of rows spanned
        by the specified cell.
        """
        func = self.get_dbus_method("getRowExtentAt")
        return func(*args, **kwargs)
    
    def getRowHeader(self, *args, **kwargs):
        """
        Get the header associated with a table row, if available. This
        differs from getRowDescription, which returns a string.
        @param : row
        the specified table row, zero-indexed.
        @return an Accessible representatin of the specified table row,
        if available.
        """
        func = self.get_dbus_method("getRowHeader")
        return func(*args, **kwargs)
    
    def getSelectedColumns(self, *args, **kwargs):
        """
        Obtain the indices of all columns which are currently selected.
        @return a sequence of integers comprising the indices of columns
        currently selected.
        """
        func = self.get_dbus_method("getSelectedColumns")
        return func(*args, **kwargs)
    
    def getSelectedRows(self, *args, **kwargs):
        """
        Obtain the indices of all rows which are currently selected.
        @return a sequence of integers comprising the indices of rows
        currently selected.
        """
        func = self.get_dbus_method("getSelectedRows")
        return func(*args, **kwargs)
    
    def isColumnSelected(self, *args, **kwargs):
        """
        Determine whether a table column is selected. 
        @param : column
        the column being queried.
        @return True if the specified column is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isColumnSelected")
        return func(*args, **kwargs)
    
    def isRowSelected(self, *args, **kwargs):
        """
        Determine whether a table row is selected. 
        @param : row
        the row being queried.
        @return True if the specified row is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isRowSelected")
        return func(*args, **kwargs)
    
    def isSelected(self, *args, **kwargs):
        """
        Determine whether the cell at a specific row and column is selected.
        @param : row
        a row occupied by the cell whose state is being queried. 
        @param : column
        a column occupied by the cell whose state is being queried.
        @return True if the specified cell is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isSelected")
        return func(*args, **kwargs)
    
    def removeColumnSelection(self, *args, **kwargs):
        """
        Remove the specified column from current column selection, if
        the table's selection model permits it.
        @param : column
        @return True if the specified column was successfully de-selected,
        False if not.
        """
        func = self.get_dbus_method("removeColumnSelection")
        return func(*args, **kwargs)
    
    def removeRowSelection(self, *args, **kwargs):
        """
        Remove the specified row from current row selection, if the table's
        selection model permits it.
        @param : row
        @return True if the specified row was successfully de-selected,
        False if not.
        """
        func = self.get_dbus_method("removeRowSelection")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)
    
    def unImplemented5(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented5")
        return func(*args, **kwargs)
    
    def unImplemented6(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented6")
        return func(*args, **kwargs)
    
    def unImplemented7(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented7")
        return func(*args, **kwargs)
    
    def get_caption(self):
        self._pgetter(self._dbus_interface, "caption")
    def set_caption(self, value):
        self._psetter(self._dbus_interface, "caption", value)
    _captionDoc = \
        """
        An Accessible which represents of a caption for a Table.
        """
    caption = property(fget=get_caption, fset=set_caption, doc=_captionDoc)
    
    def get_nColumns(self):
        self._pgetter(self._dbus_interface, "nColumns")
    def set_nColumns(self, value):
        self._psetter(self._dbus_interface, "nColumns", value)
    _nColumnsDoc = \
        """
        The total number of columns in this table (including empty columns),
        exclusive of columns which are programmatically hidden. Columns
        which are scrolled out of view or clipped by the current viewport
        are included.
        """
    nColumns = property(fget=get_nColumns, fset=set_nColumns, doc=_nColumnsDoc)
    
    def get_nRows(self):
        self._pgetter(self._dbus_interface, "nRows")
    def set_nRows(self, value):
        self._psetter(self._dbus_interface, "nRows", value)
    _nRowsDoc = \
        """
        The total number of rows in this table (including empty rows),
        exclusive of any rows which are programmatically hidden. Rows
        which are merely scrolled out of view are included.
        """
    nRows = property(fget=get_nRows, fset=set_nRows, doc=_nRowsDoc)
    
    def get_nSelectedColumns(self):
        self._pgetter(self._dbus_interface, "nSelectedColumns")
    def set_nSelectedColumns(self, value):
        self._psetter(self._dbus_interface, "nSelectedColumns", value)
    _nSelectedColumnsDoc = \
        """
        The number of columns currently selected. A selected column is
        one in which all included cells are selected.
        """
    nSelectedColumns = property(fget=get_nSelectedColumns, fset=set_nSelectedColumns, doc=_nSelectedColumnsDoc)
    
    def get_nSelectedRows(self):
        self._pgetter(self._dbus_interface, "nSelectedRows")
    def set_nSelectedRows(self, value):
        self._psetter(self._dbus_interface, "nSelectedRows", value)
    _nSelectedRowsDoc = \
        """
        The number of rows currently selected. A selected row is one
        in which all included cells are selected.
        """
    nSelectedRows = property(fget=get_nSelectedRows, fset=set_nSelectedRows, doc=_nSelectedRowsDoc)
    
    def get_summary(self):
        self._pgetter(self._dbus_interface, "summary")
    def set_summary(self, value):
        self._psetter(self._dbus_interface, "summary", value)
    _summaryDoc = \
        """
        An accessible object which summarizes the contents of a Table.
        This object is frequently itself a Table instance, albeit a simplified
        one.
        """
    summary = property(fget=get_summary, fset=set_summary, doc=_summaryDoc)




BUTTON_PRESSED_EVENT = EventType(2)

BUTTON_RELEASED_EVENT = EventType(3)

KEY_PRESS = KeySynthType(0)

KEY_PRESSED = KeyEventType(0)

KEY_PRESSED_EVENT = EventType(0)

KEY_PRESSRELEASE = KeySynthType(2)

KEY_RELEASE = KeySynthType(1)

KEY_RELEASED = KeyEventType(1)

KEY_RELEASED_EVENT = EventType(1)

KEY_STRING = KeySynthType(4)

KEY_SYM = KeySynthType(3)


MODIFIER_ALT = ModifierType(3)

MODIFIER_CONTROL = ModifierType(2)

MODIFIER_META = ModifierType(4)

MODIFIER_META2 = ModifierType(5)

MODIFIER_META3 = ModifierType(6)

MODIFIER_NUMLOCK = ModifierType(7)

MODIFIER_SHIFT = ModifierType(0)

MODIFIER_SHIFTLOCK = ModifierType(1)

